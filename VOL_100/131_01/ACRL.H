/*
	************************************************************
			      ACRL Globals
	************************************************************

	W. Lemiszki					  9 Jan 1982

	Filename: acrl.h				 BDS C v1.50
*/


#include <bdscio.h>		/* standard stuff			*/

#define EOS '\0'		/* end of string 			*/
#define byte char		/* pseudonym for clarity		*/
#define addr unsigned		/* ditto				*/

byte verbose;			/* TRUE to list every byte		*/
byte printflg;			/* TRUE to generate listing		*/
int objflg;			/* TRUE to generate object file		*/

byte xrel;			/* current expression reloc attr	*/
byte infunc;			/* TRUE within function			*/
byte endflag;			/* TRUE after 'end' stmt		*/
byte prepost;			/* prelude/normal/postlude switch	*/
#define PRE 1			/* prelude				*/
#define POST 2			/* postlude				*/

int  pass;			/* pass counter				*/
addr lc;			/* location counter (in crl file)	*/
addr floc;			/* function location counter		*/
addr outloc;			/* output file location			*/
int  numfunc;			/* # of functions declared		*/
int  linecnt;			/* line # counter			*/

char *lptr;			/* input line pointer			*/
byte class;			/* token class				*/
byte type;			/* type (within class)			*/
int  value;			/* token value				*/
byte reloc;			/* TRUE if relocatable			*/

char errchar;			/* error character			*/
int  errcnt;			/* total no. of errors			*/
byte specflag;			/* TRUE for special list address field	*/
addr specval;			/* the value to print			*/

int  codelen;			/* length of code buffer		*/
int  rellen;			/* length of relocation list		*/

byte incflag;			/* TRUE when in include file		*/
int  linesav;			/* linecnt save during include		*/



struct entry			/* symbol table entry			*/
	{
	char *nam;
	byte clas;
	byte typ;
	addr val;
	struct entry *next;
	};

struct entry *id;		/* points to most recently found ident	*/
struct entry *labid;		/* label entry pointer			*/
char *lnam;			/* label name pointer			*/
byte ltyp;			/* label type				*/

#define ROOTSIZ 322		/* 256 reserved, 64 functions, +2	*/
struct entry *roots[ROOTSIZ];	/* symbol table lists			*/
#define RESVD	0		/* base of reserved words		*/
#define	GLOBAL	256		/* global symbols			*/
#define FUNCS	257		/* function list			*/
#define LOCAL	258		/* local symbols			*/

int  faddr[64];			/* function address (lc) table		*/
int  flen[64];			/* function length table		*/

char infile[BUFSIZ];		/* input file buffer			*/
char iname[20];			/* input filename			*/

char outfile[BUFSIZ];		/* output file buffer			*/
char oname[20];			/* output filename			*/

char incfile[BUFSIZ];		/* include file buffer			*/

char line[MAXLINE];		/* input line buffer			*/
char tokbuf[MAXLINE];		/* scanner token buffer			*/

char cctab[128];		/* character class table		*/

byte codebuf[200];		/* the code buffer			*/
addr relbuf[500];		/* relocation list buffer		*/
				/* (relbuf must follow codebuf.  During
				   directory output, a 512 byte code
				   buffer is needed.			*/


/* TOKEN CLASSES */

#define	EOL	 0		/* end of line			*/
#define	MACHINE	 1		/* machine instruction		*/
#define	PSEUDO	 2		/* pseudo op			*/
#define	OPERATOR 3		/* expression operator		*/
#define	OPERAND	 4		/* operand (or unary)		*/
#define	IDENT	 5		/* identifier (symbol)		*/
#define STRING	 6		/* quoted string		*/
#define MISC	 7		/* misc tokens			*/


/* PSEUDO-OP Types */

#define	psEND	1
#define	psDB	2
#define	psDW	3
#define	psEQU	4
#define	psSET	5
#define psDS	6
#define psFUNC	7
#define	psFEND	8
#define psINCL	9


/* OPERATOR Types */

#define	opOR	1
#define	opAND	2
#define	opXOR	3
#define	opMOD	4
#define	opSHL	5
#define	opSHR	6


/* OPERAND Types */

#define	opNOT	1
#define	opHIGH	2
#define	opLOW	3
#define	NUMBER	4


/* IDENTIFIER Types */

#define	idLAB	1		/* statement label		*/
#define	idEQU	2		/* equate			*/
#define	idSET	3		/* redefinable symbol		*/
#define	idMUL	4		/* multiply defined symbol	*/
#define	idUND	5		/* undefined symbol 		*/
#define	idFUNC	6		/* function name 		*/

#define RELBIT 0x80		/* If this bit is set (in type byte)	*/
			   	/* the identifier is relocatable	*/
