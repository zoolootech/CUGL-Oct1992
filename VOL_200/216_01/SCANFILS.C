/*	scanfils.c
 *
 *	Read in two directory listings that have been run through the SORT
 *	utility.  Files names that have a changed date/time or that are in the
 *	new directory listing but not the old have a COPY command generated
 *	in an output file.
 *	Note that files that appear in the old listing but not the new must have
 *	been deleted between the 2 listings.  Although an ERASE statement could
 *	be generated, that's kinda dangerous, so an ECHO is generated instead.
 *
 *	Example of usage:
 *		dir | sort >glopold
 *		... other processing ...
 *		dir | sort >glopnew
 *		scanfils glopold glopnew d: >savefils.bat
 *		savefils
 *
 *	Suppose the directory originally contained files A, B, C, D, E, and F.
 *	Other procesing is done.  Now the directory contains files A, C, D, E,
 *	and G.	In addition, file A was updated.
 *	SCANFILS will generate the following statements in its output:
 *		copy A d:
 *		echo B
 *		echo F
 *		copy G d:
 *	(The "d:" cones from the command line, see example invocation above).
 *	Note that the filenames that are input to this utility on the command
 *	line will be ignored insofar as generating any output lines (but only
 *	if the names are simple, ie don't contain drive or directory).	This
 *	means that when used in the example above, no COPY will be generated
 *	for GLOPOLD or GLOPNEW even though they will appear in the directory
 *	listing.
 *	In addition, "X files added, Y files deleted, Z files changed" will
 *	appear on the standard ERROR output as a report.
 *
 */

#include "stdio.h"
#include "process.h"		/* For exit () */
#include "stdlib.h"		/* For perror () */
#include "memory.h"		/* For memcmp, et al */
#include "string.h"		/* For strcmp, et al */

char odir[12], ndir[12];

main (argc, argv, envp)

int argc;
char *argv[];
char *envp[];

{
	void usage ();
	void rl ();
	void gcopy (), gecho ();
	FILE *old, *new;
	int added = 0, changed = 0, deleted = 0;
	char lold[80], lnew[80];
	int cresult;
	char *p;

	if (argc != 4)
		usage ();

	if (! (old = fopen (argv[1], "r"))) {
		perror ("scanfils: Unable to open \"old\" directory file");
		exit (1);
	}

	if (! (new = fopen (argv[2], "r"))) {
		perror ("scanfils: Unable to open \"new\" directory file");
		exit (1);
	}

	/* Generate filenames in "dir" format for both old and new files */
	memset (odir, ' ', 12);
	if (! strpbrk (argv[1], "/\:")) {
		if (p = strchr (argv[1], '.')) {
			memcpy (odir, argv[1], p-argv[1]);
			memcpy (odir+9, p+1, strlen (p+1));
		} else {
			memcpy (odir, argv[1], strlen (argv[1]));
		}
	}
	for (p = odir; p < odir+12; p++)
		*p = toupper (*p);

	memset (ndir, ' ', 12);
	if (! strpbrk (argv[2], "/\:")) {
		if (p = strchr (argv[2], '.')) {
			memcpy (ndir, argv[2], p-argv[2]);
			memcpy (ndir+9, p+1, strlen (p+1));
		} else {
			memcpy (ndir, argv[2], strlen (argv[2]));
		}
	}
	for (p = ndir; p < ndir+12; p++)
		*p = toupper (*p);

	/* Prime the comparison arrays */
	rl (old, lold, sizeof (lold));
	rl (new, lnew, sizeof (lnew));

	/* Go thru the files until eof on both */
	while (lold[0] != '\177' && lnew[0] != '\177') {
		cresult = memcmp (lold, lnew, 12);
		if (cresult < 0) {		/* old file deleted */
			++deleted;
			gecho (lold);
			rl (old, lold, sizeof (lold));
		} else if (cresult == 0) {	/* same file names */
			if (strcmp (lold, lnew)) { /* ... different attr */
				++changed;
				gcopy (lnew, argv[3]);
			}
			rl (old, lold, sizeof (lold));
			rl (new, lnew, sizeof (lnew));
		} else {			/* new file added */
			++added;
			gcopy (lnew, argv[3]);
			rl (new, lnew, sizeof (lnew));
		}
	}

	/* Display summary */
	fprintf (stderr, "scanfils:  %d added, %d deleted, %d changed", added,
		deleted, changed);
	return (0);
}

/*	gcopy
 *
 *	Generate a COPY command to the standard output.
 */

void	gcopy (s, d)

char *s;		/* string containing filename (dir listing fmt) */
char *d;		/* string containing name of destination drive */

{
	char *p;

	p = strchr (s, ' ');
	*p = '\0';
	*(s+12) = '\0';
	if (memcmp (s+9, "   ", 3))		/* if extension present */
		printf ("COPY %s.%s %s\n", s, s+9, d);
	else
		printf ("COPY %s %s\n", s, d);
	*p = ' ';
	*(s+12) = ' ';
	return;
}

/*	gecho
 *
 *	Generate an ECHO command to the standard output.
 */

void	gecho (s, d)

char *s;		/* string containing filename (dir listing fmt) */

{
	char *p;

	p = strchr (s, ' ');
	*p = '\0';
	*(s+12) = '\0';
	if (memcmp (s+9, "   ", 3))		/* if extension present */
		printf ("ECHO %s.%s\n", s, s+9);
	else
		printf ("ECHO %s\n", s);
	*p = ' ';
	*(s+12) = ' ';
	return;
}

/*	rl
 *
 *	Read a line from the input file specified.
 *	If at end of file, a byte of 0xFF is put in the first byte.
 *	Lines beginning with spaces are skipped (Lines in the directory
 *	listing that we're uninterested in start with a space!)
 *	Ditto for lines beginning with a period.
 *	Ditto for lines that match either the "old" or "new" filenames.
 */

void rl (stream, s, len)

FILE *stream;
char *s;
int len;

{
	char *p;

	do {
		if (feof (stream))
			 *s = '\177';
		else
			if (! fgets (s, len, stream))
				*s = '\177';
	} while (*s <= ' ' || *s == '.' || !memcmp (odir, s, 12) ||
		!memcmp (ndir, s, 12));
	return;
}

/*	usage
 *
 *	Display usage message and exit
 */

void usage ()

{
	printf ("scanfils usage:  olist nlist drive >output\n");
	printf ("\n");
	printf ("where olist:	Sorted old directory listing\n");
	printf ("      nlist:	Sorted new directory listing\n");
	printf ("      drive:	Output drive in generated COPY commands\n");
	printf ("\n");
	printf ("Sorted directory listings may be generated via DIR | SORT");
	printf (" >GLOP\n");

	exit (1);
}
