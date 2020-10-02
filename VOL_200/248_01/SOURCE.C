/*	SOURCE:	Source file processing for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

WORD *getword()		/* get the next word in the input stream */

{
	register char *wp;		/* ptr into word */
	register int len;		/* length of word */

	static WORD rword;		/* word to return */
	static char text[NSTRING];	/* text of word */
	/*    text MUST immediatly follow rword for now */

nextfile:
	/* first check if we need a new file */
	if (srcfile == NULL) {
		/* are we out of input files? */
		if (++sfnum >= numspell)
			return(NULL);

		/* open the next one! */
		srcfile = fopen(splname[sfnum], "r");
		if (srcfile == NULL) {
			printf("%%Can not open input file '%s'\n",
				splname[sfnum]);
			return(NULL);
		}

		/* and make sure we are ready for more... */
		iptr = NULL;
		srcline = -1;
	}

	/* skip off leading non-alphas */
	if (iptr)
		while (*iptr && !isletter(*iptr))
			iptr++;

nextline:
	/* time to read a new line? */
	if (iptr == NULL || *iptr == 0) {
		if (fgets(iline, MAXLINE - 1, srcfile) == NULL) {
			fclose(srcfile);
			srcfile = NULL;
			goto nextfile;
		}
		iptr = iline;
		srcline++;
	}

	/* skip off leading non-alphas */
	while (*iptr && !isletter(*iptr))
		iptr++;


	if (*iptr == 0)
		goto nextline;

	/* and now set the word up */
	wp = &rword.w_text[0];
	rword.w_file = sfnum;
	rword.w_line = srcline;
	rword.w_col = iptr - &iline[0];
	while (*iptr && isletter(*iptr))
		*wp++ = *iptr++;

	/* terminate and return it... */
	*wp = 0;
	if (rword.w_text[1] == 0)
		goto nextfile;

	return(&rword);
}
