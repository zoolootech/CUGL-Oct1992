/* MAKEIO.C -- Input/Output and messaging for MAKE.C
 * 
 * For information on compilation, see MAKE.C.  For usage, see
 * MAKE.DOC
 *
 * CREDITS:
 * 
 * -- getline(), getblock(), stov(), err(), and serr() functions by
 *    Allen Holub (DDJ #106).
 * -- prompt(), batch(), addfile(), push(), and pop() functions 
 *    by James Pritchett.
 * -- All code by Allen Holub adapted for BDS C (where necessary)
 *    by James Pritchett.
 *
 * Version 1.0 -- 10/28/85
 * Version 1.1 -- 12/06/85
 */

#include <bdscio.h>
#include "make.h"


void prompt()   /* This is an escape hatch */
{
    printf("\nHit any key to execute commands (^C aborts)");
    getchar();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void batch()    /* Make the batch file, getting command lines from
                 * the command stack.
                 */
{
    int i;              /* Counter for $$$.SUB records */
    char *com;          /* Pointer to command line */
    char subrec[128];   /* SUB file record */
    int fd;             /* For disk I/O calls */


/* In order to batch SUBMITs, first check to see if A:$$$.SUB currently
 * exists.  If so, seek to the end of file, and append new commands.
 * If A:$$$.SUB does not exist, create it.
 */

    if ((fd = open("A:$$$.SUB",1)) != ERROR)
        seek(fd,0,2);
    else if ((fd = creat("A:$$$.SUB")) == ERROR)
        err("Can't create A:$$$.SUB\n",0);

/* NOTE: In a pending SUBMIT file, the commands must be in reverse
 * order.  Hence, commands are stored in a stack, with the commands
 * pushed in the proper order, and thus popped in reverse order.
 * Also, pending SUBMIT files must have their commands in a 
 * particular format: Each command must be preceded by a count of
 * the number of chars in it; the commands must be null-terminated;
 * and each command must be padded out to take up exactly one 
 * record (128 bytes) of the file.
 */

    while (com = pop()) {
        i = 0;
        subrec[i++] = strlen(com)+1;    /* put the length first */
        while (i < 127 && *com)         /* put the command line */
            subrec[i++] = *(com++);
        while (i < 128)                 /* pad with nulls */
            subrec[i++] = 0;
        if (write(fd,subrec,1) != 1)    /* write this record */
            err("Error in writing to A:$$$.SUB\n",0);
    }
    if(close(fd) == ERROR) 
        err("Error in closing A:$$$.SUB\n",0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

TNODE   *addfile(file)  /* Handle a file-dependancy.  This function
                         * makes a node for the filename, evaluates
                         * truth/falsity by means of an attribute
                         * bit, and sets that bit after evaluation.
                         */
char *file;             /* Filename */
{
    TNODE   *nodep;     /* Pointer to node made for this filename */
    char    fcb[36];    /* FCB for BDOS calls */
    char    *name;      /* Filename as exists in directory */
    int     entry;      /* Directory code returned from SEARCH_F call */
    int     i;
    char    curuser;    /* Current user area */
    char    fileuser;   /* User area of file */

/* Make the node for this file and place in tree. */

    nodep = gmem(NODESIZE);
    nodep->rnode = nodep->lnode = 0;
    nodep->being_made = file;
    nodep->depends_on = 0;
    nodep->do_this = 0;

#ifdef  DEBUG
debug("DEBUG addfile: Node made for %s\n",nodep->being_made);
#endif

    tree(nodep,&root);  /* place the node */


/* Handle any user area specified in filename */

    curuser = bdos(GS_USER,0x0FF);  /* Get current user */

/* If filename has a user spec (defined by '/'), set the new user
 * and increment file past the '/'.
 */

    if ((*(file+1) == '/') || (*(file+2) == '/')) {
        if ((fileuser = atoi(file)) > MAXUSER)
            fileuser = curuser;
        while (*(file++) != '/')
            ;
    }

/* If there is no user spec on filename, but there is a default user,
 * set it accordingly.
 */

    else if (defuser != UNKNOWN)
        fileuser = defuser;

/* If no user spec and no default user, just stay put. */

    else
        fileuser = curuser;

/* Now, set that user! */

    bdos(GS_USER,fileuser);


/* Find the file and check the attribute bit */

    bdos(SET_DMA,DMA);
    setfcb(fcb,file);   /* Make an FCB for the filename */
    if (!(*fcb))        /* If no drive name, use default */
        *fcb = defdsk;
    entry = bdos(SEARCH_F,fcb);
    if (entry == 255)           /* BDOS error code */
        serr("Dependancy file %s does not exist\n",file);
    else {
        name = DMA + (entry*32);    /* convert BDOS code to pointer */

/* If attribute is set, status = FALSE */

        if (*(name+MATTRIB) & 0x80) {
            nodep->changed = FALSE;

#ifdef  DEBUG
debug("DEBUG addfile: File %s not changed\n",fcb+1);
#endif

        }

/* Else, set status to TRUE and set bit again (-n and -r options
 * override the attribute set).
 */

        else {

#ifdef  DEBUG
debug("DEBUG addfile: File %s changed\n",fcb+1);
#endif

            nodep->changed = TRUE;
            if (!nopt && !ropt) {

/* In order to preserve any other attributes that may be set for this
 * file, the filename is copied from the directory (where attribute 
 * bits are set) to the FCB before setting MATTRIB.
 */

                for (i = 1; i <= 11; i++)
                    fcb[i] = *(name+i);
                fcb[MATTRIB] |= 0x80;

#ifdef PUBLIC
/* If using Plu*Perfect's public files system, the following must be
 * done to keep public files in their proper place.
 */
if (*name != fileuser)
    bdos(GS_USER,*name);
#endif

                bdos(SET_ATT,fcb);
            }
        }

/* Now, reset the user to curuser before ending */

        bdos(GS_USER,curuser);
        return nodep;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char *getline(maxline)      /* Get one logical line from the file.
                             * Strips CR-LF's and returns null-terminated
                             * string. If EOF is encountered, returns 0.
                             * Else, returns pointer to the line.
                             */
int maxline;        /* Maximum line length */
{
    char *bp;       /* Pointer to line buffer (external) */
    int c,lastc;

/* Read in a line, concatenating lines ending in '\' and terminating with
 * CR-LF or CPMEOF.  Line is placed in a temporary buffer of MAXLINE chars
 * (linbuf[]) and then copied into free memory in order to save space.
 */

    bp = linbuf;    
    while(1) {
        inputline++;
        for (lastc = 0; (c = getc(iobuff)) != CPMEOF 
                                    && c != '\r'; lastc = c) {
            if (--maxline > 0)
                *bp++ = toupper(c); /* All input folded to uppercase */
        }
        if (c == '\r')
            c = getc(iobuff);   /* skip the CR in CR-LF pair */
        if (!(c == '\n' && lastc == '\\'))
            break;              /* end of the line */
        else if (maxline > 0)   /* concatenate lines */
            --bp;
        else
            err("Line too long (%d chars maximum)",MAXLINE);
    }
    *bp = 0;    /* Terminate with null */


/* Allocate just enough space for the line and move it there.  EOF or
 * Out of memory returns 0
 */

    if ((c == CPMEOF && bp == linbuf) || !(bp = alloc((bp-linbuf)+1))) {
        return NULL;
    }
    strcpy(bp,linbuf);
    return bp;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char **getblock()       /* Get a block of lines ended by an empty line.
                         * Creates an array of pointers to those lines.
                         * This function is used to create the do_this
                         * arrays.
                         */
{
    char *p;                /* Pointer to line of text from makefile */
    char *lines[MAXBLOCK];  /* Local line vector */
    char **blockv;          /* Pointer to this vector */
    int  blockc;            /* Count of lines in vector */

    blockv = lines;
    blockc = 0;


/* Read lines from file until a blank line is encountered.  Skip over
 * all white space, and place pointer to first char of text into the
 * local line vector.  Abort if too many lines.
 */

    while (p = getline(MAXLINE)) {
        if (!(*p))
            break;
        skipwhite(p);
        if (++blockc <= MAXBLOCK)
            *blockv++ = p;
        else
            err("Action too long (max = %d lines)",MAXBLOCK);
    }

/* Allocate enough space for the line vector and put it there.
 * Terminate the vector with a null.
 */

    blockv = gmem((blockc + 1) * sizeof(blockv[0]));
    movmem(lines, blockv, blockc*sizeof(blockv[0]));
    blockv[blockc] = NULL;

    return blockv;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char **stov(str,maxvect)    /* Convert a string of tokens separated
                             * by white space to an array of pointers
                             * to the tokens, substituting nulls for
                             * the white space.
                             * This routine is used to create the
                             * depends_on vector in an object node.
                             */
char *str;                  /* String to convert */
int maxvect;                /* Maximum size of vector */
{
    char    **vect, **vp;   /* Pointers to the vector */

/* Allocate enough space for a worst-case scenario. */

    vp = vect = gmem((maxvect+1) * sizeof(str));

/* Parse the line, storing pointers in vector */

    while(*str && --maxvect >= 0) {
        skipwhite(str);
        *vp++ = str;
        skipnonwhite(str);
        if (*str)
            *str++ = 0; /* replace space with null */
    }
    *vp = 0;    /* null-terminate array */
    if (maxvect < 0)
        err("Too many dependancies! (%d maximum)",MAXDEP);
    return vect; 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void err(msg,param)     /* Error message.  Prints message with int
                         * param, then aborts.
                         */
char    *msg;
int param;
{
    printf("FATAL ERROR (%s line %d): ",filename,inputline);
    printf(msg,param);
    exit();
}

void serr(msg,param)    /* As above, except with a string param */
char *msg, *param;
{
    printf("FATAL ERROR (%s line %d): ",filename,inputline);
    printf(msg,param);
    exit();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void push(elem)         /* Push a stack element onto the stack */
char *elem;             /* Data to push */
{
    STACK   *elemptr;   /* Pointer to new stack element */

    elemptr = gmem(STACKSIZE);      /* Allocate space */
    elemptr->comline = elem;        /* Fill with data */
    elemptr->next = stackp;         /* Point to previous top */
    stackp = elemptr;               /* Reset stack pointer */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char *pop()             /* Pop element off stack and return the 
                         * data therein.
                         */
{
    STACK   *elemptr;   /* Pointer to top of stack */

    if (elemptr = stackp) {
        stackp = elemptr->next;     /* Set stackp to new top */
        return elemptr->comline;    /* And return data */
    }
    else                            /* If stack is empty, return NULL */
        return NULL;
}

/* end */


* * * * * * * * * * * * *