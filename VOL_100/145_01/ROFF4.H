#define	MSDOS	1	/* Not BDS-C, that's for sure!	*/
#define	C86	0	/* Computer Innovations compiler	*/
#define	DeSmet	1	/* DeSmet compiler - funnyness with fopen	*/
#define	BDS	0

/********************************************************/
/*							*/
/*		    ROFF4, Version 1.60			*/
/*							*/
/*(C) 1983,4 by Ernest E. Bergmann			*/
/*		Physics, Building #16			*/
/*		Lehigh Univerisity			*/
/*		Bethlehem, Pa. 18015			*/
/*							*/
/* Permission is hereby granted for all commercial and	*/
/* non-commercial reproduction and distribution of this	*/
/* material provided this notice is included.		*/
/*							*/
/********************************************************/
/*FEB 19, 1984*/
/*Jan 15, 1984*/
#if BDS
#include "bdscio.h"
#else
#include "stdio.h"
#include "bdsjunk.h"
#endif

/*June 27, 1983 eliminate directed input*/
#if BDS
extern char _doflag;	/* flag if directed /O being used */
extern char  **_nullpos; /*???*/
extern char _dobuf[BUFSIZ];	/* /O buffer used for direction  */
extern char sav_out_file[20];	/*??? added to work with dioinit
				and dioflush() ???*/

#endif
#if C86
extern FILE	*_dobuf;
#endif
#if DeSmet
extern FILE	_dobuf;
#define	bdos	_os
#endif

#if MSDOS
extern extern char *fgets2();
#endif

extern int 	debug;
extern int PAGESTOP;
extern int JUSTIFY;
extern int FFEED;
extern int FIRSTPAGE,LASTPAGE;/*for selectively printing output*/
extern int SUPPRESS;/*if true,no output is passed by putchar()*/

#if BDS
#define STDERR		4
#endif
#if C86
#define STDERR		stderr
#endif
#if DeSmet
#define STDERR		2
#endif

/* send error messages to console while in DIO */
#define STKSIZ		3
#define DEBUG		( debug != 0 )
#define HUGE		135	/* generally large number */
#define LSZ		255	/* line buffer size*/
#define	COMMAND		'.'	/* all commands starts with this */
#define	CONSOLE		-5	/* one of output options */
#define PRINTER 	-4	/* another */
#if BDS
#define FILE		-3	/* another */
#else
#define FFILE		-3	/* another */
#endif
#define	UNKNOWN		-1	/* returned if doesn't recg. command */
#define	NO_VAL		-32760	/* returned when no argument w/commad */
#define WE_HAVE_A_WORD   1       /* returned by getwrd func. */
#define	NO		0
#define	YES		1
#define UNDERLINE	'\137'
#define	CR		0x0D
#define BACKSPACE	'\b'
#define NUMSIGN		'#'	/* for title strings */
#define NEWLINE		'\n'
#define TAB		'\t'
#define BLANK		' '
#define FORMF		0x0C	/* formfeed for printer */
#define SQUOTE		0x27	/* single quote */
#define DQUOTE		0x22	/* double quote */
#define TRSIZ		2000	/*size of TRTBL*/

#define TRANSLATE 2	/* May 23, 1983*/
#define BLACK 1
#define WHITE 0
#define CONTROL -1
#define SENTINEL -2
#define HTAB	-3
#define OTHERS -4
#define XCHAR '-'
#define UCHAR '_'

#define FI		1	/* fill lines */
#define TI		2	/* temporary indent */
#define BP		3	/* begin page  */
#define BR		4	/* causes break */
#define CE		5	/* center line(s) */
#define IN		7	/* left indent */
#define LS		8	/* line spacing */
#define NF		9	/* no fill */
#define PL		10	/* set page length */
#define RM		11	/* set right margin */
#define SP		12	/* add blank line(s) */
#define ST		13	/* stop(pause) at page start?*/
#define FO		14	/* footer title */
#define HE		15	/* header title */
#define M1		16	/* top margin */
#define M2		17	/* second top margin */
#define M3		18	/* first bottom margin */
#define M4		19	/* bottom-most margin       */
#define IG		20	/* "ignore";comments,Nov 6,82*/
#define NE		21	/* "need";Nov 7,82*/
#define FF		22	/* "formfeed";Nov 10*/
#define SC		23	/* "space character";Nov13*/
#define OW		24	/* "output width";Nov 13*/
#define TS		25	/* "tabsize";Nov 13*/
#define EH		26	/* "even headers";Nov 14*/
#define OH		27	/* "odd headers"*/
#define EF		28	/* "even footers"*/
#define OF		29	/* "odd footers"*/
#define AB		30	/* "abort";Nov 15*/
#define DB		31	/* "debug"*/
#define TC		32	/* "translation flag char"*/
#define TR		33	/* "def translation string"*/
#define CF		34	/* Dec 4:control flag char*/
#define IC		35	/* insert character */
#define OU		36	/* output */
#define JU		37	/* right justify?*/
#define NJ		38	/* don't right justify*/
#define WH		39	/* whole line spacing code*/
#define FR		40	/* fractional spacing,code*/
#define DS		41	/*define string*/
#define DM		42	/*define macro*/
#define EM		43	/*end macro*/
#define RG		44	/*register variable*/
#define DI		45	/*diversion*/
#define ED		46	/*end diversion*/
#define SO		47	/*"source", include*/
#define PC		48	/*printer control definition*/
#define SA		49	/*"say" inline msg to console*/
#define BJ		50	/*break with right justification */
/*				of current line*/

extern int FILL;	/* set to YES or NO */
extern int _FILL[STKSIZ];	/*convert global value to a stack*/
extern int LSVAL;	/* line spacing value -> default will be 1 */
extern int _LSVAL[STKSIZ];
extern int TIVAL;	/* temporary indent -> default  0 */
extern int INVAL;	/* left indent -> default  0 */
extern int _INVAL[STKSIZ];
extern int RMVAL;	/* right margin -> default  PAGEWIDTH */
extern int _RMVAL[STKSIZ];
extern int CEVAL;	/* set equal to number of lines to be centered	*/
extern int TCVAL;	/*translation flag char*/
extern int _TCVAL[STKSIZ];
		/* default is 0                                   */
extern int SPVAL;	/* blank lines to be spaced down */

extern int CURPAG;	/* current output page number; init = 0 */
extern int NEWPAG;	/* next output page number; init = 1 */
extern int VLINENO;	/* virtual (intended) line advances on page,
			see vadv()*/
extern int FVLINENO;	/* + line fraction */
extern int PLINENO;	/* printer's actual line advances on page,
			see padv()*/
extern int FPLINENO;	/* + line fraction */
extern int PLVAL;	/* page length in lines */
extern int _PLVAL[STKSIZ];
extern int M1VAL;	/* margin before& including header in lines*/
extern int _M1VAL[STKSIZ];
extern int M2VAL;	/* margin after header in lines*/
extern int _M2VAL[STKSIZ];
extern int M3VAL;	/* margin after last text line in lines*/
extern int _M3VAL[STKSIZ];
extern int M4VAL;	/* bottom margin, including footer in lines*/
extern int _M4VAL[STKSIZ];
extern int BOTTOM;	/* end of text area in lines;start of M3+M4 */
extern int SCVAL;	/* space character*/
extern int _SCVAL[STKSIZ];
extern int OWVAL;	/* output device width*/
extern int _OWVAL[STKSIZ];
extern int TABSIZ;	/* spacing of tabstops*/
extern int _TABSIZ[STKSIZ];
extern int SENTENCE;	/* Nov 20*/
extern int CFVAL;	/*Dec 4:control flag character value*/
extern int _CFVAL[STKSIZ];
extern int ICVAL;	/*insert character*/
extern int _ICVAL[STKSIZ];

extern char LINE[ LSZ ];	/*input line buffer, Nov 26*/
extern char EHEAD[ LSZ ],*EH2,*EH3;	/* even header title */
extern char OHEAD[ LSZ ],*OH2,*OH3;	/* odd header title */
extern char EFOOT[ LSZ ],*EF2,*EF3;	/* even footer title */
extern char OFOOT[ LSZ ],*OF2,*OF3;	/* even footer title */


/* defaults for global parameters */

#define FI_DEF		1
#define	LS_DEF		1
#define	IN_DEF		0
#define	RM_DEF		60
#define	TI_DEF		0
#define	CE_DEF		1
#define	UL_DEF		-1
#define M1_DEF		2
#define M2_DEF		2
#define M3_DEF		2
#define M4_DEF		2
#define PL_DEF		66
#define FF_DEF		YES	/* .ff defaults to "on" */
#define FF_INI		NO	/* initial setting*/
#define SC_INI		BLANK
#define OW_INI		60	/*initial output width*/
#define TS_DEF		8	/*standard tabsize*/
#define TC_DEF		'~'	/*translation flag default*/
#define CF_DEF		'^'	/*Dec 4*/
#define IC_DEF		'\\'
#define CW_DEF		12	/* 12/120" */
#define JU_INI		YES	/*right justification*/
#define REGDEF 		1	/*default for register var.*/
				/*when .rg has no numeric arg*/

extern int DIR;	/* for "spreading" of lines    */
extern int OUTWRDS;	/* no. words in OUTBUF; init = 0 */
extern char OUTBUF[ LSZ ];	/*lines to be filled collected here */
extern int OUTW;	/*current display width of OUTBUF*/
extern int OUTPOS;	/* =strlen(OUTBUF) */
extern int WTOP,LTOP,OUTTOP;	/*zero|negative;levels of subscripts*/
extern int WBOT,LBOT,OUTBOT;	/*zero|positive;levels of subscripts*/
extern int OLDLN;		/*position of previous main line*/
extern int OLDBOT;		/*OUTBOT for previous line;reset
			each page*/
extern int FRQ,FRVAL;	/* fractional line?, what fraction size*/
extern char *FRSTRING; /* ^ to code for fractional spacing*/
extern char *WHSTRING;	/* ^ to code for whole line spacing*/
extern char *CPTR[128-' '];	/*pointer table for print control*/
extern char *TPTR[128-' '];	/*pointer table for translation strings
			  for char; initialize to null */
extern char TRTBL[TRSIZ];	/*holds translation strings*/
extern char *TREND;		/*^to end of above;init = TRTBL*/
extern int DLINK;		/*^ head of diversion list*/
extern int *RLINK;		/*^ head of register variable list*/
extern int *SLINK,*MLINK;	/*points to head of linked
			string, macro lists*/
#if BDS
extern struct _buf *IOBUF;
#endif
#if	C86
extern FILE	*iobuf;
#endif
#if	DeSmet
extern FILE	iobuf;
#endif
/*following added for buffered and formatted output:  */
extern char OUTBUF2[LSZ];	/*for line to be output in fancy fmt*/
extern int BPOS,CP,PP;		/*buffer,column,printer positions*/
extern char XBUF[LSZ];		/*strikout buffer*/
extern int XF,XCOL;		/* " flag and column */
extern char UBUF[LSZ];		/*underline buffer*/
extern int UF,UCOL;		/* " flag and column */
extern int FIRST;		/*flag for first pass*/
extern char DBUF[LSZ];		/*double strike buffer*/
extern int DPOS;
extern int OCNT;
extern int MCNT;
extern int BLKCNT;

#define	REVSCROLL	FALSE
#define CANBS		FALSE

#define BACKSIZE	LSZ
extern char BACKBUF[BACKSIZE];
extern int BINP;	/*position in above;init to 0*/
#define BELL	'\007'
extern char KEYBD;		/*boolean & prompt for keyboard input*/
extern char KLINE[MAXLINE];	/*keyboard line input buffer*/
extern char *KPTR;		/*pointer for above*/

extern struct divfd
	{char *nm;	/*name*/
	int cs;		/*character count*/
	int ls;		/*line count*/
#if BDS
	struct _buf *bf; /*to iobuf if open, FALSE otherwise*/
#endif
#if C86
	FILE *bf; /*to iobuf if open, FALSE otherwise*/
#endif
#if	DeSmet
	FILE bf; /*to iobuf if open, FALSE otherwise*/
#endif
};

#define FMAX		4	/* # of additional files open*/
extern int	FPTR;
#if BDS
extern struct _buf *FSTACK[FMAX];
#endif
#if C86
extern FILE *FSTACK[FMAX];
#endif
#if DeSmet
extern FILE FSTACK[FMAX];
#endif
extern int	TFLAG;/*added for start(), complete()*/
extern int XF2,UF2,MCNT2;
