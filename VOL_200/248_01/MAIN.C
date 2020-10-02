/*	MAIN:	Main module for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved

	Revision History:

	21-jul-87
		[Released version 1.0 to the USENET]
*/

#define	maindef	1

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

main(argc, argv)

int argc;	/* command line argument count */
char **argv;	/*              argument vector */

{
	WORD *tword;

	/* check to see if they need help.... */
	if (argc == 1) {
		usage();
		exit(EXBADOPT);
	}

	/* announce us */
	printf("MicroSPELL %s	Interactive Speller and Corrector\n", VERSION);

	/* load the common word list */
	loadcom();

	/* parse the command line */
	while (--argc) {
		argv++;		/* skip to next argument */
		if (argv[0][0] == '-')
			option(&argc, &argv);
		else
			sfile(argv[0]);
	}

	/* prepare to spell */
	comsort();
	if (swdebug && (numfiltr > numcom))
		printf("[%u User words loaded and sorted]\n", numfiltr - numcom);

	if (mopen() == FALSE)
		exit(EXMDICT);
	mclose();
	if ((outfile = fopen("spell.lst", "w")) == NULL) {
		printf("%%Can not open temp file\n");
		exit(EXTEMP);
	}

	/* read the words in */
	tword = getword();
	while (tword) {
		++totwords;
		if (!iscom(tword->w_text)) {
			++prowords;
			insword(tword);
		}
		tword = getword();
	}

	/* if there are words left to spell check... go do it */
	if (numwords > 0)
		check();
	if (swwords)
		fprintf(outfile, "!end\n");
	else
		fprintf(outfile, "-2\n0\n");
	fclose(outfile);

	printf("[%u/%u words processed]\n", prowords, totwords);

	if (swemacs) {
		perform("emacs @scan.cmd");
#if	ATARI & LATTICE
		printf("%%Cannot execute EMACS from here...type\m");
		printf("     emacs @scan.cmd\nto scan document\n");
#endif
	}

	exit(EXGOOD);
}

insword(cword)	/* insert a word in the source word list */

WORD *cword;

{
	register WORD *wp;	/* temporary word pointer */

	if (numwords == MAXWORDS)
		check();

	/* malloc room for the word */
	wp = NULL;
	while (wp == NULL && numwords >= 0) {
		wp = (WORD *)malloc(sizeof(WORD) + strlen(cword->w_text) + 1);
		if (wp == NULL)
			check();
	}

	if (wp == NULL) {
		printf("%%Out of Memory while scanning input file\n");
		exit(EXNORAM);
	}

	/* copy the word in */
	wp->w_file = cword->w_file;
	wp->w_line = cword->w_line;
	wp->w_col = cword->w_col;
	strcpy(wp->w_text, cword->w_text);

	/* and hook it in */
	sword[numwords++] = wp;
}

option(argc, argv)	/* process a command line option */

int *argc;		/* ptr to cmd line argc */
char ***argv;		/* paramters */

{
	register char ochar;	/* option character */

	ochar = (*argv)[0][1];
	switch (ochar) {
		case 'd':	/* debug switched on */
			swdebug = TRUE;
			break;

		case 'e':	/* emacs to be used to correct */
			swemacs = TRUE;
			break;

		case 'u':	/* specify user dictionary */
			if ((*argv)[0][2])
				uread(&(*argv)[0][2]);
			else {
				if (*argc > 0)
					uread((*argv)[1]);
					(*argv)++;
					(*argc)--;
				}
			break;

		case 'w':	/* output a word lsit instead of locations */
			swwords = TRUE;
			break;

		default:	/* bad option */
			printf("%%No such option '%s'\n", &(*argv)[0][1]);
			exit(EXBADOPT);
	}
}

sfile(fname)	/* record and check the file to be spelled */

char *fname;	/* file to spell */

{
	if (swdebug)
		printf("[Queueing '%s' to spell]\n", fname);

	/* check to see if we have too many */
	if (numspell >= MAXSPELL) {
		printf("%%Too many input files... '%s' ignored\n", fname);
		return(FALSE);
	} else {
		strcpy(splname[numspell++], fname);
		return(TRUE);
	}
}

usage()		/* print the command line usage */

{
	printf("MicroSPELL %s	Interactive Speller and Corrector\n", VERSION);
	puts("\nUsage\n");
	puts("	spell {<options>} <file> {<file>.....<file>}\n");
	puts("Options:\n");
	puts("	-d		Debugging mode");
	puts("	-e		Use MicroEMACS to scan errors");
	puts("	-u<fname>	Use user word list <fname>");
	puts("	-w		Output a word list instead of location list");
}

#if	RAMSIZE & LATTICE & MSDOS
/*	These routines will allow me to track memory usage by placing
	a layer on top of the standard system malloc() and free() calls.
	with this code defined, the number of allocated bytes is displayed
	in the upper right corner of the screen
*/

#undef	malloc
#undef	free

char *allocate(nbytes)	/* allocate nbytes and track */

unsigned nbytes;	/* # of bytes to allocate */

{
	char *mp;	/* ptr returned from malloc */
	char *malloc();

	mp = malloc(nbytes);
	if (mp) {
		envram += nbytes;
#if	RAMSHOW
		dspram();
#endif
	}

	return(mp);
}

release(mp)	/* release malloced memory and track */

char *mp;	/* chunk of RAM to release */

{
	unsigned *lp;	/* ptr to the long containing the block size */

	if (mp) {
		lp = ((unsigned *)mp) - 1;

		/* update amount of ram currently malloced */
		envram -= (long)*lp - 2;
		free(mp);
#if	RAMSHOW
		dspram();
#endif
	}
}

#if	RAMSHOW
dspram()	/* display the amount of RAM currently malloced */

{
	char mbuf[20];
	char *sp;

/*	TTmove(term.t_nrow - 1, 70);*/
	sprintf(mbuf, "[%lu]", envram);
	sp = &mbuf[0];
	puts(sp);
}
#endif
#endif

#if	AZTEC & MSDOS
#undef	fgetc
#undef	fgets
/*	a1gets:		Get an ascii string from a file using a1getc	*/

char *a1gets(buffer, length, fp)

char *buffer;	/* buffer to leave string in */
int length;	/* maximum length of string */
FILE *fp;	/* file to get string from */

{
	register int c;		/* current character read */
	register char *bp;	/* pointer into buffer */

	bp = buffer;

	while ((c = a1getc(fp))	!= EOF) {
		*bp++ = (char)c;
		if (c == '\n')
			break;
	}

	*bp = 0;
	if (c == EOF)
		return(NULL);
	else
		return(buffer);
}

/*	a1getc:		Get an ascii char from the file input stream
			but DO NOT strip the high bit
*/

int a1getc(fp)

FILE *fp;

{
	int c;		/* translated character */

	c = getc(fp);	/* get the character */

	/* if its a <LF> char, throw it out  */
	while (c == 10)
		c = getc(fp);

	/* if its a <RETURN> char, change it to a LF */
	if (c == '\r')
		c = '\n';

	/* if its a ^Z, its an EOF */
	if (c == 26)
		c = EOF;

	return(c);
}
#endif

perform(cmd)	/* send out a DOS command and execute it */

char *cmd;	/* command to execute */

{
	if (swdebug)	/* output command string with diagnostics */
		puts(cmd);

#if	LATTICE & ~ATARI & ~CMS
	forkl(getenv("COMSPEC"),"command","-C",cmd,NULL);
	return(wait());
#endif

#if	(AZTEC & ~AMIGA) | CMS
	system(cmd);
	return(TRUE);
#endif

#if	AZTEC & AMIGA
	Execute(cmd, 0L, 0L);
	return(TRUE);
#endif
}

#if	CMS
#undef	fopen
/*	The IBM 30xx likes to tell us when file opens
	fail...it's too chatty....I like to handle these myself	*/

FILE *cmsopen(file, mode)

char *file;	/* name of file to open */
char *mode;	/* mode to open it in */

{
	quiet(1);
	return(fopen(file,mode));
}
#endif
