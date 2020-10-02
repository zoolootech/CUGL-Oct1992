/*
	New version of dio.h
	source:  dio.h
	version: May 14, 1982; November 7, 1982; February 22, 1983

	adapted from dio.h by:

	Edward K. Ream
	1850 Summit Ave.
	Madison, WI 53705
	(608) 231 - 2952
*/


/*
the new version of dio.h is very similar to the old
these exceptions:


1.  I have added the following variables to dio.h:
    _deflag, _dispec, _dospec, _despec and _debuf
    These variables are needed by dio.c.

2.  The DEV_TTY definition is new.

3.  I have added comments about i/o redirection.

4.  The constants STD_IN, STD_OUT etc were copied
    to this file from STDLIB1.C.

All changes from the old version of dio.h are marked:
----- CHANGE -----

file information:

   Units 0, 1, and 4 are the STD_IN, STD_OUT and STD_ERR units.
By default, these units are the same as the user's console,
but these units may be redirected to files (or special files)
via the command line characters >, < and @.

   Units 2 and 3 and 5 are special files.
Each special file is assigned permanently to one device.
Units 0, 1, and 4 may be redirected to these files.
Additionally, I/O may be done directly to these
files without first doing a call to fopen().

   Unit 5 is a new special file, DEV_TTY.  This is always
the user's console.  I/O redirection has NO effect on this
special file.  If you say putc(c, DEV_TTY) then c will go
to the console even if STD_OUT has been directed elsewhere.

   All other units are disk files.
These units are usually opened by calling fopen().
However, units 0, 1, and 4 may be redirected to disk units
via the command line.

*/


/* ----- CHANGE ----- constants copied from STDLIB1.C */

#define STD_IN	0
#define STD_OUT 1
#define STD_ERR 4

#define DEV_LST 2
#define DEV_RDR 3
#define DEV_PUN 3

/* ----- CHANGE ----- new definitions */

#define DEV_TTY 5		/* ALWAYS the console */
#define PRINTER  "PRINTER"	/* user's printer */


/*
	External data used by DIO.C for directed I/O simulation:
*/

/*
#define BUF_CONS 1	/* uncomment if console buffering is
			desired	(see DIO.C)	   */
*/

char _diflag, _doflag;	/* flag if directed I/O being used */

/* ----- CHANGE -----	define 3 more flags.
 *			at present, _dispec is not used.
 */

char _deflag;	/* like _deflag and _deflag but for STD_ERR */
int _dispec, _dospec, _despec;	/* ERROR or direct output to
				 * a special file.
				 */

char _pipef, *_pipedest;   /* true if a pipe is being filled */
char **_savei, **_nullpos; /* used to remember position in
			      command line when piping 	     */
char _dibuf[BUFSIZ], _dobuf[BUFSIZ];	/* I/O buffers used for direction  */

/* ----- CHANGE ----- a new buffer is needed */

char _debuf[BUFSIZ];

#ifdef BUF_CONS				/* console buffering data	   */

char _conbuf[MAXLINE + 2];		/* console input buffer used for 
					   non-directed standard input	   */
char *_conbufp;				/* pointer to next character to
						read from console buffer   */
#endif
irected standard input	   */
char *_conbufp;				/* pointer to next