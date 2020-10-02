/*	USER:	User word list processing for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

uread(fname)		/* open and read in a user dictionary */

char *fname;		/* name of dictionary to open */

{
	register char *sp;	/* temp string pointer */
	register FILE *ufp;	/* user word list file handle */
	char buf[NSTRING];	/* bufer to hold user word */

	/* first, try to open it up..... */
	ufp = fopen(fname, "r");
	if (ufp == NULL) {
		printf("%%Can not find user word list %s\n", fname);
		return(FALSE);
	}

	/* if this is the first one, remember the name for later */
	if (*userlist == 0)
		strcpy(userlist, fname);

	/* and now, dump the words into the common word list */
	while (numfiltr < MAXCOM) {
		if (fgets(buf, NSTRING - 1, ufp) == NULL)
			break;
		buf[strlen(buf)-1] = 0;	/* get rid of the newline */

		/* if it's a blank line, ignore it */
		if (buf[0] == 0)
			continue;

		/* get room for it...*/
		if ((sp = malloc(strlen(buf)+1)) == NULL) {
			fclose(ufp);
			return(FALSE);
		}

		/* and store it */
		strcpy(sp, buf);
		cword[numfiltr++] = sp;
	}

	/* complain if the list is full */
	if (numfiltr == MAXCOM)
		printf("%%User word list space filled\n");
	fclose(ufp);
	return(TRUE);
}
