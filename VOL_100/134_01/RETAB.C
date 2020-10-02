/*	RETAB.C		*/

/*	This code is in the public domain	*/

/*	The following program is written to illustrate the use of
   coroutines to simulate Unix-style pipelines.  It consists of the
   Software Tools programs "entab" and "detab", hooked together:

	detab | entab

	Each command is implemented as a coroutine; the GETCHAR and
   PUTCHAR over the pipeline is replaced with the appropriate RESUME.
   Perhaps some ambitious hacker can build a general pipeline mechanism
   atop the coroutine package?
*/

#include <bdscio.h>
#include <dio.h>
#include <coro.h>
#include <cmdutil.h>

/*	External storage	*/

/*	Storage for the tab positions for entab and detab	*/

char ntabpos [MAXLINE];
char dtabpos [MAXLINE];

/*	Coroutine FCV pointers	*/

struct c_fcv_ * ntabco;
struct c_fcv_ * dtabco;

/*	Main program	*/

main (argc, argv)
    int argc;
    char ** argv;
    {

	int entab ();		/* "Main program" of the ENTAB coroutine */
	int detab ();		/* "Main program" of the DETAB coroutine */

	_allocp = NULL;		/* pre-1.5: Initialize the allocator	*/

	dioinit (&argc, argv);	/* Initialize directed I/O 		*/

	getopt (argc, argv);	/* Process command line options		*/

	/*	Create the two coroutines, giving each 1K bytes of stack
	   space.  */

	ntabco = c_create (&entab, 1024);
	dtabco = c_create (&detab, 1024);

	/*	The following call wakes up the "entab" coroutine.  The
	   first character it needs will wake up the "detab" coroutine
	   via "c_resume".  The first of the two coroutines to terminate
	   (via detach or return) will come back here.
	*/

	c_call (ntabco);

	/*	One or the other coroutine has terminated.  If "detab"
	   ended prematurely, pass an EOF on to "entab" to get it to
	   quit.
	*/

	if (c_passer () == dtabco) c_call (ntabco, EOF);

	/*	All done; kill the two coroutines.	*/

	c_destroy (dtabco);
	c_destroy (ntabco);

	/*	Get rid of redirected I/O	*/

	dioflush ();

	/* Th-th-th-that's all, folks. */

	}

/*	GETOPT procedure.	*/

/*	The following procedure reads the tab stops from the command line
   and installs them into the "ntabpos" and "dtabpos" arrays.	*/

getopt (argc, argv)
    int argc;
    char ** argv;
    {
	struct option optable [3];
	int *info;

	initv (optable, EOF,
		"Detab", MNVL_KWD,
#define DETAB_OPT 0
		"eNtab", MNVL_KWD,
#define ENTAB_OPT 1
		EOF);

	++argv;			/* Discard command name */
	--argc;

	deftab (dtabpos);	/* Set up default tab stops */
	deftab (ntabpos);

	while (argc) {		/* Process the command line */

	    switch (procarg (&argc, &argv, optable, &info)) {
		case DETAB_OPT:
			settab (dtabpos, info);
			break;
		case ENTAB_OPT:
			settab (ntabpos, info);
			break;
		default:
			showsyntax ("retab", optable);
			exit ();
		}
	    }
	}

/*	Define default tab stops (same as CP/M)		*/

deftab (posns)
    char *posns;		/* Vector defining tab positions */
    {
	int i;

	for (i=1; i<MAXLINE; ++i)
		posns [i] = ((i % 8) == 1); /* Position 1 and every 8
						columns thereafter */
	}

/*	Define user-specified tab stops		*/

settab (posns, info)
    char *posns;			/* Vector defining tab positions */
    int *info;			/* Command line argument list */
    {
	int i;
	int j;
	int prevpos;

	prevpos = 1;

	setmem (posns, MAXLINE, 0);	/* Initialize to no tabs */

	for (i=1; i <= *info; ++i) {	/* Loop thru specified positions */
		if (info [i] >= 0) {
			posns [info [i]] = TRUE;
			prevpos = i;
			}
		else {
			for (j = prevpos; j < MAXLINE; j -= info [i])
				posns [j] = TRUE;
			}
		}
	}

/*	DETAB procedure		*/

/*	This is the DETAB coroutine.  With the exception of the fact that
   it uses RESUME instead of PUTCHAR, it's straight out of Software Tools */

detab () {
	int col;		/* Current column within the line */
	int c;			/* Current character */

	col = 1;

	while ((c = getchar ()) != EOF && c != CPMEOF) {
				/* Read a character from source file */

		if (c == '\t') {	/* TAB */
			do {
				c_resume (ntabco, ' ');
				++col;
				} while (col < MAXLINE && !dtabpos [col]);
			}

		else if (c == '\r' || c == '\n') {
					/* CR, NEWLINE */
			c_resume (ntabco, c);
			col = 1;
			}

		else {			/* Anything else */
			c_resume (ntabco, c);
			++col;
			}
		}
	}

/*	ENTAB coroutine		*/

/*	Just like DETAB, this one is straight out of SOFTWARE TOOLS.  It
   calls RESUME instead of GETCHAR to get its input from the pipeline. */

entab () {
	int col;		/* Current column in line */
	int newcol;		/* Column following a tab */
	int c;			/* Current character */

	col = 1;

	while (1) {
		newcol = col;

		while ((c = c_resume (dtabco)) == ' ') {
			++newcol;
			if (ntabpos [newcol]) {
				putchar ('\t');
				col = newcol;
				}
			}

		for ( ; col < newcol; ++col) putchar (' ');

		if (c == EOF) return;

		putchar (c);

		if (c == '\n' || c == '\r')
			col = 1;
		else
			++col;
		}
	}
character from source file */

		if (c == '\t') {	/* TAB */
			do {
				c_resume (ntabco, ' ');
				++col;
		