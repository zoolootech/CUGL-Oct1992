/*	CTOA2.C: CRL-to-CSM file postprocessor, part 2.	*/

#include <bdscio.h>
#include <dio.h>
#include <cmdutil.h>
#include "ctoatbls.h"
#include "ctoa.h"

/*	Acquire the C.CCC symbol table, and put any non-library-defined
	symbols on the .CSM file.	*/

initccc () {
	struct ccc_entry * ccc;

	ccc_table = ccctabl ();

	for (ccc=ccc_table; ccc -> ccc_addr; ++ccc) {
		if (ccc -> ccc_flags & CCC_NMAC) {
			printf ("%s:\tequ\t0%04xH\t\t; C.CCC entry point\n",
				ccc->ccc_name, ccc->ccc_addr);
			}
		}
	printf ("\n");
	}

/*	Find a symbol in the C.CCC symbol table with the requisite attributes*/

struct ccc_entry * scanccc (addr, flags)
    unsigned addr;		/* Address to look for */
    char flags;			/* Flags required */
    {
	struct ccc_entry * ccc;

	for (ccc = ccc_table; ccc -> ccc_addr; ++ccc)
		if ((ccc -> ccc_flags & flags) == flags
		 && ccc -> ccc_addr == addr) 
			return (ccc);
	return (NULL);
	}

/*	Display an address from the C.CCC symbol table */

putccc (ccc)
    struct ccc_entry * ccc;
    {
	printf ("%s", ccc -> ccc_name);
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

	c = getc (srcfile);	/* Get a character */
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
		c = getc (srcfile);		/* Get next character */
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
		if (c2 == '\\') sputc (getc (srcfile));	/* Do escapes */
		c2 = sputc (getc (srcfile));	/* Next char */
		} while (c2 != c && c2 != CPMEOF && c2 != EOF);
	}

/*	Read a 'C' comment from source file 	*/

scomt () {
	int c;
	if ((c = getc (srcfile)) != '*') {	/* Really a comment? */
		ungetc (c, srcfile);		/* No, put it back */
		return;
		}
	sputc (c);
	do {
		while ((c = sputc (getc (srcfile))) != '*') { /* Find '*' */
			if (c == EOF || c == CPMEOF) return;
			if (c == '/') scomt ();	/* Handle nested comments */
			}
		if ((c = getc (srcfile)) == '*') 	/* Handle ... ***/
			ungetc (c, srcfile);
		else sputc (c);
		} while (c != '/' && c != EOF && c != CPMEOF);
	}

/*	Copy a source character to the .CSM file	*/

int sputc (c)
    int c;
    {
	if (!slopen) {			/* Begin a new comment line */
		printf ("; ");
		slopen = TRUE;
		}
	if (c != EOF && c != CPMEOF)
		printf ("%c", c);	/* Put the character */
	if (c == '!') printf (";");	/* Keep ASM from barfing. */
	else if (c == '\n') slopen = FALSE;	/* Close source line */
	return (c);
	}

/*	Open the .CDB file, and set up .CDB processing.	*/

int initcdb () {
	int cdblen;		/* Length of the file in bytes */
	
	nextvs = nautvs = nparvs = 0;	/* No variables read yet */

	strcpy (cdbfnam, srcfnam);		/* Get CDB name */
	makeext (cdbfnam, "CDB");
	if (copen (cdbfile, cdbfnam) == ERROR) { /* Open CDB file */
		fprintf (STD_ERR,
		  "; Can't open %s: %s.\n; Assume %s compiled without '-k'.\n",
			cdbfnam, errmsg (errno ()), srcfnam);
		havecdb = cdbopen = FALSE;
		return;
		}
	havecdb = cdbopen = TRUE;

	tcseek (cdbfile, 0, CABS, cdbfnam);	/* Read CDB length */
	tcread (cdbfile, &cdblen, 2, cdbfnam);
	ncdbents = cdblen / sizeof cdbentry;
	
	get1cdbe ();			/* Read first CDB entry */
	}

/*	Read the definitions for one function from the .CDB file */

cdbfunct () {
	int level;			/* "Block level" from CDB entry */
	char gotfunct;			/* Flag = TRUE iff we've found start
					   of function's lexical scope on CDB*/

	++fnno;				/* Count functions */
	nparvs = nautvs = 0;		/* No params or autos yet. */
	gotfunct = FALSE;		/* Haven't seen funct. yet. */
	while (cdbopen) {
		if ((level = cdbentry.cdbflag2 & 0x3F) == 0
		 && (cdbentry.cdbflag1 & 0x0F) == 0x01) { /* Funct def? */
			if (gotfunct) break;
			framesize = cdbentry . cdbaddr;
			gotfunct = TRUE;
			}
		else if (level == 0) { /* External? */
			if (gotfunct) break;
			cdbext ();
			}
		else if (level == fnno)			     /* Auto? */
			cdbauto ();
		get1cdbe ();		/* Read next CDB entry */
		}
	}

/*	Process an external variable from .CDB	*/

cdbext () {
	if (!(cdbentry.cdbflag1 & 0x0F)) {	/* Really an external var? */
		strcpy70 (extvtab [nextvs] . vtname, cdbentry . cdbname);
		extvtab [nextvs] . vtaddr = cdbentry . cdbaddr;
		printf ("EXT$%s\tEQU\t%05xh\n", extvtab [nextvs] . vtname,
			extvtab [nextvs] . vtaddr);
		++nextvs;
		}
	}

/*	Process an auto or parameter variable from CDB	*/

cdbauto () {
	if (!(cdbentry . cdbflag1 & 0x0F)) {	/* Really an auto? */
		strcpy70 (autvtab [nautvs] . vtname, cdbentry . cdbname);
		autvtab [nautvs++] . vtaddr = cdbentry . cdbaddr;
		}
	else if (!(cdbentry . cdbflag1 & 0x0B)) { /* How about a parameter? */
		strcpy70 (parvtab [nparvs] . vtname, cdbentry . cdbname);
		parvtab [nparvs++] . vtaddr = cdbentry . cdbaddr;
		}
	}

/*	Read a variable description from the .CDB file 	*/

get1cdbe () {
	if (--ncdbents < 0) {
		cdbopen = FALSE;
		cclose (cdbfile);
		}
	else {
		tcread (cdbfile, cdbentry, sizeof cdbentry, cdbfnam);
		}
	}

/*	Output the stack frame layout of a function	*/

doframe () {
	int i;
	
	for (i=0; i<nautvs; ++i)
		printf ("AUTO$%s\tSET\t%05xh\n", autvtab [i] . vtname,
			autvtab [i] . vtaddr);
	printf ("\nFRAME$SIZE\tSET\t%05xh\n\n", framesize);
	for (i=0; i<nparvs; ++i)
		printf ("PARAM$%s\tSET\t%05xh\n", parvtab [i] .vtname,
			parvtab [i] . vtaddr);
	printf ("\n");
	}

/*	Output a stack pointer increment/decrement 	*/

do_frsiz (adder)
    int adder;
    {
	if (!havecdb || abs (adder) != framesize) {
		printf ("%05xh\t\t; %d", adder, adder);
		return;
		}
	if (adder < 0) printf ("-");
	printf ("FRAME$SIZE");
	}
	
/*	Output an offset into automatic storage		*/

do_autoff (offset)
    unsigned offset;
    {
	if (!havecdb) {
		printf ("%05xh\t\t; %d", offset, offset);
		return;
		}
	if (offset >= framesize+4) {	/* Really a parameter? */
		printf ("FRAME$SIZE+4+PARAM$");
		do_offset (nparvs, parvtab, offset-framesize-4);
		}
	else {                   	/* No, really AUTO */
		printf ("AUTO$");
		do_offset (nautvs, autvtab, offset);
		}
	}

/*	Output an offset into external storage 	*/

do_extoff (offset)
    unsigned offset;
    {
	if (!havecdb) {
		printf ("%05xh\t\t; %d", offset, offset);
		return;
		}
	printf ("EXT$");
	do_offset (nextvs, extvtab, offset);
	}

/*	Output an offset relative to a particular variable table */

do_offset (nvars, vartab, offset)
    int nvars;
    struct vtabent *vartab;
    unsigned offset;
    {
	while (--nvars >= 0) {
		if (vartab [nvars] . vtaddr <= offset) {
			printf ("%s", vartab [nvars] . vtname);
			offset -= vartab [nvars] . vtaddr;
			break;
			}
		}
	if (offset) printf ("+%d", offset);
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

/*	Copy a bit-7-terminated string to a null-terminated one. */

strcpy70 (out, in)
    char * out;			/* Null-terminated string output */
    char * in;			/* Bit-7-terminated string in */
    {
	char * tout;
	tout = out;

	do {
		*tout++ = *in & 0x7f;	/* Move a character */
		} while ((*in++ & 0x80) == 0);	/* Until end mark reached */
	*tout++ = '\0';		/* Supply the end marker on output */
	return (out);		/* Return the output string */
	}

/*	Error-checking equivalents of the "CHARIO" functions	*/

tcopen (buf, name)
    CFILE * buf;		/* File to be opened */
    char * name;		/* Name to open it with */
    {
	int retval;
	if ((retval = copen (buf, name)) == ERROR) {
		fprintf (STD_ERR, "Unable to open %s: %s\n",
						name, errmsg (errno ()));
		quit ();
		}
	return (retval);
	}

tcseek (buf, addr, type, name)
    CFILE * buf;		/* File on which to seek */
    unsigned addr;		/* Address being sought */
    int type;			/* Type of seek (absolute/relative) */
    char * name;		/* Filename for error message */
    {
	int retval;
	if ((retval = cseek (buf, addr, type)) == ERROR) {
		fprintf (STD_ERR, "Seek error on %s: %s\n",
						name, errmsg (errno ()));
		quit ();
		}
	return (retval);
	}

tcread (buf, loc, len, name)
    CFILE * buf;		/* File to read */
    char * loc;			/* Place to put the result */
    unsigned len;		/* Number of bytes to read */
    char * name;		/* Filename for error message */
    {
	int retval;
	if ((retval = cread (buf, loc, len)) == ERROR) {
		fprintf (STD_ERR, "Read error on %s: %s\n",
						name, errmsg (errno ()));
		quit ();
		}
	return (retval);
	}

/*	Stop the program prematurely	*/

quit () {
	dioflush ();
	exit ();
	}

