/*
**	bdsmark.c -- mark library functions that BDS C implements in a
**  nonstandard fashion.
**
**	This program will accept a BDS C program as input and write
**  the program to its output with markers placed around the BDS C
**  standard library functions that are not implemented in a standard
**  manner.  In some cases the differences are trivial.  Other times
**  they are not.  Using this program to mark a file can help in
**  converting a file to portable form for compilation in a system
**  with a standard "standard" library.
**
**	David D. Clark
**	3 February 1987
*/

#include	"stdio.h"		/* Aztec won't take "<...>" notation */
/*#include	"ctype.h"*/		/* delete for 8 bit versions */

#define QC

#ifndef	TRUE
#define TRUE	1
#define	FALSE	0
#endif

#define	TOKLEN		9		/* max length for token to test */
#define	MARKER		"^^"		/* marker in output file */

char	*nonstd[] =			/* nonstandard functions */
	{				/* reason: */
	   "creat",			/* parameters */
	   "exit",			/* doesn't flush files */
	   "fgets",			/* parameters */
	   "fopen",			/* parameters */
	   "getc",			/* text vs binary i/o */
	   "getchar",			/* text vs binary i/o */
	   "putc",			/* text vs binary i/o */
	   "puts",			/* doesn't append '\n' */
	   "read",			/* parameters */
	   "tolower",			/* checks argument range */
	   "toupper",			/* checks argument range */
	   "write"			/* parameters */
	};

char	c;				/* current scanner character */
char	token[TOKLEN];			/* token to be tested */
FILE	*infile, *outfile;		/* input and output files */
FILE	*fopen();


main(argc, argv)
int	argc;
char	*argv[];
{

	if (argc != 3)
	{
		fputs("Usage: bdsmark infile outfile\n", stderr);
		exit(1);
	}
	
	if ((infile = fopen(*++argv, "r")) == NULL)
	{
		fputs("Can't open input file:  ", stderr);
		fputs(*argv, stderr);
		putc('\n', stderr);
		exit(1);
	}

	if ((outfile = fopen(*++argv, "w")) == NULL)
	{
		fputs("Can't open output file:  ", stderr);
		fputs(*argv, stderr);
		putc('\n', stderr);
		exit(1);
	}

	scanner();			/* do the translation */

	fclose(infile);
	fclose(outfile);
	exit(0);
}


/*
**	scanner -- scan the input file and produce the marked
**  output file.  This function looks specifically for comments,
**  string constants, character constants and identifiers.  When
**  a potential comment is detected, control is passed to the
**  function "commenter" to pass the rest of the comment without
**  change.  String and character constants are also passed unchanged.
**  When an identifier is detected, it is copied into the global
**  character array "token", then "tsttoken" is called to compare
**  it to the names of the nonstandard functions.  If a match is
**  found, it is marked in the output file.
*/

scanner()
{
static char *cp;
static int i;

	while ((c = getc(infile)) && !feof(infile) && !ferror(infile))
	{
		if (c == '/')			/* check for a comment */
			commenter();
		else if (c == '\'')		/* handle character constant */
			passuntil('\'');
		else if (c == '"')		/* handle literal string */
			passuntil('"');
		else if (iscsymf(c))		/* look for an identifier */
		{
			i = 0;
			cp = token;
			while (iscsym(c) && (i < (TOKLEN - 1)))
			{
				*cp++ = c;
				i++;
				c = getc(infile);
			}
			*cp = 0;
			if (tsttoken())		/* mark it */
			{
				fputs(MARKER, outfile);
				fputs(token, outfile);
				fputs(MARKER, outfile);
			}
			else
				fputs(token, outfile);
		}
		putc(c, outfile);	/* write last char scanned */
	}
	putc(c, outfile);		/* try to write end-of-file char */
}


/*
**	passuntil -- pass characters from "infile" to "outfile" until
**  the terminator character "term" is detected.  Detect and pass
**  "escaped" character sequences unchanged.  On entry, the scanner
**  character in "c" has not yet been written to the output file.  On
**  return, the scanner character will contain the terminator character
**  ready for output.
*/

passuntil(term)
int term;
{
	putc(c, outfile);
	while ((c = getc(infile)) != term)
	{
		putc(c, outfile);
		if (c == '\\')
		{
			c = getc(infile);
			putc(c, outfile);
		}
	}
}


/*
**	tsttoken -- return TRUE if the token matches one of the
**  names of the nonstandard functions in the variable "nonstd", FALSE
**  otherwise.  Because there are only a few, a simple linear search
**  is done on the array. 
*/

int tsttoken()
{
static char **t;

	for (t = nonstd; **t; t++)
	{
		if (strcmp(*t, token) == 0)
			return (TRUE);
	}
	return (FALSE);
}


/*
**	commenter -- pass comments unchanged.  On entry, the leading
**  '/' has already been detected.  On return, c contains the final '/'
**  of the comment.
**
**	This function expects comments to be in the UNIX style.  It
**  does not handle nested comments correctly.  In any case, the
**  damage caused by nested comments should be minimal.  The worst
**  that should occur is marking of nonstandard function names that
**  appear in the comment.
*/

commenter()
{

	putc(c, outfile);		/* write the '/' char */
	if ((c = getc(infile)) == '*')	/* is next char '*'? */
	{				/* yes, handle rest of comment */
		putc(c, outfile);
		c = getc(infile);
		for (;;)
		{
			putc(c, outfile);
			if (c == '*')	/* '*' of end of comment? */
			{
				if ((c = getc(infile)) == '/')
					return;	/* got it all */
				else
					putc(c, outfile);
			}
			c = getc(infile);
		}
	}
}


#ifndef QC


/*
**	iscsymf -- is c a valid character for the first character in
**  a C symbol?
*/

int iscsymf(c)
int c;
{
	return (isalpha(c) || c == '_');
}
