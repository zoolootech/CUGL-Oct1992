/* VIEW:  Displays text files. CR advances by line, SPACE by screen.
 * BS goes backwards by line, ESC by screen.  ^B returns to beginning,
 * ^E goes to end.  ^C to abort.
 * Syntax:  VIEW [-t] [n/][d:]filename.typ {more filenames . . .}
 * "t" = tab size (defaults to 8 spaces)
 * Multiple filenames on command line allowed.  Ambiguous filenames allowed
 * (see WILDEXP.C documentation).
 *
 * Version 1.5 -- 10/10/85
 * Version 1.51 (with slight changes for portability) -- 12/09/85
 *
 * James Pritchett
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Compiling instructions:
 *
 * VIEW was originally written to use memory-mapping for all video output
 * via an enhanced version of the BDS "txtplot" function.  The source for
 * txtplot() (version for the NEC PC-8801A computer) is given in the file 
 * TXTPLOT.CSM, and can easily be adapted for other systems supporting
 * memory-mapped video.  The file TXTPLOT.C contains a version of the same
 * function, written in C, that does NOT use memory-mapping.  See the 
 * file VIEW151.DOC for more info on these functions.
 * 
 * The commands to compile/link VIEW:
 *
 * WITH memory-mapping:
 *    SUBMIT CASM TXTPLOT
 *    CC VIEW 
 *    L2 VIEW TXTPLOT -L WILDEXP
 *
 * WITHOUT memory-mapping:
 *    CC TXTPLOT
 *    CC VIEW 
 *    L2 VIEW TXTPLOT -L WILDEXP
 *
 */ 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <bdscio.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

                    /* * * * * * * * * * * * * * *
                     * NON-USER DEFINED SYMBOLS  *
                     * * * * * * * * * * * * * * */

/* None of these symbols should need revision for porting to other systems */

/* For opening message */

#define VERSION   "1.51"
#define DATE      "10/10/85 (12/09/85)"

/* File buffer definitions */

/* NOTE:  If your TPA is too small to handle a 32K buffer, or
 * if your TPA is really big and can handle more, then you
 * should reset SEGBLKS to an appropriate number.
 */

#define SEGBLKS   128                   /* 128 recs per segment */
#define SEGSIZE   (SEGBLKS * SECSIZ)    /* segment = 16K */
#define FBLKS     (2 * SEGBLKS)         /* 2 segments at a time */
#define FSIZE     (2 * SEGSIZE)         /* 2 segments in buffer */
#define xseg      buffer                /* xseg at top of buffer */

/* Macros for buffer-handling */

#define atend() (*botptr == CPMEOF)
#define attop() ((topptr == xseg) && !curxseg)
#define topfault(p)  ((p < (xseg+2)) && curxseg)
#define botfault(p)  ((p == bufend) && !lastseg)

/* ASCII defines */

#define CNTLB   0x02
#define CNTLC   0x03
#define CNTLE   0x05
#define BS      0x08
#define LF      0x0a
#define CR      0x0d
#define CNTLN   0x0e
#define ESC     0x1b
#define MASK    0x7f                    /* for parity-strip */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

                /* * * * * * * * * * * * *
                 * USER-DEFINED SYMBOLS  *
                 * * * * * * * * * * * * */

/* The following should be customized for individual systems.  The 
 * values given here are for an NEC PC-8801A computer.
 */

/* Screen size: */

#define MAXROW      24          /* Number of rows on screen */
#define LASTROW     23          /* MAXROW-1 */

/* Memory-mapped video parameters: */

#define BASE        0x0f300     /* Video memory base address */
#define XSIZE       24          /* Number of lines in display */
#define YSIZE       120         /* Characters per line */

/* Macros for screen routines:
 * 
 * The following four functions are defined as macros: clear screen,
 * home cursor, move cursor to bottom right corner of screen, insert
 * blank line at cursor location.  These will probably have to be
 * customized for the proper escape/control sequences for a particular
 * terminal/computer.  If any of these are not available as simple
 * escape/control sequences, you may want to write functions to perform
 * the same actions.  Also, if you use the non-memory-mapped version of
 * TXTPLOT, you may want to define "cursbot" as a call to the "gotoxy"
 * function given in TXTPLOT.C.  Otherwise, this is the only "gotoxy"
 * call in the program, so a separate function is unnecessary.
 */

#define clear()     putch(CNTLZ)
#define home()      putch(0x01e)
#define cursbot()   putch(ESC); putch('='); putch('7'); putch('m')
#define lnins()     putch(ESC); putch('E')

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* global variables */

int     tabsiz;             /* Size of tabs (= 8 or given on command line) */
int     curxseg;            /* Number of first segment in memory */
int     lastseg;            /* = TRUE if last segment of file is in
                             * memory, FALSE if not.
                             */
int     fd;
int     lastrow;            /* Last row plotted on screen */
char    *topptr, *botptr;   /* Pointers into text buffer.  These point
                             * to the lines of text at top and bottom of
                             * screen.
                             */
char    *yseg;              /* Pointer to second buffer segment */
char    *bufend;            /* Pointer to end of buffer */
char    buffer[FSIZE];

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

main(argc,argv)
int  argc;
char **argv;
{
    char c;

    wildexp(&argc, &argv);
    printf("VIEW -- Version %s   James Pritchett, %s\n\n",VERSION,DATE);


/* If too few arguments, give a usage summary */

    if (argc < 2) {
        printf("USAGE: VIEW [-t] <list of files>\n");
        printf("       t = tab size (defaults to 8 spaces)\n");
        printf("       Ambiguous filenames allowed\n");
        exit();
    }


/* Tabsize argument must be first argument */

    if (*(argv[1]) == '-') {
        tabsiz = atoi(argv[1]+1);
        if (!tabsiz)                /* If error on atoi, set to default */
            tabsiz = 8;
        argc--;
        argv++;                     /* Point past the tab argument */
    }
    else
        tabsiz = 8;

    printf("\t<RET> = Forward line   <SPACE> = Forward screen\n");
    printf("\t<BS>  = Backward line  <ESC>   = Backward screen\n");
    printf("\t^B    = Top of file    ^E      = End of file\n");
    printf("\t^N    = Next file      ^C      = Exit program\n");


/* Initialize memory-mapped video */

    setplot(BASE,XSIZE,YSIZE);

/* Set up buffer pointers */

    bufend = buffer + FSIZE - 1;
    yseg = buffer + SEGSIZE;


/* Main event:  Get files and process commands */

    while (--argc) {
        if (getfile(*(++argv)) == ERROR)
            continue;       /* just skip bad filenames */
        while ((c = getchar()) != CNTLN) {
            switch (c) {
                case CR:     nextln();
                             break;
                case ' ':    nextscn();
                             break;
                case BS:     backln();
                             break;
                case ESC:    backscn();
                             break;
                case CNTLB:  tobegin();
                             break;
                case CNTLE:  toend();
                             break;
                case CNTLC:  exit();
            }
            cursbot();          /* Re-locate cursor after all commands */
        }
    close(fd);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int  getfile(fname)             /* Read in a file */
char *fname;
{

    topptr = 0;     /* reset this pointer when opening new file */
    if ((fd = open(fname,0)) == ERROR) {
        printf("\nERROR: Can't open file.\n");
        return(ERROR);  
    }
    printf("\nLoading %s . . .",fname);
    curxseg = 1;        /* this causes tobegin() to load the file */
    tobegin();          /* Load and display first screen */
    cursbot();          /* Locate cursor out of the way */
    return(0);          /* Return OK */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void fillbuff(seg)      /* Fill the buffer with two file segments, starting
                         * with "seg".
                         */
int seg;
{
    int     numread;

    lastseg = FALSE;    /* Start from this assumption */
    curxseg = seg;      /* First segment = seg */
    seek(fd,(seg * SEGBLKS),0);     /* find it */
    if ((numread = read(fd,buffer,FBLKS)) == ERROR) {
        printf("\nERROR: Disk read error.\n");
        exit();     /* all read errors crash the program */
    }

/* If we didn't fill the buffer, or if the last char is an EOF, then
 * this must be all there is, and the last segment of the file is
 * in memory.  So set the flag TRUE.
 */

    if ((numread != FBLKS) || (*bufend == CPMEOF))
        lastseg = TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void nextln()           /* Go forward one line */
{
    char    *temp;

    if (atend())
        return;
    putch(LF);      /* this causes a scroll up one line */
    temp = ptrdown(botptr);    /* be sure segfault is taken care of first */
    topptr = ptrdown(topptr);
    txtplot(botptr,LASTROW,0,tabsiz);  /* if there was a segfault,  */
    botptr = temp;               /* botptr was revised */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void nextscn()      /* Go forward one screen */
{
    if (atend())
        return;
    topptr = botptr;    /* botptr => next topline */
    dispscn();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void backln()           /* Go back one line */
{
    if (attop())
        return;
    home();
    lnins();        /* a scroll down */
    topptr = ptrup(topptr);
    if (lastrow < MAXROW)  /* bug fix: if last screen has < 24 lines, botptr */
        lastrow++;     /*          should remain unchanged               */
    else
        botptr = ptrup(botptr);
    txtplot(topptr,0,0,tabsiz);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void backscn()          /* Go backward one screen */
{
    int x;

    if (attop())
        return;
    for (x = 0; x < MAXROW && !attop(); x++)
        topptr = ptrup(topptr);     /* move back 24 lines or to beginning */
    dispscn();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tobegin()          /* Go to beginning of file */
{
    if (attop())
        return;
    if (curxseg)        /* seek to beginning if segment 0 is not in memory */
        fillbuff(0);
    topptr = buffer;
    dispscn();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void toend()            /* Go to end of file */
{
    int x, lastxseg;

    if (atend())
        return;
    if (!lastseg) {     /* seek to end if not in memory */
        lastxseg = ((cfsize(fd) - 1)/SEGBLKS) - 1;
        fillbuff(lastxseg);
        botptr = yseg;
    }
    while (!atend())        /* find the bottom of file first */
        botptr = ptrdown(botptr);
    topptr = botptr;
    for (x = 0; x < MAXROW && !attop(); x++)
        topptr = ptrup(topptr);    /* then figure topline from there */
    dispscn();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char *ptrdown(ptr)          /* Move a pointer down one line in buffer */
char *ptr;
{
    while (*ptr != CPMEOF) {
        if (botfault(ptr)) {    /* if we have more to read, then do so */
            ptr = bufup(ptr);
            readseg(yseg);
        }
        if (((*ptr++) & MASK) == LF)  /* strip parity */
            break;
    }
    return ptr;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char *ptrup(ptr)        /* Move a pointer up one line in buffer */
char *ptr;
{
    if (topfault(ptr)) {   /* we have to check before skipping back */
        ptr = bufdn(ptr);  /* over LF   */
        readseg(xseg);
    }
    ptr--;          /* go back past LF */
    while ((*(ptr - 1) & MASK) != LF && (ptr != buffer)) {
        if (topfault(ptr)) {    /* and check while going back */
            ptr = bufdn(ptr);
            readseg(xseg);
        }
        ptr--;
    }
    return ptr; 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void dispscn()      /* Fill the screen, starting from *topptr */
{
    char *temp;

/* note: this routine displays MAXROW lines, or as many as remain in 
 * file, whichever is less, starting with the line pointed to by
 * topptr.  It locates the next line, THEN displays the current
 * line, so as to force any segment faults before displaying the
 * line in which that fault occurred.  temp points to the next line,
 * botptr to the current line.  If a segfault occurs, botptr will
 * be adjusted as necessary by bufup(), thus assuring the proper
 * current line will be displayed.  At the conclusion of this
 * routine, botptr is pointing to the next line in the file, or
 * to the end of file, and lastrow = the number of lines currently
 * on the screen (botptr should not be moved up until MAXROW lines are
 * on the screen).
 */

    clear();
    for (lastrow = 0, botptr = topptr; lastrow < MAXROW && !atend();
                 lastrow++, botptr = temp) {
        temp = ptrdown(botptr);
        txtplot(botptr,lastrow,0,tabsiz);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char getchar()   /* gets a char with no echo */
{
    char c;

    while (!(c = bdos(6,-1)))
        ;
    return c;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char *bufup(p)      /* Move yseg into xseg position */
char *p;
{
    movmem(yseg,xseg,SEGSIZE);
    p -= SEGSIZE;       /* revise all pointers */
    topptr -= SEGSIZE;
    botptr -= SEGSIZE;
    return(p);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char *bufdn(p)      /* Move xseg into yseg position */
char *p;
{
    movmem(xseg,yseg,SEGSIZE);
    p += SEGSIZE;       /* revise all pointers */
    topptr += SEGSIZE;
    botptr += SEGSIZE;
    return(p);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void readseg(segstart)  /* routine to read in a new segment */
char *segstart;     /* segstart is where the segment is to go */
{
    int firstrec, numread;

    if (segstart == xseg) {  /* if we're reading in a new xseg, */
        curxseg--;           /* then we're scrolling backwards */
        firstrec = curxseg * SEGBLKS;
    }
    else {      /* if we're reading a yseg, then we're scrolling forwards */
        curxseg++;
        firstrec = (curxseg + 1) * SEGBLKS;
    }

    seek(fd,firstrec,0);
    if ((numread = read(fd,segstart,SEGBLKS)) == ERROR) {
        printf("\nERROR: Disk read error.\n");
        exit();     /* all read errors crash the program */
    }
    if ((numread != SEGBLKS) || (*bufend == CPMEOF))
        lastseg = TRUE;
}

/* end */

  movmem(xseg,yseg,SEGSIZE);
    p +=