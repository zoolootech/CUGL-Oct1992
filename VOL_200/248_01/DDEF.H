/*	DDEF:	Global definitions for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

/* some global fuction declarations */

char *malloc();
char *strcpy();
char *strcat();
char *strncpy();
char *itoa();
FILE *popen();
WORD *getword();
char *flook();
char *nxtmword();
char *fgets();

#ifdef	CMPRS
char *gcword();
#endif

#ifdef	maindef

/* for MAIN.C */

int numwords = 0;			/* current # of source words loaded */
int totwords = 0;			/* total number of words processed */
int prowords = 0;			/* total processed words */
int badwords = 0;			/* number of mismatched words */
WORD *sword[MAXWORDS+1];		/* current source word list */

int outnum = -1;			/* current output source file # */
FILE *outfile = NULL;			/* mispelled word temp file */

int sfnum = -1;				/* current source file ordinal # */
FILE *srcfile = NULL;			/* current source file pointer */
int srcline = 0;			/* current source line number */
char iline[MAXLINE];			/* current input line */
char *iptr;				/* current ptr into line */

#if	CMPRS
char *mdfile = "dict.dct";		/* main dictionary text file */
#else
char *mdfile = "dict.txt";		/* main dictionary text file */
#endif
FILE *mdptr = NULL;			/* ptr to main dictionary */

char *comlist = "common.txt";		/* most common word list file */
char userlist[NFILEN] = "";		/* First user word lsit file */
int commonf = FALSE;			/* common dictionary loaded flag */
int numcom = 0;				/* # of common words */
int numfiltr = 0;			/* number of filter words */
char *cword[MAXCOM+1];			/* list of common words */

int numspell = 0;			/* number of files to spell */
char splname[MAXSPELL][NFILEN];		/* name of files to spell */

int (*comp)() = NULL;			/* current comparison function */
char hivalue[] = { 0xff, 0};		/* HIGH value string */

int swdebug = FALSE;			/* debugging flag */
int swemacs = FALSE;			/* use MicroEMACS to correct */
int swwords = FALSE;			/* output a word list */

#if	RAMSIZE
long envram = 0;			/* # of bytes malloced */
#endif
#else

/* for all the other .C files */

extern int numwords;			/* current # of source words loaded */
extern int totwords;			/* total number of words processed */
extern int prowords;			/* total processed words */
extern int badwords;			/* number of mismatched words */
extern WORD *sword[MAXWORDS];		/* current source word list */

extern int outnum;			/* current output source file # */
extern FILE *outfile;			/* mispelled word temp file */

extern int sfnum;			/* current source file ordinal # */
extern FILE *srcfile;			/* current source file pointer */
extern int srcline;			/* current source line number */
extern char iline[NSTRING];		/* current input line */
extern char *iptr;			/* current ptr into line */

extern char *mdfile;			/* main dictionary text file */
extern FILE *mdptr;			/* ptr to main dictionary */

extern char *comlist;			/* most common word list file */
extern char userlist[];			/* First user word lsit file */
extern int commonf;			/* common dictionary loaded flag */
extern int numcom;			/* # of common words */
extern int numfiltr;			/* number of filter words */
extern char *cword[];			/* list of common words */

extern int numspell;			/* number of files to spell */
extern char splname[MAXSPELL][NFILEN];	/* name of files to spell */

extern int (*comp)();			/* current comparison function */
extern char hivalue[];			/* HIGH value string */

extern int swdebug;			/* debugging flag */
extern int swemacs;			/* use MicroEMACS to correct */
extern int swwords;			/* output a word list */

#if	RAMSIZE
extern long envram;			/* # of bytes malloced */
#endif
#endif

