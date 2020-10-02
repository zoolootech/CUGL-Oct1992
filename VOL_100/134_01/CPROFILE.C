/*	CPROFILE -- BDS `C' Program Profiler	*/

/*	Copyright (c) 1983 by Kevin B. Kenny
	Released to the BDS `C' Users' Group for non-commercial distribution.

	Kevin Kenny
	729-A E. Cochise Dr.
	Phoenix, Arizona   85020
*/

#define TITLE "CPROFILE version 83-11-14 copyright (c) 1983 by Kevin Kenny\n"

#include "bdscio.h"
#include "dio.h"
#include "cmdutil.h"

/*	Configuration	*/

#define TOP 0x7000		/* End of usable memory.  Must be supplied
				   to CLINK or L2 with the '-t' option. */

/*	Define some general-purpose pointers	*/

union ptr {
	unsigned i;		/* Integer representation */
	unsigned * w;		/* Word pointer */
	char * b;		/* Byte pointer */
	};

/*	Define a long integer for the LONG.C package. 	*/

struct longint {
	char longcont [4];
	};
#define LONG struct longint	/* Poor man's TYPEDEF */

/*	Define the structure of the execution profile table */

struct proftab {
	unsigned pfaddr;	/* PC of the RST 6 operation */
	LONG pftimes;		/* Number of times that RST was hit. */
	} * ptent;

/*	Define the structure of the beginning of the run-time RST 6
	interceptor/supervisor						*/

struct supvsr {
	char supjmp0;		/* A JMP (0xC3) instruction */
	unsigned supbdos;	/* Entry to CP/M BDOS (where the JMP goes) */
	char supname [9];	/* 'cprofile' monogram to detect re-entry */
	char supjmp1;		/* Another JMP (0xC3) instruction */
	unsigned supinit;	/* Initialization entry into supervisor */
	char supjmp2;		/* Another JMP (0xC3) instruction */
	unsigned supboot;	/* Fake warmboot entry into supervisor */
	unsigned suptop;	/* Upper address of the supervisor */
	struct proftab * supent1;	/* Pointer to first entry in profile */
	struct proftab * supnxte;	/* Pointer to next avail entry */
	unsigned supbios;	/* Address to restore to BIOS transfer */
	char supcmd [128];	/* CPROFILE command line saved for reload */
	};
union {
	struct supvsr * sps;	/* Pointer to the supervisor */
	void (* spf) ();
	unsigned spi;		/* Integer value thereof */
	} suprvp;

/*	Define the command line options	*/

struct option optable [5];		/* Place to put the option table */
int tabsize;				/* Size of the profile table needed */
int pagelen;				/* Printer page length */
int nsrcfs;				/* Number of source files supplied */
char * * sfiles;			/* Names of the source files */
char * srcfnptr [2];			/* Default source file pointer */
char srcfnam [20];			/* Default source file */

/*	Define the I/O environment	*/

char * prgnam;				/* Name of the program being analyzed*/
char comfnam [20];			/* Name of the .COM file */
char comfile [134];			/* I/O stream for the .COM file */
char symfnam [20];			/* Name of the .SYM file */
FILE symfile;				/* CHARIO stream for the .SYM file */
FILE srcfile;				/* Input stream for the source file */

/*	Working storage		*/

int hisargc;				/* ARGC of profiled program */
char ** hisargv;			/* ARGV of profiled program */
char pass2;				/* Flag = TRUE iff in analysis pass */
char fnname [9];			/* Name of function being analyzed */
int stmt;				/* Statement number within function */
int line;				/* Line number within function */
char symfnnam [9];			/* Last function read from .SYM */
unsigned symtopad;			/* Upper address of last .SYM funct */
char symend;				/* Flag == TRUE if at end of .SYM */
char sfopen;				/* Flag == TRUE if a source file open*/
char seof;				/* FLAG == TRUE at end of source file*/
int slevel;				/* Depth of {} nesting in source */
char sname [33];			/* Name of a symbol in source */
char infunct;				/* Flag == TRUE if in funct in src */
int sline;				/* Relative line # in source funct. */
char slopen;				/* Flag == TRUE if in source line */
int linage;				/* Number of lines on current page */
int pageno;				/* Current page # */

/*	Main program	*/

main (argc, argv)
    int argc;
    char * * argv;
    {
	union ptr bdosp;

	/* Test if CPROFILE is active.  If it is, this must be the second pass.
	   */

	pass2 = FALSE;
	prgnam = argv [1];	/* Rem,ember the program name */
	bdosp.i = 6;		/* Point to the BDOS entry address in lowcore*/
	suprvp.spi = *bdosp.w;	/* Get the BDOS entry address (top of TPA) */
	if (!strcmp (suprvp.sps -> supname, "cprofile")) pass2 = TRUE;

	/*	Scan the command line, separating CPROFILE's arguments from
		the target program's.	*/

	hisargv = argv = argv + 1; hisargc = 0;	/* His args start at once */
	--argc;	
	while (argc && strcmp (*argv, ":")) {	/* My args start with a ":" */
		++argv; --argc; ++hisargc;
		}
	if (pass2) dioinit (&argc, argv);

	/* Process command line options	*/

	getopts (argc, argv);

	/* Call appropriate analysis routines */

	if (pass2) {
		/* Second pass -- call the analyzer */

		analyze (argc, argv);
		}

	else {
		/* First pass -- install supervisor and call in program to
			be profiled */

		if (!hisargc) {
			fprintf (STD_ERR, "Command must be supplied.\n");
			goto synerr;
			}
		fprintf (STD_ERR, TITLE);
		install ();
		fprintf (STD_ERR, "<<< Loading target program. >>>\n");
		chain (hisargc, hisargv, &(suprvp.sps -> supjmp1));
		}
synerr:
	dioflush ();
	}

/*	Process command line options	*/

getopts (argc, argv)
    int argc;
    char ** argv;
    {
	char error;			/* Flag = TRUE iff command erroneous */
	union {
		int ii;
		char * si;
		char * * msi;
		} info;
	
	/* Define the available options */

	initv (optable, EOF,
#define STRG_OPT -1
		"Linecount", NVAL_KWD,
#define LCNT_OPT 0
		"PageLength", NVAL_KWD,
#define PLEN_OPT 1
		"SourceFiles", MSVL_KWD,
#define SFIL_OPT 2
		EOF);

	++argv; --argc;			/* Discard colon separator */

	error = FALSE;
	tabsize = 512 * sizeof ptent;
	pagelen = 58;
	srcfnptr [1] = & srcfnam;
	strcpy (srcfnam, hisargv [0]);
	makeext (srcfnam, "C");
	sfiles = & srcfnptr;
	nsrcfs = 1;

	while (argc > 0) {
		switch (procarg (&argc, &argv, optable, &info)) {
			case STRG_OPT:
				break;		/* Discard redirection */
			case LCNT_OPT:
				tabsize = info.ii * sizeof (* ptent);
				break;
			case PLEN_OPT:
				pagelen = info.ii;
				break;
			case SFIL_OPT:
				nsrcfs = * (sfiles = info.msi);
				break;
			default:
				error=TRUE;
			}
		}
	
	if (error || hisargc < 1) {
		showsyntax ("cprofile <command and options> : [>filename]",
				optable);
		quit ();
		}
	}


/*	Install CPROFILE in the top of the TPA	*/

install () {
	struct ptr p, q;		/* Working pointer */	
	void superv ();			/* The supervisor's code */
	unsigned * supervrl ();		/* The supervisor's relocation table */
	unsigned suprvs;		/* Size of the supervisor */
	unsigned offset;		/* Relocation factor for supervisor */
	unsigned nrelocs;		/* Number of relocated words in sup */

	/* Find the top of the TPA.  Using this, the size of the supervisor,
	   and the size of the profile table, find where the supervisor origin
	   will be.  */

	p.i = 6;			/* Get address of BDOS jump + 1 */
	p.i = *p.w;			/* Get address of TPA top. */
	suprvp.spf = &superv;		/* Point to current supervisor origin*/
	suprvs = suprvp.sps -> suptop - suprvp.spi;	
					/* Get its length */
	p.i = (p.i - suprvs - tabsize) & 0xFF00;
					/* Find new supervisor origin */
	if (p.i < TOP) {
		fprintf (STD_ERR, 
			"Profile table overlays CPROFILE; use smaller -L\n");
		quit ();
		}

	/* Move the supervisor into place */

	offset = p.i - suprvp.spi;
	movmem (suprvp.spi, p.i, suprvs);
	suprvp.spi = p.i;
	
	/* Relocate the supervisor */

	p.w = supervrl ();		/* Get relocation table */

	nrelocs = *p.w++;
	while (nrelocs--) {
		q.i = suprvp.spi + *p.w++;	/* Get a relocation address */
		*q.w += offset;			/* Relocate it */
		}
	
	/*	Link the BDOS transfer through the supervisor */

	p.i = 6;
	suprvp.sps -> supbdos = *p.w;
	*p.w = suprvp.spi;

	/*	Link the BIOS warmboot transfer through the supervisor */

	p.i = 1;
	p.i = *p.w + 1;
	suprvp.sps -> supbios = *p.w;
	*p.w = &(suprvp.sps -> supjmp2);

	/*	Record the CPROFILE command line in the supervisor for reload*/

	movmem (0x0080, &(suprvp.sps -> supcmd), 128);
	}

/*	Analyze the results of a profiled run	*/

analyze (argc, argv)
    int argc;
    char ** argv;
    {
	int pfcomp ();
	struct proftab * tent;

	/* Restore linkage to real BDOS */

	deinstal ();
	fprintf (STD_ERR, "<<< Starting analysis phase. >>>\n");
	
	/* Open needed files for analysis */

	openfiles ();

	/* Initialize source file reader */

	seof = infunct = sfopen = FALSE;
	slevel = sline = 0;

	/* Sort the profile data by address */

	qsort (suprvp.sps -> supent1, 
	       suprvp.sps -> supnxte - suprvp.sps -> supent1,
	       sizeof * tent,
               pfcomp);

	/* Display the profile */

	sopen ();			/* Open up the first source file */
	linage = 0; pageno = 0; begline (); slopen = FALSE;
	for (tent = suprvp.sps -> supent1;
 	     tent < suprvp.sps -> supnxte;
	     ++tent) {
		doprent (tent);
		}
	}

/*	De-install the run-time supervisor	*/

deinstal () {
	union ptr p;			/* Working pointer */

	p.i = 6;			/* Set pointer to BDOS entry point */

	suprvp.spi = *p.w;		/* Find supervisor entry */

	*p.w = suprvp.sps -> supbdos;	/* Relink the BDOS transfer correctly*/

	}

/*	Open .COM and .SYM files in analysis pass	*/

openfiles () {
	
	/* Open the .SYM file */

	strcpy (symfnam, prgnam);
	makeext (symfnam, "SYM");
	if (fopen (symfnam, symfile) == ERROR) {
		fprintf (STD_ERR, "Can't open %s: %s\n", 
					symfnam, errmsg (errno ()));
		quit ();
		}
	strcpy (symfnnam, "C.CCC");
	if (frdstr (symfile, symfnnam) == ERROR) {
		fprintf (STD_ERR, "Enpty .SYM file %s.\n", symfnam);
		symend=TRUE;
		symtopad = 0xFFFF;
		}

	/* Open the .COM file */

	strcpy (comfnam, prgnam);
	makeext (comfnam, "COM");
	if (copen (comfile, comfnam) == ERROR) {
		fprintf (STD_ERR, "Can't open %s: %s\n", 
					comfnam, errmsg (errno ()));
		quit ();
		}
	}

/*	Display one profile entry	*/

doprent (tent)
    struct proftab * tent;
    {
	char tcount [12];		/* Text for count of times executed */

	tcseek (comfile, tent -> pfaddr - 0x0100, 0, comfnam);
					/* Read the RST 6 parameter */
	tcread (comfile, &line, 2, comfnam);

	stmt = line >> 12; line &= 0x0FFF;	/* Get line number */

	if (line > 1) slineno (line - 1);

	if (line == 0) {
		begfun (tent -> pfaddr);	/* Begin a new function */
		begline ();
		printf ("%-8s", fnname);
		}
	else {
		begline ();
		printf ("%4d", line);
		if (stmt == 0)
			printf ("    "); /* Print line # . stmt  count */
		else
			printf (".%-2d\t", stmt);
		}
	
	ltoa (tcount, tent -> pftimes);
	printf ("%7s\t", tcount);

	slineno (line);			/* Print source line */
	}

/*	Begin a new function in profile	*/

begfun (addr) 
    unsigned addr;			/* Starting address of function + 1 */
    {
	while (addr >= symtopad && !symend) {
		nxfsym ();
		}
	strcpy (fnname, symfnnam);
	sfunct (fnname);
	}

/*	Compare addresses of two entries in the profile table	*/

int pfcomp (t1, t2)
    struct proftab *t1, *t2;
    {
	if (t1 -> pfaddr > t2 -> pfaddr) return (1);
	else if (t1 -> pfaddr == t2 -> pfaddr) return (0);
	else return (-1);
	}

/*	Read the next function from the .SYM file	*/

nxfsym () {
	if (symend) return;
	if (frdstr (symfile, symfnnam) == ERROR
	 || frdhex (symfile, &symtopad) == ERROR) {
		symend = TRUE;
		symtopad = 0xFFFF;
		fclose (symfile);
		}
	}

/*	Locate a function on the source file.  Function is presumed to
	start when its name appears outside curly-braces. */

sfunct (s)
    char * s;				/* Function name */
    {
	do {
		if (seof) return;	/* End of file; no hope. */
		sscan();		/* Look for the name. */
		} while (slevel != 0 || strcmp (sname, s));
	infunct = TRUE;
	sline = 1;			/* This is first line of function. */
	while (slevel == 0 && !seof) sscan ();	/* Find the open brace */
	seol ();			/* Advance to a line break. */
	}

/*	Advance to the end of a function by balancing curly-braces. */

sendfn () {
	infunct = FALSE;
	while (slevel > 0 && !seof) sscan ();	/* Eat tokens 'til balanced. */
	seol ();			/* Advance to a line break. */
	}

/*	Locate a line within the current function. */

slineno (l)
    int l;				/* Line number to find. */
    {
	while (sline != 0 && sline <= l) {	/* Advance */	
		sscan ();
		seol ();
		}
	}

/*	Advance source file to a line break	*/

seol () {
	while (slopen) sscan ();
	}

/*	Scan a token out of the source file	*/

sscan () {
	int c;			/* Current character from file */

	c = sgetc ();	/* Get a character */
	if (isalpha (c) || c == '_') 
		ssymb (c);	/* Scan an identifier if that's what we have */
	else {
		sputc (c);	/* Flush the current character to output */
		switch (c) {
case '/':		scomt ();		/* Process comments */
			break;
case '{':		++slevel;		/* Balance curly braces */
			break;
case '}':		--slevel; /* NO BREAK HERE */
case ';':		if (!infunct) sline = 0;	/* Reset relative line
							when leaving function*/
			break;
case '\'':
case '"':		squote (c);		/* Process quoted strings */
			break;
case '\n':		if (sline) ++sline;	/* Maintain relative line # */
			break;
case CPMEOF:
case EOF:		seof = TRUE;		/* Handle EOF */
			slopen = FALSE;
			sline = 0;
			break;
			}
		}
	}

/*	Read an identifier from source file 	*/

ssymb (c)
    int c;
    {
	char * symp;			/* Pointer to symbol being scanned */
	
	symp = & sname;			/* Initialize scan pointer */

	do {
		if (slevel == 0) *symp++ = toupper (c);	
						/* Accumulate character */
		sputc (c);
		c = sgetc ();		/* Get next character */
		} while (isalpha(c) || isdigit (c) || c == '_');
	ungetc (c, srcfile);		/* Push back char that stopped scan */
	if (slevel == 0) *symp++ = '\0';	/* End the symbol */
	sname [8] = '\0';		/* Truncate it to 8 bytes */
	}

/*	Read a quoted string from source file 	*/

squote (c)
    int c;
    {
	int c2;
	c2 = c;
	do {
		if (c2 == '\\') sputc (sgetc ());	/* Do escapes */
		c2 = sputc (sgetc ());	/* Next char */
		} while (c2 != c && c2 != CPMEOF && c2 != EOF);
	}

/*	Read a 'C' comment from source file 	*/

scomt () {
	int c;
	if ((c = sgetc ()) != '*') {	/* Really a comment? */
		ungetc (c, srcfile);		/* No, put it back */
		return;
		}
	sputc (c);
	do {
		while ((c = sputc (sgetc ())) != '*') { /* Find '*' */
			if (c == EOF || c == CPMEOF) return;
			if (c == '/') scomt ();	/* Handle nested comments */
			}
		if ((c = sgetc ()) == '*') 	/* Handle ... ***/
			ungetc (c, srcfile);
		else sputc (c);
		} while (c != '/' && c != EOF && c != CPMEOF);
	}

/*	Read a character from the stack of source files	*/

int sgetc () {
	int c;

	/* Open a file if necessary */

	if (!sfopen) {
		if (sopen () == EOF) return (EOF);
		return (sgetc ());
		}
	c = getc (srcfile);
	if (c == EOF || c == CPMEOF) {
		fclose (srcfile);
		sfopen = FALSE;
		linage = 0;
		return (sgetc ());
		}
	else return (c);
	}

/*	Open a source file	*/

int sopen () {
	seof = TRUE;
	if (nsrcfs-- <= 0) return (EOF);
	if (fopen (*++sfiles, srcfile) == ERROR) {
		fprintf (STD_ERR, "Can't open source file %s:%s\n",
				*sfiles, errmsg (errno ()));
		return (sopen ());
		}
	seof = FALSE;
	sfopen = TRUE;
	return (OK);
	}

/*	Copy a source character to the output file	*/

int sputc (c)
    int c;
    {
	if (!slopen) {			/* Begin a new comment line */
		begline ();
		printf ("\t\t");
		}
	if (c != EOF && c != CPMEOF && c != '\n' && c != '\r')
		printf ("%c", c);	/* Put the character */
	if (c == '\n') slopen = FALSE;	/* Close source line */
	return (c);
	}
	
/*	Read a whitespace-terminated string from a file	*/

int frdstr (file, string)
    char * file;
    char * string;
    {
	int c;
	if (skipwh (file) == ERROR) return (ERROR);
	while ((c = kgetc (file)) != EOF && c != CPMEOF && !isspace (c))
		*string++ = c;
	*string++ = '\0';
	return (OK);
	}

/*	Read a hex number from a file	*/

int frdhex (file, number)
    char * file;
    int * number;
    {
	int c;
	int d;
	if (skipwh (file) == ERROR) return (ERROR);
	*number = 0;
	while ((d = any ((c = toupper (kgetc (file))), "0123456789ABCDEF"))
								!= -1) {
		*number = (*number << 4) + d;
		}
	return (OK);
	}

/*	Skip whitespace on a file	*/

skipwh (file)
    char * file;
    {
	int c;
	while (isspace (c = kgetc (file)));
	if (c == EOF || c == CPMEOF) return (ERROR);
	ungetc (c, file);
	return (OK);
	}

/*	Kludged 'getc' to make CR-LF pairs into '\n's	*/

int kgetc (file)
    char * file;
    {
	int c;
	if ((c = getc (file)) != '\r') return (c);
	if ((c = getc (file)) == '\n') return (c);
	ungetc (c, file);
	return ('\r');
	}

/*	Begin a line on the listing	*/

begline () {
	int ac;
	char ** av;

	printf ("\n");

	/*	Break page if necessary		*/

	if (linage == 0 || linage >= pagelen) {
		printf ("\fLine\t   Times\tBDS `C' Execution Profile\t");
		if (sfopen) printf ("%s", *sfiles);
		printf ("\tPage %d\n", ++pageno);
		printf ("----\t   -----  < ");
		for (ac=hisargc, av=hisargv;  ac;  --ac, ++av) {
			printf ("%s ", *av);
			}
		printf (">\n\n");
		linage = 3;
		}
	slopen = TRUE;
	++linage;
	}

/*	Locate a character in a string	*/

int any (c, string)
    char c;
    char * string;
    {
	int i;
	i = 0;
	while (*string) if (c == *string++) return (i); else ++i;
	return (-1);
	}

/*	Put an extension on a file name	*/

makeext (name, ext)
    char * name;		/* Name of the file */
    char * ext;			/* Desired extension */
    {
	do {			/* Find the separatrix */
		if (!*name) {	/* No extension originally */
			*name++ = '.';
			break;
			}
		} while (*name++ != '.');

	strcpy (name, ext);	/* Add the extension */
	return (name);
	}

/*	Seek on a CHARIO file with error checking	*/

tcseek (file, addr, flag, name)
    char * file;
    unsigned addr;
    int flag;
    char * name;
    {
	if (cseek (file, addr, flag) == ERROR) {
		fprintf (STD_ERR, "Seek error on %s: %s.\n", 
						name, errmsg (errno ()));
		quit ();
		}
	}

/*	Read a CHARIO file with error checking	*/

tcread (file, buf, size, name)
    char * file;
    char * buf;
    unsigned size;
    char * name;
    {
	if (cread (file, buf, size) < size) {
		fprintf (STD_ERR, "Read error on %s: %s.\n", 
						name, errmsg (errno ()));
		quit ();
		}
	}

/*	Abort the program	*/

quit () {
	if (pass2) dioflush ();
	exit ();
	}
keext (name, ext)
    char * name;		/* Name of the file */
    char * 