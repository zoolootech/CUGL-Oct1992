/*	FILE:	File handling for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"
#include	"dpath.h"

/*	popen():	open a file for read, looking down the path */

FILE *popen(file)

char *file;	/* file name to open */

{
	char *flook();

	return(fopen(flook(file), "r"));
}

/*	Look up the existance of a file along the normal or PATH
	environment variable. Look first in the HOME directory if
	asked and possible
*/

char *flook(fname)

char *fname;	/* base file name to search for */

{
	register char *path;	/* environmental PATH variable */
	register char *sp;	/* pointer into path spec */
	register int i;		/* index */
	register FILE *fp;	/* trial fp */
	static char fspec[NSTRING];	/* full path spec to search */
	char *getenv();

#if	((MSDOS) & (LATTICE | AZTEC | MSC)) | V7 | USG | BSD

	/* get the PATH variable */
	path = getenv("PATH");
	if (path != NULL)
		while (*path) {

			/* build next possible file spec */
			sp = fspec;
			while (*path && (*path != PATHCHR))
				*sp++ = *path++;
			*sp++ = '/';
			*sp = 0;
			strcat(fspec, fname);

			/* and try it out */
			fp = fopen(fspec, "r");
			if (fp != NULL) {
				fclose(fp);
				return(fspec);
			}

			if (*path == PATHCHR)
				++path;
		}
#endif

	/* look it up via the old table method */
	for (i=2; i < NPNAMES; i++) {
		strcpy(fspec, pathname[i]);
		strcat(fspec, fname);

		/* and try it out */
		fp = fopen(fspec, "r");
		if (fp != NULL) {
			fclose(fp);
			return(fspec);
		}
	}

	/* return the original...hope it is in the current directory */
	return(fname);
}
