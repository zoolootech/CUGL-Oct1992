/*	COMMON:	Most Common word processing for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

loadcom()	/* load the most used words in to the common list */

{
	register char *sp;	/* temp string pointer */
	register int i;		/* index */
	FILE *cf;		/* ptr to common word list file */
	char buf[NSTRING+1];	/* buffer to hold current word */

	/* if we are already loaded....don't bother again */
	if (commonf == TRUE)
		return(TRUE);

	/* try to open it.... */
	if ((cf = popen(comlist)) == NULL)
		return(FALSE);

	/* and load them up */
	commonf = TRUE;
	while (numcom < MAXCOM) {
		/* get the next word */
		if (fgets(buf, NSTRING, cf) == NULL)
			break;
		buf[strlen(buf)-1] = 0;	/* get rid of the newline */

		/* get room for it...*/
		if ((sp = malloc(strlen(buf)+1)) == NULL) {
			fclose(cf);
			return(FALSE);
		}

		/* and store it */
		strcpy(sp, buf);
		cword[numcom++] = sp;
	}

	fclose(cf);
	if (swdebug)
		printf("[%u common words loaded]\n", numcom);
	numfiltr = numcom;
	return(TRUE);
}

dumpcom()	/* unload the common word list */

{
	/* only bother if we are really here */
	if (commonf == FALSE)
		return(TRUE);

	/* dump them */
	while (numfiltr--)
		free(cword[numcom]);
	if (swdebug)
		printf("[common word list dumped]");
	return(TRUE);
}

iscom(word)	/* is a word in the common/user word list? */

char *word;	/* word to check */

{
	register int lower;	/* lower limit for search */
	register int upper;	/* upper limit for search */
	register int mid;	/* current word in search */
	register int cresult;	/* result of current compare */
	register char *tp;	/* temp string pointer */
	static char tword[NSTRING];	/* temp copy of word */

	/* set up limits */
	lower = 0;
	upper = numfiltr - 1;

	/* Binary search the list */
	while (upper >= lower) {

		/* find the current mid item */
		mid = (upper + lower) / 2;

		/* compare it */
		cresult = strcmp(word, cword[mid]);
		if (cresult == 0)	/* success!!!! */
			return(TRUE);

		/* reset the search bounds */
		if (cresult > 0)
			lower = mid + 1;
		else
			upper = mid - 1;
	}

	/* we haven't found it... try lowercasing it */
	if (islower(*word))
		goto fail;
	tp = tword;
	while (*word)
		if (isupper(*word))
			*tp++ = *word++ + DIFCASE;
		else
			*tp++ = *word++;
	*tp = 0;

	/* set up limits */
	lower = 0;
	upper = numfiltr - 1;

	/* Binary search the list */
	while (upper >= lower) {

		/* find the current mid item */
		mid = (upper + lower) / 2;

		/* compare it */
		cresult = strcmp(tword, cword[mid]);
		if (cresult == 0)	/* success!!!! */
			return(TRUE);

		/* reset the search bounds */
		if (cresult > 0)
			lower = mid + 1;
		else
			upper = mid - 1;
	}

fail:	/* it's not here, jim.... */
	return(FALSE);
}
