/* MAKE: CP/M version of the UNIX utility.  Compiles and links files
 * as necessary, based on dependancy information in a "makefile".
 * For usage, see MAKE.DOC.
 *
 * This code is an adaptation and expansion of the program "mk" given
 * by Allen Holub in Dr. Dobb's Journal, August 1985 (#106).  
 *
 * To compile:
 *      CC MAKE -E3000
 *      CC MAKEIO -E3000
 *      L2 MAKE MAKEIO
 *
 * If DEBUG is #defined in MAKE.H, then the file MDEBUG.C must be
 * compiled and linked in.
 *
 * CREDITS:
 * 
 * -- dependancies(), tree(), find(), and gmem() functions by 
 *    Allen Holub (DDJ #106).
 * -- main(), make(), and makenode() functions based on code by
 *    Allen Holub (DDJ #106), with significant revisions by
 *    James Pritchett.
 * -- init() and macro() functions by James Pritchett.
 * -- All code by Allen Holub adapted for BDS C (where necessary)
 *    by James Pritchett.
 *
 * Version 1.0 -- 10/28/85
 * Version 1.1 -- 12/06/85
 */

#include <bdscio.h>
#include "make.h"

#define VERSION "1.1"
#define DATE    "12/06/85"

main(argc,argv)
int     argc;
char    **argv;
{
    init(argc,argv);    /* messages, variable initializers, command parsing */

    if (fopen(filename,iobuff) == ERROR)    /* Open up the makefile */
        serr("Can't open %s\n",filename);
    if (!dependancies())                    /* Build the object tree */
        err("File empty!\n",0);
    else {
        if (!qopt) {
            printf("Making target object -> %s\n",first);
            printf("Commands needed:\n");
        }
        make(first);                        /* Evaluate all objects */
        if (stackp && !nopt && !topt) {     /* Batch the commands */
            if (!qopt)
                prompt();
            batch();
        }
        else if (!stackp)
            printf("\tNothing needs making\n");
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void init(argc,argv)    /* Opening message, help message, variable
                         * initialization, and command line parsing.
                         */
int     argc;
char    **argv;
{
    char c;
    int i;

    printf("\nMAKE  Version %s   James Pritchett, %s\n\n",VERSION,DATE);

/* Answer a cry for help */

    if (*argv[1] == '?') {
        printf("Usage: MAKE [target] [-options]\n");
        printf("Makes target or first in file ");
        printf("(if target not specified)\n\n");
        printf("\tOptions:\n");
        printf("\t\t-f filename: Use filename as makefile\n");
        printf("\t\t-a:          Make all targets\n");
        printf("\t\t-n:          Make no targets\n");
        printf("\t\t-q:          Supress all messages\n");
        printf("\t\t-r:          Do not set file attributes\n");
        printf("\t\t-t:          Set file attributes, but do not make\n");
        exit();
    }

/* Global variable initialization */

    root = NULL;    /* These should be zeroed anyway, */
    first = NULL;   /*    but better safe than sorry  */
    inputline = 0;
    stackp = NULL;
    aopt = nopt = qopt = ropt = topt = FALSE;
    filename = "MAKEFILE";      /* Default filename for makefile */
    defdsk = 0;
    defuser = UNKNOWN;

/* Parse the command line, beginning with argv[1] */

    i = 1;

/* If the first arg is not an option, it must be the target */

    if ((--argc) && (*argv[1] != '-')) {
        first = argv[1];
        argc--;
        i++;
    }

/* Parse the options */

    while (argc--) {
        if (*argv[i] != '-')    /* Ignore anything that ain't an option */
            continue;
        switch (c = *(argv[i++]+1)) {
            case 'A':
                aopt = TRUE;
                break;
            case 'N':
                nopt = TRUE;
                break;
            case 'Q':
                qopt = TRUE;
                break;
            case 'R':
                ropt = TRUE;
                break;
            case 'T':
                topt = TRUE;
                break;
            case 'F':           /* Alternate makefile name follows */
                argc--;
                filename = argv[i++];
                break;
            default:
                printf("ERROR: Unknown option -%c ignored.\n",c);
                break;
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void make(target)       /* Main routine for making.  This function
                         * is recursive: it calls itself if it finds
                         * another object listed in the dependancy_list.
                         */
char *target;   /* Name of object to make */
{
    TNODE   *snode;     /* current target node */
    TNODE   *dnode;     /* dependancy node */
    char    **linev;    /* for handling dependancy vector */

#ifdef  DEBUG
debug("DEBUG make: making <%s>\n",target);
#endif


/* Find the node for this object and abort if not found */

    if (!(snode = find(target,root)))
        serr("Don't know how to make <%s>\n",target);
    snode->changed = FALSE;     /* start from unchanged */


/* Evaluate all the dependancies */

    for(linev = snode->depends_on; *linev; linev++) {

#ifdef  DEBUG
debug("    DEBUG make: Dependancy = %s\n",*linev);
#endif

/* Find the node for the dependancy.  If the dependancy is not
 * in the tree, it must be a file, and should be evaluated and
 * added to the tree.
 */

        if (!(dnode = find(*linev,root))) {
            dnode = addfile(*linev);
        }

/* If the state of the dependancy is UNKNOWN, then it must be
 * an object that has yet to be made.  If so, then make the
 * dependancy first.
 */
        if (dnode->changed == UNKNOWN) {
            make(*linev);
        }

/* If the status of the dependancy is TRUE, then set the object
 * status to TRUE.
 */
        if (dnode->changed) {
            (snode->changed)++;

#ifdef  DEBUG
debug("\tDEBUG make (%s): Dependency state TRUE\n",target);
#endif

        }
    } /* Close of "for" -- Loop for all dependancies in list. */


/* If, after evaluating dependancies, the object has been set TRUE,
 * then push the actions onto the stack.  If -a option has been
 * specified, push the actions no matter what the object's state.
 */
    if (snode->changed || aopt) {

#ifdef  DEBUG
debug("DEBUG make (%s): Pushing actions\n",target);
#endif

        for (linev = snode->do_this; *linev; linev++) {
            if (!qopt)  /* skip the echo if -q is given */
                printf("\t%s\n",*linev);
            push(*linev);
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int dependancies()  /* Build the objectname tree */
{
    TNODE   *node;

/* Set up root of tree */

    if (node = makenode()) {

/* If a target was not given on the command line, then use the first
 * object in the makefile as the target.
 */
        if (!first)
            first = node->being_made;
        if (!tree(node,&root))
            err("Can't insert first node into tree.\n");

/* Makes nodes for all other objects and place into tree. */

        while (node = makenode()) {
            if (!tree(node,&root))
                free(node);
        }

#ifdef  DEBUG
printf("DEBUG dependancies: Tree made.\n\n");
trav(root);     /* Print tree */
#endif

        fclose(iobuff);
        return TRUE;
    }
    return FALSE;   /* Return FALSE if the makefile is empty */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int tree(node,rootp)    /* Place a node into the tree.  This function
                         * is recursive, calling itself with the left
                         * or right branches of the tree until it
                         * finds the appropriate spot.
                         */
TNODE   *node;          /* Pointer to node to place in tree */
TNODE   **rootp;        /* Pointer to pointer to current node in 
                         * the tree traversal.
                         */
{
    int notequal;       /* For evaluating < and > */

/* If the branch we were passed is a leaf node, then this is where
 * the node goes.
 */

    if (*rootp == NULL) {
        *rootp = node;
        return TRUE;
    }

/* Otherwise, if the objectnames are equal, don't do anything
 * and return FALSE so that the space allocated for this node
 * will be freed up.
 */

    if (!(notequal = strcmp((*rootp)->being_made, node->being_made)))
        return FALSE;

/* Else, call tree() again with either the left (if <) or right (if >)
 * branches.
 */
    return(tree(node,(notequal > 0) ? &((*rootp)->lnode)
                                    : &((*rootp)->rnode)));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

TNODE   *find(key,root) /* Find an object in the tree.  This function
                         * is recursive, in a manner similar to tree()
                         */
char *key;              /* Object name to find */
TNODE *root;            /* Current node in tree traversal */
{
    int notequal;

/* If the root passed was a leaf node, then find has failed */

    if (!root)
        return FALSE;

/* Else, if objectnames are same, this is a match */

    if (!(notequal = strcmp(root->being_made,key))) {
        return(root);
    }

/* Else, call again with left or right branch. */

    return(find(key,(notequal > 0) ? root->lnode : root->rnode));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

TNODE   *makenode()     /* Make an object node */
{
    char    *line;      /* Line of text from makefile */
    char    *lp;        /* 2nd pointer to text line */
    TNODE   *nodep;     /* Pointer to node made */


/* Get the first line of the next statement in makefile */


/* Skip blank lines, comments, and macros. Return on EOF. */

    do {
        if ((line = getline(MAXLINE)) == NULL)  /* End of file */
            return NULL;
        if (*line == '$')               /* Handle a macro */
            macro(line+1);
    } while (!(*line) || *line == COMMENT || *line == '$');


/* Be sure there's a colon after the objectname */
 
    for (lp = line; *lp && *lp != ':'; lp++)
        ;
    if (*lp != ':')
        err("Missing ':'",0);


/* Skip white space and set lp to point to the dependancies */

    else
        for(*lp++ = 0; iswhite(*lp); lp++)
            ;


/* Allocate space for the node, and fill 'er up */

    nodep = gmem(NODESIZE);
    nodep->lnode = nodep->rnode = 0;
    nodep->being_made = line;
    nodep->changed = UNKNOWN;
    nodep->depends_on = stov(lp,MAXDEP);
    nodep->do_this = getblock();

#ifdef  DEBUG
debug("DEBUG makenode: Node made for %s\n",nodep->being_made);
#endif

    return nodep;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char    *gmem(numbytes) /* Allocates memory and aborts on error */
int numbytes;   /* Space to allocate */
{
    char *p;

    if (!(p = alloc(numbytes)))
        err("Out of memory.",0);
    return p;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void macro(line)        /* Handle macros. Only macros currently
                         * supported are for default disk ($:d)
                         * and default user ($/).
                         */
char    *line;          /* Body of macro */
{
    switch(*(line++)) {
        case ':':                             /* Default disk */
            if (*line >= 'A' && *line <= 'P')
                defdsk = *line - 'A' + 1;
            break;
        case '/':                             /* Default user */
            if ((defuser = atoi(line)) > MAXUSER)
                defuser = UNKNOWN;
            break;
        default:
            printf("ERROR: Unknown macro $%s ignored.\n",line-1);
            break;
    }
}

/* end */

Allocates memor