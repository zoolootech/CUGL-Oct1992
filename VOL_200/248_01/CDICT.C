/*	CDICT:	Compress Dictionary utility program for
		MicroSPELL 1.01

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dsfx.h"

/* globals */

char mdfile[NSTRING];		/* main dictionary text file name */
char mcfile[NSTRING];		/* compressed dictionary file name */
FILE *mdptr = NULL;		/* main dictionary file pointer */
FILE *mcptr = NULL;		/* compressed dictionary file pointer */
int sflen[NSUFFIX];		/* length of suffixes */

main(argc, argv)

int argc;	/* # of command line arguments */
char **argv;	/* text of command line arguments */

{
	register char *word;		/* current word */
	register int suffix;		/* suffix index */
	char lastword[NSTRING];		/* previous word in dictionary */
	char tempword[NSTRING];		/* temporary word in dictionary */
	char *nxtmword();

	printf("CDICT Dictionary Compression Utility for MicroSPELL v1.00\n");

	if (argc != 3) {
		help();
		exit(EXBADOPT);
	}

	strcpy(mdfile, argv[1]);
	strcpy(mcfile, argv[2]);

	if (mopen() != TRUE) {
		printf("%%Can not open text dictionary file\n");
		exit(EXMDICT);
	}

	/* open the output compressed dictionary file */
	mcptr = fopen(mcfile, "w");
	if (mcptr == NULL) {
		printf("%%Can not open compressed dictionary output file\n");
		exit(EXMDICT);
	}

	/* prepare the suffix length table */
	for (suffix = 0; suffix < NSUFFIX; suffix++)
		sflen[suffix] = strlen(sfx[suffix]);

	printf("[Compressing %s => %s]\n", mdfile, mcfile);
	lastword[0] = 0;	/* null last word */

	/* scan the dictionary, compressing */
	word = nxtmword();
	while (word) {
		strcpy(tempword, word);
		cmpsword(lastword, word);
		strcpy(lastword, tempword);
		word = nxtmword();
	}

	/* close things up */
	mclose();
	fclose(mcptr);
	printf("[Dictionary Compressed]\n");
}

help()		/* tell us about cdict... */

{
	printf("\nUsage:\n\n");
	printf("	CDICT <text dictionary> <compressed dictionary>\n");
}

mopen()		/* open the main dicionary */

{
	/* if it is already open, close it down */
	if (mdptr != NULL)
		fclose(mdptr);

	/* open up the text dictionary... */
	if ((mdptr = fopen(mdfile, "r")) == NULL)
		return(FALSE);

	return(TRUE);
}

mclose()	/* close the dictionary down */

{
	/* if it is already open, close it down */
	if (mdptr != NULL)
		fclose(mdptr);
	mdptr = NULL;
}

char *nxtmword()	/* get the next word from the main dictionary */

{
	static char word[NSTRING];	/* word to return */

	/* is it already closed? */
	if (mdptr == NULL)
		return(NULL);

	/* get the next word */
	if (fgets(word, NSTRING - 1, mdptr) == NULL) {
		/* no more left!!!! close out */
		fclose(mdptr);
		mdptr = NULL;
		return(NULL);
	}

	/* all's well, dump the return, any trailing spaces and
	   return the word */
	do
		word[strlen(word) - 1] = 0;
	while (word[strlen(word) - 1] == ' ');
	return(word);
}

cmpsword(lastword, word)	/* compress the given word */

char *lastword;		/* previous dictionary word */
char *word;		/* current dictionary word */

{
	register int index;	/* index into current word */
	register int same;	/* # of same characters */
	register int suffix;	/* suffix code */
	register int wlen;	/* length of current word */

	/* scan for common suffixes */
	wlen = strlen(word);
	for (suffix = 0; suffix < NSUFFIX; suffix++) {
		if (wlen < sflen[suffix])
			continue;
		if (strcmp(&word[wlen - sflen[suffix]], sfx[suffix]) == 0) {
			word[wlen - sflen[suffix]] = 0;	/* trunc it */
			break;
		}
	}

	/* If there is no suffix...suffix ends up as NSUFFIX */

	/* scan for like beginning characters */
	index = 0;
	while (lastword[index] && lastword[index] == word[index])
		index++;

	same = index;
#if	ASCII
	suffix |= 128;
#endif
/*printf("[%s/%s] %u (%u:%s)\n",lastword, word, same, suffix&127, sfx[suffix&127]);*/
	fprintf(mcptr, "%c%s%c", 'A'+same, &word[index], suffix);
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

