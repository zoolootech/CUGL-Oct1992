/********************************************************/
/*		 ROFF5, Version 2.00			*/
/*    (c) 1983,4,8,9 by Ernest E. Bergmann		*/
/*	730 Seneca Street, Bethlehem, Pa. 18015		*/
/*							*/
/* Permission is hereby granted for all commercial and	*/
/* non-commercial reproduction and distribution of this	*/
/* material provided this notice is included.		*/
/********************************************************/
#define    VERSION 2
#define subVERSION 00
#include "\turboc\include\stdio.h"
#include "\turboc\include\string.h"
#define NULL 0
#define TRUE 1
#define FALSE 0
#define ERR -1
#define ERROR (0)
#define CPMEOF 26

#define MAXLINE 512

/* send error messages to console while in DIO */
#define STKSIZ		3
#define HUGE		135	/* generally large number */
#define LSZ		255	/* line buffer size*/
#define	COMMAND		'.'	/* all commands starts with this */

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
#define CNTLZ		26	/* ^Z=MS-DOS EOF char */
#define TRANSLATE 2	/* May 23, 1983*/
#define BLACK 1
#define WHITE 0
#define CONTROL -1
#define SENTINEL -2
#define HTAB	-3
#define SOH	-5
#define OTHERS -4
#define XCHAR '-'
#define UCHAR '_'

/* defaults for global parameters */
#define CZSUB_DEF	154	/* ^Z substitute default */
#define FI_DEF		1
#define	LS_DEF		1
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
#define TF_DEF		'~'	/*translation flag default*/
#define CF_DEF		'^'	/*Dec 4*/
#define IC_DEF		'\\'
#define EV_INI		0
#define CW_DEF		12	/* 12/120" */
#define JU_INI		YES	/*right justification*/
#define REGDEF 		1	/*default for register var.*/
				/*when .rg has no numeric arg*/


#define	REVSCROLL	FALSE
#define CANBS		FALSE
#define BELL	'\007'

typedef enum
{IF,		/*"if" conditional */
 IE,		/* start of "if-else" conditional */
 EL,		/* "else" part of conditional */
 LEFT,		/* for ".{" */
 RIGHT,		/* for ".}" */
 UNKNOWN,	/* returned if doesn't recg. command */
 FI,		/* fill lines */
 TI,		/* temporary indent */
 BP,		/* begin page  */
 BR,		/* causes break */
 CE,		/* center line(s) */
 LS,		/* line spacing */
 NF,		/* no fill */
 PL,		/* set page length */
 SP,		/* add blank line(s) */
 ST,		/* stop(pause) at page start?*/
 FO,		/* footer title */
 HE,		/* header title */
 M1,		/* top margin */
 M2,		/* second top margin */
 M3,		/* first bottom margin */
 M4,		/* bottom-most margin       */
 IG,		/* "ignore";comments*/
 DOTDOT,	/* ".." comment */
 NE,		/* "need";Nov 7,82*/
 FF,		/* "formfeed";Nov 10*/
 SC,		/* "space character"*/
 EH,		/* "even headers"*/
 OH,		/* "odd headers"*/
 EF,		/* "even footers"*/
 OF,		/* "odd footers"*/
 AB,		/* "abort"*/
 TF,		/* "translation flag" */
 CF,		/* "control flag char"*/
 IC,		/* insert character */
 OU,		/* output */
 AD,		/* adjust (justify)*/
 NA,		/* do Not Adjust */
 FR,		/* fractional spacing,code*/
 DS,		/*define string*/
 DE,		/*define macro*/
 AM,		/*append macro*/
 NR,		/*Number Register*/
 DI,		/*diversion*/
 DA,		/* " append*/
 SO,		/*"source", include*/
 OT,		/* Output Translation */
 TM,		/*msg to terminal (stderr)*/
 BJ,		/*break with right justification of current line*/
 AF,   		/*"assign format" to register*/
 CZ,		/* ^Z substitute character */
 RL, 		/* Ruler Line tab spec similar to Wordstar */
 EV,		/* environment switching */
 RM,		/* remove a definition */
 RN,		/* rename a definition */
 PM,		/*print list of macros */
 MC,		/*margin character*/
 NM,		/*numbers in margin*/
 NN,		/*suspend .NM*/
 PC,		/*page number character*/
 PO,		/*page offset*/
 LC,		/*Leader expansion Character*/
 TC,		/*Tab expansion Character*/
 WH,		/* WHen page trap */
 CH,		/* Change page trap */
 IT,		/* Input Trap */
 EM,		/* End Macro trap */
 DT,		/* Diversion Trap */
 TL,		/* 3 part TitLe */
 GO		/* (advance printer only) to absolute location */
} CMD;

typedef struct
	{char typ;
	 char fill;
	 int minwidth;
	}
FORMAT;

typedef struct
{void *link;
 char ident[1]; /*indefinitely long, terminated by '\0' */
 /* followed by integer, string, or pointer to structure */
} ENTRY;  /*can be serched by find2()*/
typedef struct
{void *link;
 char ident[2]; /*="\177"*/
 int dl; /*width*/
 int dn; /*# lines*/
 int dt; /*trap postion*/
 char dtname[LSZ];
} DENTRY;
extern
ENTRY	SLINK,	/* string substitution table */ /*insert(), showit()*/
	MLINK, /* macro and diversion substitution table */
		/*minsert(),showm(),macq()*/
		/*SLINK,MLINK table entries are following ident string
			with second string used as replacement*/
	RLINK;	/* register substitution table;
		   ident string followed by int*/ /*showr(),regist()*/

/* typical addition of new entries consists of:
	entry *pe;
	char  *pc;
	char buff[?];
	unsigned siz;
	ENTRY *where;
	pe = buff;
	pe->link = ?LINK.link;
	pc = &pe.ident;
	pc++ = stpcpy(pc, NAME);
        Now pc points to where info should be placed;
	Compute actual siz;
	if (NULL==(where=(ENTRY *)malloc(siz)))
	{fprintf(stderr,"ERROR MESSAGE cannot malloc\n");
	 exit();
	}
	?LINK.link = (ENTRY *)memcpy(where,buff,siz);
*/
extern int
 INPUTlns,	/*count of all lines from input*/
 INTRAP,	/* 0 if no input trap pending; counts down if pos. */
 PAGESTOP,
 JUSTIFY, DOTJ,
 FFEED,
 FIRSTPAGE,LASTPAGE,/*for selectively printing output*/
 SUPPRESS,/*if true,no output is passed by putchar()*/


 FILL,		/* boolean for filled lines or not */
 LSVAL[STKSIZ+1],
 TIVAL,	/* temporary indent -> default  0 */
 INVAL,    /* indent margin */
 RMVAL,    /* right margin */
 CEVAL,	/* set equal to number of lines to be centered	*/
 TFVAL[STKSIZ+1], /* default is 0 */
 SPVAL;	/* blank lines to be spaced down */
extern int
 CURPAG,	/* current output page number; init = 0 */
 NEWPAG;	/* next output page number; init = 1 */
extern FORMAT PAGFMT; /*format for printing page numbers*/
extern int
 VLINENO,	/* virtual (intended) line advances on page,
			see vadv()*/
 FVLINENO,	/* + line fraction */
 PLINENO,	/* printer's actual line advances on page,
			see padv()*/
 FPLINENO,	/* + line fraction */
 PLVAL[STKSIZ+1],
 M1VAL[STKSIZ+1],
 M2VAL[STKSIZ+1],
 M3VAL[STKSIZ+1],
 M4VAL[STKSIZ+1],
 TRAPLOC,	/* location of next page trap in lines */
 FOOTLOC,	/* location of start of page footer */
 SCVAL[STKSIZ+1],
 SENTENCE,	/* Nov 20*/
 CFVAL[STKSIZ+1],
 ICVAL[STKSIZ+1],
 EVVAL[STKSIZ+1];

extern int
 LCVAL,		/* for .lc */
 TCVAL;		/* for .tc */
extern char
 ITstring[LSZ],	/* for input trap */
 EMstring[LSZ], /* for End Macro trap */
 *TRAPNAM,	/* for pagetrapping, used with TRAPLOC */
 LINE[ LSZ ], *LIN ,	/*input line buffer */
 EHEAD[ LSZ ],*EH2,*EH3,	/* even header title */
 OHEAD[ LSZ ],*OH2,*OH3,	/* odd header title */
 EFOOT[ LSZ ],*EF2,*EF3,	/* even footer title */
 OFOOT[ LSZ ],*OF2,*OF3,	/* even footer title */
 RULER[ LSZ ];	/* similar to WordStar, keeps track of tabs */
extern int CZSUB;		/* ^Z substitution character */

extern FILE *ifp, *ofp;
/*following added for buffered and formatted output:  */
extern char OUTBUF2[LSZ];	/*for line to be output in fancy fmt*/
extern int BPOS,CP,PP;		/*buffer,column,printer positions*/
extern char XBUF[LSZ];		/*strikout buffer*/
extern int XF,XCOL;		/* " flag and column */
extern char UBUF[LSZ];		/*underline buffer*/
extern int UF,UCOL;		/* " flag and column */
extern int FIRST;			/*flag for first pass*/
extern char DBUF[LSZ];		/*double strike buffer*/
extern int
 dotO,		/* .o page offset */
 DPOS,
 OCNT,
 MCNT,
 BLKCNT,
 DIR,				/* for "spreading" of lines    */
 OUTWRDS,			/* no. words in OUTBUF, init = 0 */
 OUTW,				/*current display width of OUTBUF*/
 OUTPOS,				/* =strlen(OUTBUF) */
 WTOP,LTOP,OUTTOP,	/*zero|negative;levels of subscripts*/
 WBOT,LBOT,OUTBOT,	/*zero|positive;levels of subscripts*/
 OLDLN,				/*position of previous main line*/
 OLDBOT,				/*OUTBOT for previous line;reset
							each page*/
 FRQ,FRVAL;			/* fractional line?, what fraction size*/
#define BBSIZ0	2000
extern int BBSIZ;
extern char
 OUTBUF[ LSZ ],		/*lines to be filled collected here */
 GPWBUF[ LSZ ],		/*the word buffer used by getwrd->putwrd*/
 *FRSTRING, 		/* ^ to code for fractional spacing*/
 *WHSTRING,			/* ^ to code for whole line spacing*/
 *CPTR[128-' '],	/*pointer table for print control*/
 *TPTR[128-' '],	/*pointer table for translation strings
			  				for char; initialize to null */
 KEYBD,				/*boolean & prompt for keyboard input*/
 KLINE[MAXLINE],	/*keyboard line input buffer*/
 *KPTR,				/*pointer for above*/
 *BACKBUF,			/* used as BACKBUF[] */
 BACKBUF0[BBSIZ0];
extern int BINP;			/*position in above;init to 0*/

#define FMAX 4       /* # of additional files open */
#define FNAMSIZ	60   /* limit of filename length */
typedef struct
	{FILE *IFP;
	 char FNAME[FNAMSIZ];
	 int nlines;
	 int bbsiz;
	 int binp;
	 char *backbuf;
	 }
FILEF;
extern FILEF FSTACK[FMAX];
extern int	FPTR;
extern int
 XF2,UF2,MCNT2;
#define NUMENV 4	/* defines number of possible environments */
#define ARGLIM 10
typedef struct
{ int dollar;
  char *arg[ARGLIM];
  char tokens[LSZ];
} ARGs;

#include "\turboc\roff5.fnc"

/*in preparation for new diversion capabilities: */
extern int DIVERTING;	/* TRUE/FALSE */
extern int DLINES;	/* no of lines in last or current div*/
extern int DWIDTH;	/* max OUTW in last or current diversion*/
extern char DNAME[FNAMSIZ]; /*name of current or last diversion*/
extern char Dbuff[]; /* accumulation buffer for diversion*/
extern unsigned Dsiz;	/* number of bytes in DBUFF */
extern char *Dpc2;	/* char pointer within DBUFF */
extern ENTRY *Dprev, *Dnext, *Dpe;
extern DENTRY *Dstats; /*init by dappend; updated by enddiv() */

/* for .nm, .nn */
extern int
NUMBERING,	/* T/F */
NNVAL,		/* nonumbering counter */
LN,		/* line number for NUMBERING */
LNMOD,		/* NUMBERING modulus */
LNI,		/* LN indent from offset */
LNST;		/* LN space from text */
