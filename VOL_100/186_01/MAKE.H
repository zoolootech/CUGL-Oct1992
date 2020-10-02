/* MAKE.H -- Header file for MAKE utility.
 * 
 * Based on code by Allen Holub (DDJ, #106).  Revised and
 * adapted for BDS C and CP/M-80 by James Pritchett.
 *
 * Version 1.0 -- 10/28/85
 * Version 1.1 -- 12/06/85
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */

            /* * * * * * * * * * *
             * COMPILER SWITCHES *
             * * * * * * * * * * */

/* If the following symbol is defined, then debugging code is
 * activated.  Debug routines are given in the file MDEBUG.C.
 */

/*
#define DEBUG   1
*/

/* If using the Plu*Perfect public files system, the following
 * symbol should be defined in order to cause compilation of
 * code necessary to handle public files correctly.
 */

#define PUBLIC 1

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

                /* * * * * * * * * * * *
                 * STRUCT DEFINITIONS  *
                 * * * * * * * * * * * */

/* Definition of tree node structure for objects: */

struct _tn {
    struct _tn  *lnode;         /* Left branch */
    struct _tn  *rnode;         /* Right branch */
    char        *being_made;    /* Name of object */
    char        **depends_on;   /* Pointers to dependancies */
    char        **do_this;      /* Pointers to actions */
    int         changed;        /* Action flag: 
                                 * = -1 if UNKNOWN
                                 * =  0 if unchanged/made
                                 * =  1 if changed/unmade
                                 */
} ;
#define TNODE       struct _tn  /* Pseudo-typedef */
#define NODESIZE    12          /* Size of a _tn struct */


/* Definition of stack element structure for command stack: */

struct _stack {
    char            *comline;   /* Pointer to command line */
    struct _stack   *next;      /* Next struct in stack */
} ;
#define STACK       struct _stack
#define STACKSIZE   4

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

            /* * * * * * * * * * * * * * * * *
             *    USER-DEFINABLE SYMBOLS     *
             * * * * * * * * * * * * * * * * */

/* The following four symbols define the various limits imposed
 * on a makefile.  These may be freely changed to suit your needs.
 */

#define MAXLINE     300     /* Maximum line length */
#define MAXBLOCK    10      /* Maximum actions per object */
#define MAXDEP      20      /* Maximum dependancies per object */
#define MAXUSER     15      /* Maximum user area on system */

/* MATTRIB defines the file attribute bit that is used by MAKE to
 * determine a file's status (changed/unchanged).  Any unused
 * attribute may be used (i.e., attributes 1-8 or 11).
 */

#define MATTRIB     11

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

                /* * * * * * * * * *
                 * OTHER CONSTANTS *
                 * * * * * * * * * */

#define COMMENT     '#'     /* Comment line character */
#define UNKNOWN     -1      /* Unknown status value for objects */

#define DMA         0x080   /* For the file searching routine */
#define SEARCH_F    17      /* BDOS calls */
#define SET_DMA     26
#define SET_ATT     30
#define GS_USER     32

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

                    /* * * * * *
                     * MACROS  *
                     * * * * * */

/* Macros for line parsing */

#define iswhite(c)  ((c) == ' ' || (c) == '\t')
#define skipwhite(s)    while( iswhite(*s) ) ++s
#define skipnonwhite(s) while( *s && !iswhite(*s) ) ++s

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

                /* * * * * * * * * * *
                 * GLOBAL VARIABLES  *
                 * * * * * * * * * * */


TNODE   *root;              /* Root of the object tree */
char     iobuff[BUFSIZ];    /* File buffer */
char    *filename;          /* Makefile filename */
int      inputline;         /* Makefile line number for error messages */
char    *first;             /* Target name */
char     defdsk;            /* Default disk for searches */
int      defuser;           /* Default user area for searches */
char     linbuf[MAXLINE];   /* For reading raw input lines */
STACK   *stackp;            /* Stack pointer */
int      aopt,nopt,qopt,ropt,topt;  /* options flags */

/* end */

*
                     * * * * * */

/* Macros for line parsing */

#define iswhite(c)  ((c) == ' ' || (c) == '\t')
#d