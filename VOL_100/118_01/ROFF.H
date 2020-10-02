/* software tools format program -- C version -- header file
 * source:  roff.h
 * version: November 26, 1981.
 */

/* contains the following common blocks:

	ctemp
	cparam
	cpage
	cout
	cnr
	cfiles
	cdefio
	cmac
*/


/*     define PAGECONTROL if you want format to output a
 *     page eject character (CNTRL-L) rather than count
 *     lines to finish off a page.
 */

/* comment out -----
#define PAGECONTROL 1
----- end comment out */


/* define size of important data objects */

#define MAXLINE 400
#define MAXDEF  400
#define MAXNAME 400
#define	INSIZE	400 
#define	MAXOUT	400 
#define PBSIZE  400

/* define special characters visible to the user */

#define ARGFLAG '$'
#define	PAGENUM	'#'
#define	CURRENTDATE '%'

/* define default settings of parameters */

#define	PAGEJECT FF	/* FF is ASCII formfeed (control-L) */
#define	PAGEWIDTH 65 
#define	PAGELEN	66 

/* define return codes and other values */

#define	LEFT	1 
#define	CENTER	2 
#define	RIGHT	3 
#define	STARTU	1	/* start underscoring */
#define	STOPU	2	/* stop underscoring */


/* these variable were originally local variables
 * initialized with data statements.
 */

int  cuflag;		/* cuflg in text() */
int  dir;		/* dir    in spread() */
int  putflag;		/* cuflg in put() */

/* temporary buffers -- from common block ctemp */

char tbuf1 [MAXLINE];	/* for use by puttl and tabs */
char tbuf2 [MAXLINE];
char ttl   [MAXLINE];


/* misc. line info -- from common block cparam */

int  fill;	/* flag: fill; init = YES */
int  lsval;	/* line spacing; init = 1 */
int  inval;	/* indent; >= 0; init = 0 */
int  rmval;	/* right margin; init = PAGEWIDTH = 60 */
int  tival;	/* current temporary indent; init = 0 */
int  ceval;	/* number of lines to center; init = 0 */
int  ulval;	/* number of lines to underline; init = 0 */
int  boval;	/* number of lines to boldface; init = 0 */
char cchar;	/* line control character; init = PERIOD */
int  tjust [3];	/* justification types for heads and foots */
		/* init = LEFT, CENTER, RIGHT */
int  bsval;	/* number of lines to blank suppress; init=0 */
int  rjust;	/* flag: right justify filled lines */
		/* init = YES */
int  cuval;	/* number of lines to continuously underline */
		/* init = 0 */
int  tabs [INSIZE];    /* tab stops; init every 8 spaces */


/* misc. page info -- from common block cpage */

int  curpag;	/* current output page number; init = 0 */
int  newpag;	/* next output page number; init = 1 */
int  lineno;	/* next line to be printed; init = 0 */
int  plval;	/* page length in lines; init = PAGELEN = 66 */
int  m1val;	/* margin before and including header */
int  m2val;	/* margin after header */
int  m3val;	/* margin after last text line */
int  m4val;	/* bottom margin, including footer */
int  bottom;	/* last live line on page */
		/* always equal to plval - m3val - m4val */
char ehead [MAXLINE];	/* top of page title for even pages */
			/* init = NEWLINE */
char ohead [MAXLINE];	/* top of page title for odd  pages */
int  ehlim [2];		/* left,right margins for even header */
			/* init = inval, rmval */
int  ohlim [2];		/* left,right margins for odd  header */
			/* init = inval, rmval */
char efoot [MAXLINE];	/* bot of page title for even pages */
			/* init = NEWLINE */
char ofoot [MAXLINE];	/* bot of page title for odd  pages */
			/* init = NEWLINE */
int  eflim [2];		/* left,right margins for even footer */
			/* init = inval, rmval */
int  oflim [2];		/* left,right margins for odd  footer */
			/* init = inval, rmval */
int  stopx;		/* flag for pausing between pages */
int  frstpg;		/* first page to begin printing with */
int  lastpg;		/* last page to be printed */
int  print;		/* print page flag */
int  offset;		/* offset whole page by this much */
			/* init = 0 */


/* output lines and related info -- from common block cout */

int  outp;	/* last char position in outbuf; init = 0 */
int  outw;	/* width of text in outbuf; init = 0 */
int  outwds;	/* number of words in outbuf; init = 0 */
char outbuf [MAXOUT];	/* lines to be filled collect here */
			/* word in outbuf; init=0 */


/* number registers -- from common block cnr */

int  nr [26];	/* number registers a..z */


/* list of input files -- from common block cfiles */

int  infile [SYS_STD + MAXOFILES]; /* file descriptor stack */
int  level;		/* current file is infile[level] */


/* pushed-back input characters -- from common block cdefio */

int  bp;		/* next available character; init = 0 */
char pbbuf [PBSIZE];	/* pushed-back characters */


/* symbol table for macros -- from common block cmac */

char *	mactbl;		/* symbol table containing macros */
 infile [SYS_STD + MAXOFILES]; /* file descriptor stack */
int  level;		/* current file is infile[level] */


/* pushed-back input characters -- from common block cdefio */

int  bp;		/* next available character; init = 0 */
char pbbuf [PBSIZE];	/