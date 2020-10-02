/* Standard header for Software Tools
 * This file includes a version of BDSCIO.h
 * source: tools.h
 * version:  August 23, 1981
 */

/* This file contains: 

	1.  Unchanging constants used by BDS C.
	2.  Unchanging constants used by Software Tools.
	3.  Global variables for the Software tools and
	    constants used to define those variables.

   All software tools should include tools.h and
   should NOT include bdscio.h
*/


/* Part 1 -- Unchanging constants used by BDS C*/

#define BASE 0		/* Base of CP/M system RAM  */
#define NULL 0
#define EOF -1		/* Physical EOF returned by */
			/* low level I/O functions */
#define ERROR -1
#define OK 0
#define CPMEOF 0x1a	/* CP/M End-of-text-file marker */
			/*  (sometimes!)  */
#define SECSIZ 128	/* Sector size for read/write calls */
#define TRUE 1
#define FALSE 0


/* Part 2 -- Unchanging constants used by Software Tools */

#define	BEGINNING_OF_FILE -2	/* seek to start of file */
#define	END_OF_FILE EOF		/* flag to seek for eof */

#define DIGIT	'0'
#define LETTER	'A'

#define	ERR	ERROR
#define	HUGE	30000	/* some arbitrarily large number */
#define	LAMBDA	0	/* end of list marker */
#define	NO	0
#define	YES	1
#define NOERR	OK


/* file types */

/* comment out ----- (file types)
#define	ALPHA	-9
#define	ASCII	12
#define BINARY  60
#define LOCAL	6
----- end comment out */


/* ASCII control character definitions */

#define	NUL	0

#define	BEL	7
#define	BS	8
#define	HT	9
#define	LF	10
#define	VT	11
#define	FF	12
#define	CR	13

#define	CAN	24

#define	ESC	27

#define	DEL	127


/* Synonyms for important non-printing ASCII characters */

#define	BACKSPACE 8
#define	BELL	7
#define	BLANK	32
#define	NEWLINE 10
#define	RUBOUT	127
#define	TAB	9


/* Printable ASCII characters */

#define DQUOTE 34
#define	LBRACE	123
#define	LBRACK	91
#define	LPAREN	40
#define	RBRACE	125
#define	RBRACK	93
#define	RPAREN	41
#define	SQUOTE	39
#define	UNDERLINE 95

/* Ratfor language extensions */

#define FILEDES int
#define POINTER int
#define BOOL int
#define EOS 0

/* comment out -----
define (DS_DECL,integer $1($2);common/cdsmem/$1)
define (IS_DIGIT,(DIG0<=$1&$1<=DIG9))	# valid only for ASCII!
define (IS_LETTER,(IS_UPPER($1)|IS_LOWER($1)))
define (IS_LOWER,(LETA<=$1&$1<=LETZ))
define (IS_UPPER,(BIGA<=$1&$1<=BIGZ))
----- end comment out */


/* Machine-dependent parameters */

/* comment out -----
#define	BITS_PER_CHAR	8
#define	BITS_PER_WORD	16
#define	CHARS_PER_WORD	2
#define	MAX_INTEGER	32767
#define	MIN_INTEGER	-32768
#define	MAX_REAL_EXP	38
#define	MIN_REAL_EXP	-38
#define	REAL_PRECISION	6
----- comment out */


/*
DRIVER and DRETURN have been eliminated.
Instead, each tool should start as follows:

main(arc, argv)
int argc;
char ** argv;
{
	initst(argc, argv);
	tool();
}

*/

/* Definitions for 'spawn' primitive */

/* comment out ----------
#define	WAIT	'w'	wait for subprocess to complete
#define	NOWAIT	'n'	control returns as soon as
			subprocess starts
#define	BACKGR	'b'	spawning a background process
---------- end comment out */


/* Part 3 -- Define global variables */

/* max chars in file name (including EOS)
   for CP/M =
	2 (disk)
	8 (filename)
	1 (dot)
	3 (extension)
	1 (EOS)
*/

#define	SYS_MAXFN 15	/* replaces FILENAMESIZE, MAXNAME */

#define MAXLINE 135	/* used by BOTH BDS and tools */

#define	MAXCARD	80	/* "card" size */
			/* to be replaced */

#define	MAXCHARS 20	/* max nbr of chars when converting */
			/* from integers to characters */
			/* (used by putint, outnum, etc.) */

#define	MAXPAT	128	/* max size of encoded patterns */
			/* (used in string matching) */

#define	NCHARS	33	/* number of special characters */


/* define size of I/O buffers

   The NSECTS symbol controls the size of the file buffers.
   To change the buffer size allocation, follow these steps:

     1) Alter NSECTS to the desired value here in tools.h
     2) Re-compile ALL bds files.

   Make sure you use declare all your I/O buffers with the a
   statement such as:

  		char buf_name[BUFSIZ];
*/

#define NSECTS 8	/* Number of sectors to buffer up in ram */

#define BUFSIZ (NSECTS * SECSIZ + 6 )	/* Don't touch this */

struct _buf {				/* Or this...	    */
	int _fd;
	int _nleft;
	char *_nextp;
	char _buff[NSECTS * SECSIZ];
};

/* seed for random number generators */

int sys_seed;

/* time and date */

char sys_date [MAXLINE];

/* dynamic storage allocation information */

struct _header  {
	struct _header *_ptr;
	unsigned _size;
 };

struct _header _base;
struct _header *_allocp;


 /* command line argument information */

 int sys_narg;		/* number of arguments in list */
			/* init to 0 */
 int sys_args[30];	/* pointers to n'th argument */
			/* BDS C has max of 30 arguments */

/* file information:
   
   Units are either standard units or explicitly opened files.

   Units 0, 1, and 2 are the STDIN, STDOUT, and ERROUT units.
By default, these units are the same as the user's console,
but these units may be redirected to files via the
command line characters >, < and ?.

   Units 3 and 4 are always assigned to the user's console
and line printer respectively.  These units can not be
reassigned.  Reading from unit 4 is not permitted.

   The user need never open units 0 through 4, although it
is permissible to do so.

   Units with numbers greater than 4 are files.  These units
must always be opened by calling open().  Normally, these
units are disk files.  However, these units may assign to the
files TTY or PRINTER.  All I/O assigned to TTY is assigned to
unit 3.  All I/O assigned to PRINTER is assigned to unit 4.

   I/O from standard units may be redirected to any file,
including TTY and PRINTER.

   Sys_bufp [unit] != ERR ==> unit is directed to a disk file.
   Sys_spec [unit] != ERR ==> unit is directed to special file.
   Otherwise, the unit is closed.
*/


/* define access modes */

#define READ 1
#define WRITE 2
#define APPEND 3
#define READWRITE 4

/* define standard unit numbers */

#define SYS_STD 5		/* # of standard units */

#define STDIN 0
#define STDOUT 1
#define ERROUT 2
#define STDERR ERROUT
#define SYS_TERM 3
#define SYS_PRT 4

/* define reserved filenames */

#define TERMINAL "TTY"		/* user's terminal */
#define PRINTER  "PRINTER"	/* user's printer */

#define MAXOFILES 5		/* max # of open files */

int sys_bufp [SYS_STD + MAXOFILES];	/* ERR or ^ buffer */

int sys_bufn [SYS_STD + MAXOFILES];	/* buffer number */

int sys_spec [SYS_STD + MAXOFILES];	/* ERR or std unit */

int sys_acc  [SYS_STD + MAXOFILES];	/* access mode */

char sys_bufs  [MAXOFILES * BUFSIZ];	/* file buffers */


#define TERMINAL "TTY"		/* user's terminal */
#define PRINTER  "PRINTER"	/* user's printer */

#define MAXOFILES 5		/* max # of open files */

int sys_bufp [SYS_STD + MAXOFILES];	/* ERR or ^ buffer */

int sys_bufn [SYS_STD + MAXOFILES];	/* buffer