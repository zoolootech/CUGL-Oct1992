/*	-------------------------------------------------------
	*** Template for Procedure Heading ***

	Name:
	Result:
	Errors:
	Globals:
	Macros:
	Procedures:

	Action:

	------------------------------------------------------- */

/*	: : : : : : : : : : : : : : : : : : : : : : : : : : : :

		Program to count the number of:
		chars, words or lines in a file

	: : : : : : : : : : : : : : : : : : : : : : : : : : : : */


/*	: : : : : : : : : : : : : : : : : : : : : : : : : : : :

		Macros for constant definitions

	: : : : : : : : : : : : : : : : : : : : : : : : : : : : */

#define EOFF -1		/* end of file marker returned by getc() */
#define NOFILE -1	/* no such file indication given by fopen() */

#define YES 1		/* true */
#define NO 0		/* false */

#define EOF 0x1A	/* CP/M's end file char for ascii files */
#define CR 0x0D		/* Carriage return */

/*	-------------------------------------------------------

	Name:		main(argc,argv)
	Result:		---
	Errors:		invocation syntax or no such file
	Globals:	---
	Macros:		NOFILE
	Procedures:	badcmd(),printf(),chrcount(),wrdcount()
			lincount(),exit(),fopen(),tolower()

	Action:		Interpert the command line
			handle invocation errors
			open file for buffered input
			handle no file error
			call the counting routine specified
			print results of counting routine on console

	------------------------------------------------------- */


main(argc,argv)
	int argc;
	char *argv[];
	{
	int fd;
	char inbuf[134];

	if(argc != 3)
	  badcmd();
	else if( (fd = fopen(argv[1],inbuf)) == NOFILE )
	  printf("No such file %s\n",argv[1]);
	else if( tolower( *argv[2] ) == 'c' )
	  printf("There are %u characters in file %s\n",
			chrcount(inbuf),argv[1]);
	else if( tolower( *argv[2] ) == 'w' )
	  printf("There are %u words in file %s\n",wrdcount(inbuf),argv[1]);
	else if( tolower( *argv[2] ) == 'l' )
	  printf("There are %u lines in file %s\n",lincount(inbuf),argv[1]);
	else
	  badcmd();
	exit();
	}

/*	-------------------------------------------------------

	Name:		badcmd()
	Result:		---
	Errors:		---
	Globals:	---
	Macros:		---
	Procedures:	puts()

	Action:		Print the invocation error message
			on the console

	------------------------------------------------------- */


badcmd()
	{
	puts("Correct invocation form is: COUNT <filename> <item>\n");
	puts("Where <item> is C[har] or W[ords] or L[ines]\n");
	return;
	}

/*	: : : : : : : : : : : : : : : : : : : : : : : : : : : :
	function chrcount --count the characters in the specified input file
	: : : : : : : : : : : : : : : : : : : : : : : : : : : : */
/*	-------------------------------------------------------

	Name:		chrcount(file)
	Result:		# of chars in file
	Errors:		---
	Globals:	---
	Macros:		EOFF,EOF
	Procedures:	getc()

	Action:		---

	------------------------------------------------------- */


chrcount(file)
	char file[];	/* the file buffer */
	{
	unsigned cc;	/* character count */
	int c;		/* 1 char buffer */

	cc =0;
	while( (c = getc(file)) != EOFF && c != EOF)
	  cc++;
	return cc;
	}		/* end chrcount */


/*	-------------------------------------------------------

	Name:		wrdcount(file)
	Result:		# of words (strings enclosed in space) in file
	Errors:		---
	Globals:	---
	Macros:		EOFF,EOF,CR,YES,NO
	Procedures:	getc(),isspace()

	Action:		---

	------------------------------------------------------- */

wrdcount(file)
	char file[];	/* the file buffer */
	{
	int inword;	/* switch to tell if the present char is in a word */
	unsigned wc;	/* word count */
	int c;		/* 1 char buffer */

	wc =0;
	inword = NO;
	while( (c = getc(file)) != EOFF && c != EOF)
	  if(isspace(c) || c == CR)
	    inword = NO;
	  else if(inword == NO) {
	    inword = YES;
	    wc++;
	    }
	return wc;
	}		/* end wrdcount */


/*	-------------------------------------------------------

	Name:		lincount(file)
	Result:		# of lines ('\n's) in file
	Errors:		---
	Globals:	---
	Macros:		EOFF,EOF
	Procedures:	getc()

	Action:		---

	------------------------------------------------------- */


lincount(file)
	char file[];	/* the file buffer */
	{
	unsigned lc;	/* line count */
	int c;		/* 1 char buffer */

	lc =0;
	while( (c = getc(file)) != EOFF && c != EOF)
	  if(c == '\n')
	    lc++;
	return lc;
	}		/* end lincount */
