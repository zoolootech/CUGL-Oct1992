/*	DMERGE:	Dictionary Merge Utility for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved

	Revision History:

*/

#define	maindef	1

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

/*	GLOBALS for DMERGE	*/

char dname[NSTRING];		/* main output dictionary text file */
int upflag;			/* processing upper case flag */

main(argc, argv)

int argc;	/* command line argument count */
char **argv;	/*              argument vector */

{
	WORD *tword;

	/* check to see if they need help.... */
	if (argc < 3) {
		usage();
		exit(EXBADOPT);
	}

	/* announce us */
	printf("DMERGE %s	Dictionary merge Utility\n", VERSION);

	/* grab the output dictionary name */
	--argc;
	argv++;
	strcpy(dname, argv[0]);

	/* and read in the user word lists */
	while (--argc) {
		argv++;		/* skip to next argument */
		uread(argv[0]);	/* and read the dictionary in */
	}

	/* prepare to merge */
	comsort();
	if (numfiltr)
		printf("%u User words loaded\n", numfiltr);
	else {
		printf("%%No user words loaded\n");
		exit(EXBADOPT);
	}

	if (mopen() == FALSE)
		exit(EXMDICT);

	/* open the output dictionary file */
	if ((outfile = fopen(dname, "w")) == NULL) {
		printf("%%Can not open output dictionary file\n");
		exit(EXTEMP);
	}

	/* and do the merge */
	printf("[Merging....]");
	merge();
	printf("\n%u words merged ... new dictionary is %u words long\n",
			numfiltr, totwords);

	mclose();
	fclose(outfile);
	exit(EXGOOD);
}

usage()		/* print the command line usage */

{
	printf("DMERGE:	Dictionary Merge Utility\n");
	printf("	for MicroSPELL %s\n",VERSION);
	puts("\nUsage\n");
	puts("	dmerge <output dictionary> <user file> {<user file>..}\n");
}

int merge()		/* do a merge run against the main dictionary	*/

{
	register char **curword;/* ptr to current word */
	register int cmp;	/* result of comparison */
	char mword[NSTRING];	/* current dictionary word */

	/* make sure the common word list ends with hivalue */
	strcpy(cword[numfiltr], hivalue);
	upflag = TRUE;

printf("Scanning....\n");
do
strcpy(mword, nxtmword());
 while (*mword != '{');
printf("past the junk!!!\n");

	/* start with the first word in each list */
	strcpy(mword, nxtmword());
	curword = &cword[0];

	while (strcmp(*curword, hivalue) < 0 || strcmp(mword, hivalue) < 0) {

		/* compare a dictionary word and a user word */
		cmp = strcmp(*curword, mword);

		/* and write out the appropriate one */
		if (cmp > 0) {
			outword(mword);
			strcpy(mword, nxtmword());
		} else if (cmp < 0) {
			outword(*curword);
			curword++;
		} else {
			outword(*curword);
			strcpy(mword, nxtmword());
			curword++;
		}
	}
}

outword(word)	/* output a word to the new main dictionary text file */

char *word;	/* word to write to dictionary */

{
	/* if this is the seperator character, toggle uppercase flag off */
	if (*word == SEPCHAR)
		upflag = FALSE;

	/* If we switch from upper to lower case with no seperator,
	   insert one */
	if (upflag)
		if (islower(*word)) {
			fprintf(outfile, "%c\n", SEPCHAR);
			upflag = FALSE;
		}

	/* and finally, dump the word out */
	fprintf(outfile, "%s\n", word);
	totwords++;
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

#if	LATTICE & ~CMS
	forkl(getenv("COMSPEC"),"command","-C",cmd,NULL);
	return(wait());
#endif

#if	(AZTEC & ~AMIGA) | CMS
	system(cmd);
	return(TRUE);
#endif

#if	AZTEC & AMIGA
	fexecl(cmd, NULL);
	return(wait());
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

