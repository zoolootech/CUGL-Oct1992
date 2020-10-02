/*	DIFF - File Comparator	*/

/*	Copyright (c) 1983 by Kevin B. Kenny

	Released to the BDS `C' Users' Group for non-commercial distribution
	only.

	Kevin Kenny
	729-A E. Cochise Dr.
	Phoenix, Arizona   85020
*/


#include <bdscio.h>
#include <dio.h>
#include <cmdutil.h>

/*	Command line options 	*/

int dispmode;			/* Display mode */
#define DM_NORM 0			/* Normal display */
#define DM_ALTER 1			/* Display alterations only */
#define DM_MATCH 2			/* Display matching lines */

int resync;			/* Resynchronization count */
char * header;			/* Header for display */
int blanksf;			/* Flag = 1 if blanks s/b compressed */
int single;			/* Flag = 1 to ignore case differences */
int debug;			/* Flag = 1 if debugging */
char * file1;			/* Name of first file */
char * file2;			/* Name of second file */

/*	Input file status	*/

FILE file [2];			/* IObuf's of input files */

struct file_line {
	struct file_line *next, *prev;	/* Chain linkage */
	int line_no;			/* Line number in file */
	char * line_text;		/* Text of the line */
	};
struct file_line * first_line [2];	/* First line from file in memory */
struct file_line * last_line [2];	/* Last line from file in memory */
struct file_line * cur_line [2];	/* Current line in memory */
int linenumb [2];			/* Last line read from each file */
int eofhit [2];				/* EOF read in from file */

/*	Status of resynchronizer	*/

int equal;              /* Flag to indicate equality */
int match;				/* Flag to indicate match reached */
int left;				/* File being read backwards */
int right;				/* FIle being read forwards */
int loffset;				/* Number of mismatches in "left" */
int roffset;				/* Number of mismatches in "right" */
int difflen [2];			/* Number of mismatches by file */
int hold;				/* Working word for pointer switches */

main (argc, argv)
    int argc;
    char ** argv;
    {
	_allocp = NULL;
	dioinit (&argc, argv);
	getopts (argc, argv);
	setup ();
	rundiff ();
	dioflush ();
	}

/* 	Read an process the options from the command line	*/

struct option optable [7];	/* Option descriptions for procarg */

getopts (argc, argv)
    int argc;
    char ** argv;
    {

	int info;		/* Information item returned from procarg */
	int error;		/* Error flag */

	initv (optable, EOF,
#define FILE_OPT -1			/* File name */
		"Alterations", NAKED_KWD,
#define ALTER_OPT 0			/* Flag to show just changed text */
		"Matching", NAKED_KWD,
#define MATCH_OPT 1			/* Flag to show just matching text */
		"Resynchronize", NVAL_KWD,
#define RESYN_OPT 2			/* Line count for resynchronization */
		"Header", SVAL_KWD,
#define HEAD_OPT 3			/* Header for output lines */
		"Blanks", NAKED_KWD,
#define BLANK_OPT 4			/* Ignore multiple blanks */
		"Singlecase", NAKED_KWD,
#define SINGLE_OPT 5
		"Zdebug", NAKED_KWD,
#define DEBUG_OPT 6			/* Turn on internal traces */
		EOF);

	dispmode = DM_NORM;		/* Set default options */
	resync = 3;
	header = "-------------------------------------------------- ";
	blanksf = FALSE;
	debug = FALSE;
	single = FALSE;
	file1 = NULL;
	file2 = NULL;

	++argv; --argc;		/* Discard command name from argv */

	error = FALSE;		/* No error has occurred yet */

	while (argc) {		/* Process one command option */
		switch (procarg (&argc, &argv, optable, &info)) {

		    case ALTER_OPT:
			dispmode = DM_ALTER; break;
		    case MATCH_OPT:
			dispmode = DM_MATCH; break;
		    case RESYN_OPT:
			resync = info; break;
		    case HEAD_OPT:
			header = info; break;
		    case BLANK_OPT:
			blanksf = TRUE; break;
		    case SINGLE_OPT:
			single = TRUE; break;
		    case DEBUG_OPT:
			debug = TRUE; break;
		    case FILE_OPT:
			if (file1 == NULL) file1 = info;
			else if (file2 == NULL) file2 = info;
			else error = TRUE;
			break;
		    default:
			error = TRUE;
		    }
		}

	if (file2 == NULL || error) {
		showsyntax ("diff <file1> <file2>", optable);
		exit ();
		}
	}

/*	Set up the DIFF process		*/

setup () {

	if (fopen (file1, file [0]) == ERROR) {
		fprintf (STD_ERR, "%s: can't open file.\n", file1);
		exit ();
		}
	if (fopen (file2, file [1]) == ERROR) {
		fprintf (STD_ERR, "%s: can't open file.\n", file2);
		exit ();
		}
	first_line [0] = first_line [1] =
	last_line [0] = last_line [1] =
	cur_line [0] = cur_line [1] =		NULL;
	linenumb [0] = linenumb [1] = 0;
	eofhit [0] = eofhit [1] = FALSE;
	equal = TRUE;		/* equal so far; */
	fprintf (STD_ERR, "K**2 DIFF version 83-06-07\n");
	fprintf (STD_ERR, "copyright (c) 1983 by Kevin Kenny.\n");
	}

/*	Do the DIFF process		*/

rundiff () {
	while (!ateof (0) || !ateof (1)) {	/* Run until end of files */
		domatch ();			/* Handle matching lines */
		if (ateof (0) && ateof (1)) break;
		equal = FALSE;			/* We've found a difference */
		synch ();			/* Get us back in sync */
		dodiff ();			/* Handle changed lines */
		}
	if (equal) fprintf (STD_ERR, "\nTwo files are equal.");
	}

/*	Do matching lines		*/

domatch () {
	if (debug) fprintf (STD_ERR, "in Domatch\n");
	if (!atmatch ()) return;		/* Handle non-match at start */
	
	if (dispmode == DM_MATCH) 		/* Displaying matches? */
		printf ("%s%d, %d\n", header, lineno (0), lineno (1));

	while (atmatch () && !ateof(0)) {
		if (dispmode == DM_MATCH)	/* Display matching lines */
			printf ("%s", curline (0));
		advance (0); advance (1);	/* Bump past matching lines */
		discard (0); discard (1);	/* Throw away matched lines */
		}
	}

/*	Resynchronize comparison when mismatched lines found	*/

synch () {
	if (debug) fprintf (STD_ERR, "in synch\n");
	left = 0;			/* Move pointer leftward from
					   current position in first file */
	right = 1;			/* Move pointer rightward in second
					   file. */
	loffset = roffset = 0;		/* Both pointers at current line */

	do {
		++roffset; advance (right);
					/* Move pointer rightward */
		if (--loffset >= 0) 	/* Back leftward pointer up */
			backup (left);
		else {			/* Pointer reached divergence point */
			if (bdos (6, 0xFF) == 0x03) {
				fprintf (STD_ERR, "\nDIFF aborted.");
				exit ();
				}
			hold = right; right = left; left = hold;
					/* Swap input files */
			loffset = roffset; roffset = 0;
					/* Switch pointers */
			}

		if (debug) fprintf (STD_ERR,
			"synch loop: left=%d right=%d loffset=%d roffset=%d\n",
					left, right, loffset, roffset);

		match = TRUE;		/* Innocent until proven guilty */
		for (hold = 0; hold < resync; ++hold) {
			if (!atmatch ()) {	/* Test if at least "resync" */
				match = FALSE;	/* consecutive lines match   */
				break;
				}
			advance (0); advance (1);
						/* Advance over matched lines*/
			}
		while (hold) {
			backup (0); backup (1);	/* Back up over matched lines*/
			--hold;
			}
		
		} while (!match);	/* Repeat synch until match found */

	difflen [left] = loffset;
	difflen [right] = roffset;
	while (loffset) {
		backup (left); --loffset;
		}			/* Back up over unmatched data */
	while (roffset) {
		backup (right); --roffset;
		}
	}

/*	Process differences	*/

dodiff () {

	if (debug) fprintf (STD_ERR, "in dodiff\n");

	if (dispmode != DM_MATCH) printf ("%s     %d", header, lineno (0));
	if (dispmode == DM_ALTER && difflen [0] != 0)
		printf (", %d", lineno (0) + difflen [0] - 1);
	if (dispmode != DM_MATCH) printf ("\n");

	for (hold = 0; hold < difflen [0]; ++hold) {
		if (dispmode == DM_NORM)
			printf ("%s", curline (0));
		advance (0);
		}

	if (dispmode == DM_NORM) printf ("%s%d\n", header, lineno (1));

	for (hold = 0; hold < difflen [1]; ++hold) {
		if (dispmode != DM_MATCH)
			printf ("%s", curline (1));
		advance (1);
		}

	}

/*	Test if a file is at EOF	*/

int ateof (f)
    int f;
    {
	prepbuf (f);
	return (cur_line [f] -> line_text == EOF);
	}

/*	Return the current line of a file	*/

char * curline (f)
    int f;
    {
	prepbuf (f);
	return (cur_line [f] -> line_text);
	}

/*	Return the current line number within file */

int lineno (f)
    int f;
    {
	prepbuf (f);
	return (cur_line [f] -> line_no);
	}

/*	Test if the current lines of the two files match 	*/

int atmatch () {
	char *s1, *s2;
	if (debug) fprintf (STD_ERR, "in atmatch\n");

	prepbuf (0); prepbuf (1);
	if (ateof (0) && ateof (1)) return (TRUE);
	if (ateof (0) || ateof (1)) return (FALSE);

	s1 = curline (0); s2 = curline (1);
	while (		*s1 == *s2
	   ||  (blanksf && isspace (*s1) && isspace (*s2))
	   ||  (single  && tolower (*s1) == tolower (*s2))) {
		if (*s1 == 0) return (TRUE);
		if (blanksf && isspace (*s1)) {
			while (isspace (*++s1));
			while (isspace (*++s2));
			}
		else {
			++s1; ++s2;
			}
		}
	return (FALSE);
	}

/*	Advance to the next line of a file 	*/

advance (f) 
    int f;
    {
	cur_line [f] = cur_line [f] -> next;
	prepbuf (f);
	}

/*	Back up to the proevious line in a file */

backup (f)
    int f;
    {
	if ((cur_line [f] = cur_line [f] -> prev) == NULL) {
		fprintf (STD_ERR, "\nin backup: can't happen");
		exit ();
		}
	}

/*	Discard all lines in memory up to the present point	*/

discard (f)
    int f;
    {
	while (first_line [f] != cur_line [f]) {
		if (first_line [f] -> line_text != EOF)
			free (first_line [f] -> line_text);
		hold = first_line [f] -> next;
		free (first_line [f]);
		first_line [f] = hold;
		}
	if (first_line [f] == NULL) {
		last_line [f] = NULL;
		}
	}

/*	Bring the current line to memory if it isn't there	*/

prepbuf (f)
    int f;
    {
	struct file_line * newl; char * txt;
        char text [MAXLINE];
	if (cur_line [f] != NULL) return;

	newl = alloc (sizeof (* newl));
	if (!newl) goto punt;
	if (!eofhit [f] && fgets (text, file [f])) {
		txt = alloc (strlen (text) + 1);
		if (!txt) goto punt;
		movmem (text, txt, strlen (text) + 1);
		newl -> line_text = txt;
		}
	else {
		newl -> line_text = EOF;
		eofhit [f] = TRUE;
		}
	newl -> line_no = ++ linenumb [f];

	if (last_line [f] != NULL) {
		last_line [f] -> next = newl;
		}
	else {
		first_line [f] = newl;
		}

	newl -> prev = last_line [f];
	newl -> next = NULL;
	last_line [f] = cur_line [f] = newl;
	return;

punt:	fprintf (STD_ERR, "Files have too many differences to compare.");
	exit();
	}
] -> line_text);
		hold = first_line [f] -> next;
		free (first_line [f]);
		first_line [f] = hold;
		}
	if (first_line