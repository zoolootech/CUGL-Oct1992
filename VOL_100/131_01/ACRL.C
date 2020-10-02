/*
	***********************************************************
	**		    8080/8085 Assembler		 	 **
	**							 **
	**	    .CRL Output format for BDS CLINK		 **
	***********************************************************

	W. Lemiszki					 9 Jan 1982

	Filename: acrl.c				BDS C v1.50
*/


#define VER "2.0"			/* version number		*/
#define DEFEXT ".CSM"			/* default input file type	*/

#include "acrl.h"


/*
 *	M A I N
 *	-------
 */
main(argc,argv)
int argc;
char **argv;
{
	int i;

	init();
	printf("\n\n8080/8085  .CRL Assembler  v%s\n\n", VER);

	if (argc == 1)
		fatal("Usage:  acrl <sourcefile> [-p -o -v]\n");

	strcpy(iname, argv[1]);			/* source name */
	if (index(iname, '.') == NULL)
		strcat(iname, DEFEXT);		/* add extention */

	if (fopen(iname, infile) == ERROR)
		fatal("Can't open %s\n", iname);

	for (i=2; i<argc; i++)			/* process options */
		option(argv[i]);

	if (objflg)
		{
		strcpy(oname, iname);
		strcpy(index(oname, '.'), ".CRL");	/* add output ext */
		if (fcreat(oname, outfile) == ERROR)
			fatal("Can't create %s\n", oname);
		}

	assemble();
}




/*
 *	Process optional arguments
 *	--------------------------
 */
option(arg)
char *arg;
{
	if (strcmp(arg, "-P") == 0)
		printflg = TRUE;
	else if (strcmp(arg, "-O") == 0)
		objflg = FALSE;
	else if (strcmp(arg, "-V") == 0)
		verbose = TRUE;
	else
		printf("Ignoring unknown option: %s\n", arg);
}



/* Fatal error exit */
fatal(format, arg)
char *format, *arg;
{
	printf("\n\7*** Error: ");		/* header */
	printf(format, arg);			/* error msg */
	exit();
}



/* Find a char in a string */
char *index(str, c)
char *str, c;
{
	while (*str)
		{
		if (c == *str)
			return (str);
		++str;
		}
	return NULL;
}




/*
 *	Initialize
 *	----------
 */
init()
{
	initctab();				/* initialize the tables */
	initsymb();
	init8080();

	objflg = TRUE;				/* set the default options */
	printflg = FALSE;
	verbose = FALSE;
}



/*
 *	Assemble the Source File
 *	------------------------
 */
assemble()
{
	asm(0);					/* do pass 1 */
	putchar('\n');
	fclose(infile);				/* rewind source file */
	fopen(iname, infile);
	asm(1);					/* do pass 2 */

	fclose(infile);
	if (objflg)
		{
		fflush(outfile);
		fclose(outfile);
		gendir();			/* output directory */
		}

	if (errcnt)
		printf("\n%4d", errcnt);
	    else
		puts("\n  NO");
	puts(" ASSEMBLY ERROR(S)\n");
}




/*
 *	Perform a pass over the source
 *	------------------------------
 */
asm(p)
int p;
{
	pass = p;
	infunc = endflag = FALSE;		/* init for pass */
	linecnt = errcnt = 0;
	numfunc = 0;
	outloc = 0;
	lc = 0x205;

	while (!endflag)
		{
		if (kbhit())
			getchar();		/* check for console abort */

		readline();			/* get a line */
		parse();			/* parse it */
		if (pass)			/* if pass 2 ... */
		    {
		    if (objflg)
			gen();			/* code buffer out */
		    if (printflg)
			list();			/* print listing */
		      else
			ereport();		/* or just report errors */
		    }
		lc += codelen;			/* adjust location cntrs */
		if (prepost != PRE)
			floc += codelen;
		}
}



/* Get the next source line */
readline()
{
	++linecnt;
	if (incflag)				/* include file */
		if ((lptr = fgets(line, incfile)) == 0)
			{
			fclose(incfile);
			linecnt = ++linesav;
			incflag = FALSE;
			}
	if (!incflag)				/* from input file */
		if ((lptr = fgets(line, infile)) == 0)
			fatal("Unexpected EOF\n");
}




/*
 *	Code Buffer to output file
 *	---------------------------
 */
gen()
{
	int i;

	while (outloc < lc)			/* sync up */
		{
		eputc(0);
		outloc++;
		}
	for (i=0; i<codelen; i++)		/* put each byte */
		{
		eputc(codebuf[i]);
		outloc++;
		}
}


/* Put a char with error checking */
eputc(c)
char c;
{
	if (putc(c, outfile) == ERROR)
		fatal("Write error\n");
}




/*
 *	Report lines in error to the console
 *	------------------------------------
 */
ereport()
{
	if (errchar != ' ')
	    {
	    putchar(errchar);			/* error char */
	    if (incflag)
		printf(" %4d : ", linesav);	/* include line # */
	    printf(" %4d\t%s", linecnt, line);	/* line# and line */
	    }
}




/*
 *	Console listing
 *	---------------
 */
list()
{
	int i;

	i = 0;
	putchar(errchar);			/* error char */
	putchar(xrel ? '\'' : ' ');		/* reloc char */

	if (codelen && !prepost)
		{
		printf("%04x ", floc);		/* address */
		i = listcode(i);		/* list up to 4 bytes */
		putchar('\t');
		}
	else if (specflag)			/* list special value */
		printf("%04x\t\t",specval);	/* list specval */
	else
		printf("\t\t");			/* nothing */

	puts(line);				/* list the line */
	if (verbose)
		while (i < codelen)		/* rest of code buffer */
			{
			puts("       ");
			i = listcode(i);
			putchar('\n');
			}
}



/* Print bytes in code field */
listcode(i)
int i;					/* index into codebuf[] */
{
	int j;

	for(j = min(4, codelen - i);  j--;  )
		printf("%02x", codebuf[i++]);
	return i;
}




/*
 *	Generate the directory
 *	-----------------------
 */
gendir()
{
	int i, fd;

	fd = open(oname, 1);			/* reopen crl file */
	codelen = 0;				/* empty the code buffer */
	setmem(codebuf, 512, 0);		/* zero it */
	if (numfunc)
	    funcwalk(roots[FUNCS]);		/* output the functions */
	code(0x80);				/* null byte */
	codew(lc);				/* next free address */
	write(fd, codebuf, 4);
	close(fd);
}


/* Output the function list */
funcwalk(id)
struct entry *id;
{
	if (id->next != NULL)
		funcwalk(id->next);		/* (first is at end of list) */
	nameout(id->nam);			/* name to code buffer */
	codew(faddr[id->val]);			/* address */
}


/* Put function name to code buffer */
nameout(name)
char *name;
{
	int i;

	for (i = strlen(name) - 1;  i--;  )
		code(*name++);
	code(*name + 0x80);			/* set parity bit */
}

/*EOF*/
