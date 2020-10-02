/*	MDICT:	Main Dictionary processing for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"
#include	"dsfx.h"

#if	CMPRS
/* compression function globals */
char lastword[NSTRING];		/* last word from dictionary */
#endif

mopen()		/* open the main dicionary */

{
	/* if it is already open, close it down */
	if (mdptr != NULL)
		fclose(mdptr);

	/* open op the main dictionary... */
	if ((mdptr = popen(mdfile)) == NULL) {
		printf("%%Can not find main dictionary\n");
		return(FALSE);
	}

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
#if	CMPRS
	char *gcword();
#endif

	/* is it already closed? */
	if (mdptr == NULL)
		return(hivalue);

	/* get the next word */
#if	CMPRS
	if (gcword(word) == NULL) {
#else
	if (fgets(word, NSTRING - 1, mdptr) == NULL) {
#endif
		/* no more left!!!! close out */
		fclose(mdptr);
		mdptr = NULL;
		return(hivalue);
	}

	/* all's well, dump the return and return the word */
#if	CMPRS == 0
	word[strlen(word) - 1] = 0;
#endif
	return(word);
}

#if	CMPRS
char *gcword(word)	/* get the next compressed word */

char *word;	/* buffer to place word in */

{
	register int c;		/* current character from file */
	register char *wptr;	/* ptr into word to return */
	register char *lptr;	/* pointer int last word */
	int same;		/* # of common characters from last word */
	int suffix;		/* index of common suffix */
	int fgetc();

	/* first grab the #same count */
	same = fgetc(mdptr);
	if (same == EOF)	/* at EOF... return NULL */
		return(NULL);
	same -= 'A';

	/* and copy things across */
	wptr = word;
	lptr = lastword;
	while (same-- > 0)
		*wptr++ = *lptr++;

	c = fgetc(mdptr);
#if     EBCDIC
	/* opposite high bit flags */
        while (c > 128) {
#else
        while (c < 128) {
#endif
		*wptr++ = c;
		c = fgetc(mdptr);
	}

	*wptr = 0;

	/* calculate the suffix to add... */
	suffix = c & 127;
	if (suffix != NSUFFIX)
		strcpy(wptr, sfx[suffix]);

	/* save the current uncompressed word */
	strcpy(lastword, word);

	/* and return the word */
	return(word);
}
#endif
