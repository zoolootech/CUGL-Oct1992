/*	EDEF:		Global variable definitions for
			MicroEMACS 3.9

			written by Dave G. Conroy
			modified by Steve Wilhite, George Jones
			greatly modified by Daniel Lawrence
*/

/* some global fuction declarations */

extern char *flook();
extern char *getctext();
extern char *getfname();
extern char *getval();
extern char *gtenv();
extern char *gtfun();
extern char *gtusr();
extern char *itoa();
extern char *ltos();
extern char *malloc();
extern char *mklower();
extern char *mkupper();
extern char *strcat();
extern char *strcpy();
extern char *strncpy();
extern char *bytecopy();
extern char *token();
extern char *transbind();
extern unsigned int getckey();
extern unsigned int stock();
extern char *fixnull();
#if	BCOMPL
extern BUFFER *tenexbuf();
#endif

#ifdef	maindef

/* for MAIN.C */

/* initialized global definitions */

int     fillcol = 72;                   /* Current fill column          */
short   kbdm[NKBDM];			/* Macro                        */
char	*execstr = NULL;		/* pointer to string to execute	*/
char	golabel[NPAT] = "";		/* current line to go to	*/
int	execlevel = 0;			/* execution IF level		*/
int	eolexist = TRUE;		/* does clear to EOL exist	*/
int	revexist = FALSE;		/* does reverse video exist?	*/
int	isvt100 = FALSE;		/* vt100 compatible		*/
int	flickcode = FALSE;		/* do flicker supression?	*/
char	*modename[] = {			/* name of modes		*/
	"WRAP", "CMODE", "SPELL", "EXACT", "VIEW", "OVER",
	"MAGIC", "CRYPT", "ASAVE", "BINARY"};
char	modecode[] = "WCSEVOMYAB";	/* letters to represent modes	*/
int	gmode = 0;			/* global editor mode		*/
int	gfcolor = 7;			/* global forgrnd color (white)	*/
int	gbcolor	= 0;			/* global backgrnd color (black)*/
int	gasave = 256;			/* global ASAVE size		*/
int	gacount = 256;			/* count until next ASAVE	*/
int     sgarbf  = TRUE;                 /* TRUE if screen is garbage	*/
int     mpresf  = FALSE;                /* TRUE if message in last line */
int	clexec	= FALSE;		/* command line execution flag	*/
int	mstore	= FALSE;		/* storing text to macro flag	*/
int	discmd	= TRUE;			/* display command flag		*/
int	disinp	= TRUE;			/* display input characters	*/
BUFFER	*bstore = NULL;			/* buffer to store macro text to*/
int     vtrow   = 0;                    /* Row location of SW cursor */
int     vtcol   = 0;                    /* Column location of SW cursor */
int     ttrow   = HUGE;                 /* Row location of HW cursor */
int     ttcol   = HUGE;                 /* Column location of HW cursor */
int	lbound	= 0;			/* leftmost column of current line
					   being displayed */
int	taboff	= 0;			/* tab offset for display	*/
int	hscroll = MEMMAP;		/* horizontal scrolling flag	*/
int	hjump = 1;			/* horizontal jump size		*/
int	tabsize = 8;			/* tab offset for display	*/
int	stabsize = 0;			/* current soft tab size	*/
int	metac = CTRL | '[';		/* current meta character */
int	ctlxc = CTRL | 'X';		/* current control X prefix char */
int	reptc = CTRL | 'U';		/* current universal repeat char */
int	abortc = CTRL | 'G';		/* current abort command char	*/

int	quotec = 0x11;			/* quote char during mlreply() */
char	*cname[] = {			/* names of colors		*/
	"BLACK", "RED", "GREEN", "YELLOW", "BLUE",
	"MAGENTA", "CYAN", "WHITE"};
KILL *kbufp  = NULL;		/* current kill buffer chunk pointer	*/
KILL *kbufh  = NULL;		/* kill buffer header pointer		*/
int kused = KBLOCK;		/* # of bytes used in kill buffer	*/
WINDOW *swindow = NULL;		/* saved window pointer			*/
int cryptflag = FALSE;		/* currently encrypting?		*/
short	*kbdptr;		/* current position in keyboard buf */
short	*kbdend = &kbdm[0];	/* ptr to end of the keyboard */
int	kbdmode = STOP;		/* current keyboard macro mode	*/
int	kbdrep = 0;		/* number of repetitions	*/
int	restflag = FALSE;	/* restricted use?		*/
int	lastkey = 0;		/* last keystoke		*/
int	seed = 0;		/* random number seed		*/
long	envram = 0l;	/* # of bytes current in use by malloc */
int	macbug = FALSE;		/* macro debuging flag		*/
char	errorm[] = "ERROR";	/* error literal		*/
char	truem[] = "TRUE";	/* true literal			*/
char	falsem[] = "FALSE";	/* false litereal		*/
int	cmdstatus = TRUE;	/* last command status		*/
int	firstflag = TRUE;	/* first time			*/
char	palstr[49] = "";	/* palette string		*/
int	saveflag = 0;		/* Flags, saved with the $target var */
char	*fline = NULL;		/* dynamic return line */
int	flen = 0;		/* current length of fline */
int	menuflag = FALSE;	/* menu mode */
LINE	*menuline = NULL;	/* menu	line */
int	ykchar = -1;		/* last deleted character */
int	vt100keys = 0;		/* wait after escapes */

/* uninitialized global definitions */

int     currow;                 /* Cursor row                   */
int     curcol;                 /* Cursor column                */
int     thisflag;               /* Flags, this command          */
int     lastflag;               /* Flags, last command          */
int     curgoal;                /* Goal for C-P, C-N            */
WINDOW  *curwp;                 /* Current window               */
BUFFER  *curbp;                 /* Current buffer               */
WINDOW  *wheadp;                /* Head of list of windows      */
BUFFER  *bheadp;                /* Head of list of buffers      */
BUFFER  *blistp;                /* Buffer for C-X C-B           */
WINDOW	*spltwp;		/* New window created by split	*/

extern BUFFER  *bfind();	/* Lookup a buffer by name      */
extern WINDOW  *wpopup();	/* Pop up window creation       */
extern LINE    *lalloc();	/* Allocate a line              */

char	sres[NBUFN];		/* current screen resolution	*/

char    pat[NPAT];                      /* Search pattern		*/
char	tap[NPAT];			/* Reversed pattern array.	*/
char	rpat[NPAT];			/* replacement pattern		*/

int	(*fkeytab[NFBIND])();	/* fast lookup table		*/
int	(*ffuncc)();		/* FUNC | 'C' hook		*/
KEYTAB	fkeylast;		/* last looked up key		*/

unsigned int	frlen;			/* number of chars in fline */

char	mlline[NSTRING];	/* saved message line 		*/
int	mllen;			/* length of saved message line	*/

int	scrltop, scrlbot;	/* scrolling region */
int	phrow;			/* physical terminal row */

/* The variable matchlen holds the length of the matched
 * string - used by the replace functions.
 * The variable patmatch holds the string that satisfies
 * the search command.
 * The variables matchline and matchoff hold the line and
 * offset position of the start of match.
 */
unsigned int	mtchlen = 0;
unsigned int	mlenold  = 0;
char		*patmatch = NULL;
LINE		*matchline = NULL;
int		matchoff = 0;

#if	MAGIC
/*
 * The variable magical determines if there are actual
 * metacharacters in the string - if not, then we don't
 * have to use the slower MAGIC mode search functions.
 */
short		magical = FALSE;
short		rmagical = FALSE;
MC		mcpat[NPAT];		/* the magic pattern		*/
MC		tapcm[NPAT];		/* the reversed magic pattern	*/
RMC		rmcpat[NPAT];		/* the replacement magic array */

#endif

/* directive name table:
	This holds the names of all the directives....	*/

char *dname[] = {
	"if", "else", "endif",
	"goto", "return", "endm",
	"while", "endwhile", "break",
	"force"
};

#if	DEBUGM
/*	vars needed for macro debugging output	*/
char outline[NSTRING];		/* global string to hold debug line text */
#endif

#else

/* for all the other .C files */

/* initialized global external declarations */

extern  int     fillcol;                /* Fill column                  */
extern  short   kbdm[];                 /* Holds kayboard macro data    */
extern  char    pat[];                  /* Search pattern               */
extern	char	rpat[];			/* Replacement pattern		*/
extern	char	*execstr;		/* pointer to string to execute	*/
extern	char	golabel[];		/* current line to go to	*/
extern	int	execlevel;		/* execution IF level		*/
extern	int	eolexist;		/* does clear to EOL exist?	*/
extern	int	revexist;		/* does reverse video exist?	*/
extern	int	isvt100;		/* vt100 compatible		*/
extern	int	flickcode;		/* do flicker supression?	*/
extern	char *modename[];		/* text names of modes		*/
extern	char	modecode[];		/* letters to represent modes	*/
extern	KEYTAB keytab[];		/* key bind to functions table	*/
extern	NBIND names[];			/* name to function table	*/
extern	int	gmode;			/* global editor mode		*/
extern	int	gfcolor;		/* global forgrnd color (white)	*/
extern	int	gbcolor;		/* global backgrnd color (black)*/
extern	int	gasave;			/* global ASAVE size		*/
extern	int	gacount;		/* count until next ASAVE	*/
extern  int     sgarbf;                 /* State of screen unknown      */
extern  int     mpresf;                 /* Stuff in message line        */
extern	int	clexec;			/* command line execution flag	*/
extern	int	mstore;			/* storing text to macro flag	*/
extern	int	discmd;			/* display command flag		*/
extern	int	disinp;			/* display input characters	*/
extern	BUFFER	*bstore;		/* buffer to store macro text to*/
extern	int     vtrow;                  /* Row location of SW cursor */
extern	int     vtcol;                  /* Column location of SW cursor */
extern	int     ttrow;                  /* Row location of HW cursor */
extern	int     ttcol;                  /* Column location of HW cursor */
extern	int	lbound;			/* leftmost column of current line
					   being displayed */
extern	int	taboff;			/* tab offset for display	*/
extern	int	hscroll;		/* horizontal scrolling flag	*/
extern	int	hjump;			/* horizontal jump size		*/
extern	int	tabsize;		/* tab offset for display	*/
extern	int	stabsize;		/* current soft tab size	*/
extern	int	metac;			/* current meta character */
extern	int	ctlxc;			/* current control X prefix char */
extern	int	reptc;			/* current universal repeat char */
extern	int	abortc;			/* current abort command char	*/

extern	int	quotec;			/* quote char during mlreply() */
extern	char	*cname[];		/* names of colors		*/
extern KILL *kbufp;			/* current kill buffer chunk pointer */
extern KILL *kbufh;			/* kill buffer header pointer	*/
extern int kused;			/* # of bytes used in KB        */
extern WINDOW *swindow;			/* saved window pointer		*/
extern int cryptflag;			/* currently encrypting?	*/
extern	short	*kbdptr;		/* current position in keyboard buf */
extern	short	*kbdend;		/* ptr to end of the keyboard */
extern	int kbdmode;			/* current keyboard macro mode	*/
extern	int kbdrep;			/* number of repetitions	*/
extern	int restflag;			/* restricted use?		*/
extern	int lastkey;			/* last keystoke		*/
extern	int seed;			/* random number seed		*/
extern	long envram;		/* # of bytes current in use by malloc */
extern	int	macbug;			/* macro debuging flag		*/
extern	char	errorm[];		/* error literal		*/
extern	char	truem[];		/* true literal			*/
extern	char	falsem[];		/* false litereal		*/
extern	int	cmdstatus;		/* last command status		*/
extern	char	palstr[];		/* palette string		*/
extern	int	saveflag;		/* Flags, saved with the $target var */
extern	char	*fline;			/* dynamic return line */
extern	int	flen;			/* current length of fline */
extern	int	firstflag;		/* first time			*/
extern	int	menuflag;		/* menu mode */
extern	LINE	*menuline;		/* menu	line */
extern	int	ykchar;			/* last deleted character */
extern	int	vt100keys;		/* wait after escapes */

/* uninitialized global external declarations */

extern  int     currow;                 /* Cursor row                   */
extern  int     curcol;                 /* Cursor column                */
extern  int     thisflag;               /* Flags, this command          */
extern  int     lastflag;               /* Flags, last command          */
extern  int     curgoal;                /* Goal for C-P, C-N            */
extern  WINDOW  *curwp;                 /* Current window               */
extern  BUFFER  *curbp;                 /* Current buffer               */
extern  WINDOW  *wheadp;                /* Head of list of windows      */
extern  BUFFER  *bheadp;                /* Head of list of buffers      */
extern  BUFFER  *blistp;                /* Buffer for C-X C-B           */
extern	WINDOW	*spltwp;		/* New window created by split	*/

extern  BUFFER  *bfind();               /* Lookup a buffer by name      */
extern  WINDOW  *wpopup();              /* Pop up window creation       */
extern  LINE    *lalloc();              /* Allocate a line              */
extern	char	sres[NBUFN];		/* current screen resolution	*/
extern	char    pat[];                  /* Search pattern		*/
extern	char	tap[];			/* Reversed pattern array.	*/
extern	char	rpat[];			/* replacement pattern		*/

extern	unsigned int	mtchlen;	/* length of found string	*/
extern	unsigned int	mlenold;	/* previous length of found str	*/
extern	char	*patmatch;		/* the found string		*/
extern	LINE	*matchline;		/* line pointer to found string	*/
extern	int	matchoff;		/* offset to the found string	*/

extern	int	(*fkeytab[NFBIND])();	/* fast lookup table		*/
extern	int	(*ffuncc)();		/* FUNC | 'C' hook		*/
extern	KEYTAB	fkeylast;		/* last looked up key		*/

extern	unsigned int	frlen;		/* number of chars in fline */

extern	char	mlline[NSTRING];	/* saved message line 		*/
extern	int	mllen;			/* length of saved message line	*/

extern	int	scrltop, scrlbot;	/* scrolling region */
extern	int	phrow;			/* physical terminal row */

#if	MAGIC

extern	short		magical;	/* meta-characters in pattern?	*/
extern	short		rmagical;
extern	MC		mcpat[];	/* the magic pattern		*/
extern	MC		tapcm[];	/* the reversed magic pattern	*/
extern	RMC		rmcpat[];	/* the replacement magic array */

#endif

extern char *dname[];			/* directive name table		*/

#if	DEBUGM
/*	vars needed for macro debugging output	*/
extern char outline[];		/* global string to hold debug line text */
#endif

#endif

/* terminal table defined only in TERM.C */

#ifndef	termdef
extern  TERM    term;                   /* Terminal information.        */
#endif

