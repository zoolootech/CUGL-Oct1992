/* 
	Directed I/O package for BDS C
	Source:   dio.c
	Version:  February 21, 1983

	This file was created from the BDS C v1.45 version of dio.c by:

		Edward K. Ream
		1850 Summit Avenue
		Madison, WI. 53705
		(608) 231 - 2952

	See the file dio.doc for complete documentation.

	All changes to original code marked by:  ----- CHANGE -----
*/


#include bdscio.h
#include dio.h


#define CON_INPUT	1   /* BDOS call to read console     */
#define CON_OUTPUT	2   /* BDOS call to write to console */
#define CON_STATUS	11  /* BDOS call to get status       */

#define CONTROL_C	3   /* Quit character		     */
#define STDERR		4   /* Standard Error descriptor     */
#define INPIPE		2   /* bit setting to indicate directed
			       input from a temp. pipe file  */
#define VERBOSE		2   /* bit setting to indicate output
			       is to go to the console AND to
		   	       the directed output */

/* ----- CHANGE ----- a new definition */
#define LST_OUT    5		        /* BDOS list output */



/* Redirect the STD_IN, STD_OUT, and STD_ERR streams. */

#define argc *argcp

dioinit(argcp, argv)
int *argcp;
char **argv;
{
	int i,j, argcount;

	/* No directed I/O by default. */
	/* ----- CHANGE ----- some new variables added */
	_diflag = _doflag = _deflag = _pipef = FALSE;
	_dispec = _dospec = _despec = ERROR;
	_nullpos = &argv [argc];

#ifdef BUF_CONS
	_conbuf[0] = 0;			/* no characters in buffer yet	*/
	_conbufp = _conbuf;		/* point to null buffer 	*/
#endif

	argcount = 1;

	/* Scan the command line for < and >  and |  */
	for (i = 1; i < argc; i++) {

		/* Stop if the last thing we saw was a pipe. */
		if (_pipef) {
			break;
		}

		switch (*argv [i]) {


case '<':	/* Directed input. */

		if (!argv [i] [1]) {
			/* ----- CHANGE ----- avoid goto */
			fprintf(STD_ERR,
				"bad '<' redirection\n");
			exit();
		}

		else if (fopen(&argv[i][1], _dibuf) == ERROR) {
			fprintf(STD_ERR,"Can't open %s\n",
				&argv [i] [1]);
			exit();
		}

		_diflag = TRUE;
		if (strcmp(argv[i],"<TEMPIN.$$$") == 0) {
			 _diflag |= INPIPE;
		}

		goto movargv;

/* Pipes:
 *	The vertical bar (|) separates programs and arguments.
 *	For the CURRENT program, only arguments up to the bar
 *	are visible.  The remaining arguments are passed to
 *	the following program in the pipe by calling execv().
 *
 *	_pipedist points to argument following the bar.
 *	It must be name of a program.
 *	_savei points to the LIST of arguments which will
 *	be passed to execv() by dioflush().
 */

case '|':	/* pipe */

		if (!argv [i] [1]) {
			/* ----- CHANGE ----- avoid goto */
			fprintf(STD_ERR,
				"bad pipe specifier\n");
			exit();
		}

		/* We are now in pipe mode. */
		_pipef++;

		/* Point to the FOLLOWING program's name. */
		_pipedest = &argv [i] [1];

		if (argv [i] [1]) {
			/* Convert the bar to a file name. */
			argv [i] = ">TEMPOUT.$$$";

			/* Point to the FOLLOWING program's
			 * argument list.
			 */
			_savei = &argv [i];
		}

		goto out;

/* ----- CHANGE -----	this whole case is new.
 *			If you use this you must make a
 *			slight modification to putc().
 *			The new putc is also on this file.
 */

case '@':	/* Directed error output */
		/* Requires a slight mod to  putc() */

		if (!argv [i] [1]) {
			fprintf(STD_ERR,
				"bad '@' redirection\n");
			exit();
		}

		if (strcmp (&argv [i] [1], PRINTER) == 0){
			_despec = DEV_LST;
		}
		if (_despec != ERROR) {
			_deflag++;
			goto movargv;
		}

		unlink (&argv [i] [1]);
		if (fcreat (&argv [i] [1], _debuf) == ERROR) {
			fprintf(STD_ERR, "Can't creat %s\n",
				&argv [i] [1]);
			exit();
		}
		_deflag++;
		goto movargv;
		

case '+':       /* Verbose output to console and file. */

		_doflag |= VERBOSE;
			
/* ----- CHANGE ----- this label used to be foo. */
		
out: case '>':	/* Directed output */
	
		if (!argv [i] [1])  {
			/* ----- CHANGE ----- */
			fprintf(STD_ERR,
			    "Bad '>' redirection\n");
			exit();
		}

		/* ----- CHANGE ----- look for special files */
		if (strcmp (&argv [i] [1], PRINTER) == 0){
			_dospec = DEV_LST;
		}
		if (_dospec != ERROR) {
			_doflag++;
			goto movargv;
		}

		unlink(&argv [i] [1]);
		if (fcreat(&argv [i] [1], _dobuf) == ERROR) {
		       fprintf(STD_ERR,"Can't create %s\n",
				&argv[i][1]);
		       exit();
		}
		_doflag++;

		/* Common redirected I/O processing:
		 *
		 */

movargv:	if (!_pipef) {

			/* Delete the redirected argument. */
			for (j = i; j < argc; j++) {
				argv[j] = argv[j+1];
			}
			(argc)--;
			i--;
			_nullpos--;
		}
		else {
			/* Make args after the bar invisible
			 * until the next program starts.
			 */
			argc = argcount;
			argv [argc] = 0;
		}
		break;

default:	/* normal argument */
		argcount++;

		}
	}
}

#undef argc


/* Close all files and chain to the next program.
 * Abort any open pipe if errcode is not zero.
 */

dioflush()
{
	/* Close the standard input stream. */
	if (_diflag && _dispec == ERROR) {
		fclose(_dibuf);
		if (_diflag & INPIPE) {
			unlink("tempin.$$$");
		}
	}

	/* Close the standard output stream. */
	if (_doflag && _dospec == ERROR) {
		putc(CPMEOF,_dobuf);
		fflush(_dobuf);
		fclose(_dobuf);

		/* in case previous pipe was aborted */
		unlink("tempin.$$$");
		rename("tempout.$$$","tempin.$$$");
	}

	/* Close the error output stream. */
	/* ----- CHANGE ----- this is new code */
	if (_deflag && _despec == ERROR) {

		putc(CPMEOF, _debuf);
		fflush(_debuf);
		fclose(_debuf);
        }

	/* Start executing the NEXT program in the pipe
	 * by calling execv (_pipedest, _savei).
	 * As you will recall, _pipedest and _savei are
	 * set up by initst().
	 */
	/* ----- CHANGE ----- this test is different */
	if (_doflag && _pipef)  {

		/* The first arg will redirect the input. */
		*_savei = "<TEMPIN.$$$";

		/* Wipe out the zero'th argument. */
		*_nullpos = NULL;

		/* Fire up the next program.
		 * The arguments to execv are:
		 *	1.  The name of the program to execute.
		 *	2.  A pointer to the arg list.
		 */
		
		if (execv(_pipedest,_savei) == ERROR) {
			fprintf(STD_ERR, "\7Broken pipe\n");
			exit();
		}
	}
}


/*
	This version of "getchar" replaces the regular version when using
	directed I/O. Note that the "BUF_CONS" defined symbol (in DIO.H)
	controls whether the console input is to be raw or buffered (see
	NOTES section of dio.doc)
*/

getchar()
{
	int c;

	/* ----- CHANGE ----- consider _dispec as well */
	if (_diflag && _dispec == ERROR) {
		if ((c = getc(_dibuf)) == '\r') c = getc(_dibuf);
	}
	else

#ifdef BUF_CONS		/* For buffered console input, get a line of text   */
	{		/* from the BDOS (using "gets"), & insert newline:  */
		if (!*_conbufp) {   
			gets(_conbufp = _conbuf);
			_conbuf[strlen(_conbuf) + 1] = '\0';
			_conbuf[strlen(_conbuf)] = '\n';
		}
		c = *_conbufp++;
	}
#else			/* for raw console input, simulate normal "getchar": */
		if ((c = bdos(CON_INPUT)) == CONTROL_C) exit();
#endif

	if (c == CPMEOF) return EOF;	     /* Control-Z is EOF key 	*/

	if (c == '\r') 
	{
		c = '\n';
#ifndef BUF_CONS
		if (!_diflag) bdos(2,'\n');  /* echo LF after CR to console */
#endif
	}
	return c;
}



/*
	This version of "putchar" replaces the regular version when using
	directed I/O:
*/

putchar(c)
char c;
{
	char *static;
	static = "";	/* remembers last character sent; start out null */

	/* ----- CHANGE ----- wait on ^S, abort pipe on ^C. */
	chkkey();

	/* ----- CHANGE ----- consider _dospec as well */
	if (_doflag && _dospec == ERROR)
	{
		if (c == '\n' && *static != '\r') putc('\r',_dobuf);
		*static = c;
		if(putc(c,_dobuf) == ERROR)
		{
			fprintf(STDERR,"File output error; disk full?\n");
			exit();
		}

		/* ----- CHANGE ----- BUG FIX return c */
		if (!(_doflag & VERBOSE)) return c;
	}

	/* ----- CHANGE ----- this whole section is new */
	if (_doflag && _dospec == DEV_LST) {

		/* Send character to the list device. */
		if (c == '\n' && *static != '\r') {
			/* comment out -----
			bdos (LST_OUT, '\r');
			----- end comment out */
			putc('\r', DEV_LST);
		}
		*static = c;
		/* comment out -----
		bdos (LST_OUT, c);
		----- end comment out */
		putc(c, DEV_LST);
		return c;
	}

	if (c == '\n' && *static != '\r') bdos(CON_OUTPUT,'\r');
	bdos(CON_OUTPUT,c);
	*static = c;

	/* ----- CHANGE ----- return c! */
	return c;
}


/* A new version of putc() for use with directed I/O to special files. */

/* comment out -----
#define ACKNAK 1
----- end comment out */

#define DAV 1		/* data available bit for printer	*/
#define SIIA  0x14	/* data port for printer		*/
#define SIIAC 0x16	/* control port for printer		*/

int putc(c,iobuf)
char c;
FILE *iobuf;
{
	/* ----- CHANGE ----- wait on ^S, abort pipe on ^C. */

	/* ----- CHANGE ----- this number used to be 4 */

	if (iobuf <= 5)			/* handle special device codes: */
	 {
		switch (iobuf)
		 {
			case STD_OUT:	return putchar(c);  /* std output */
			case DEV_LST:	
#ifdef ACKNAK
					/* output the character */
					bdos(5,c);
					if (c != '\n') {	
						return c;
					}
					/* output ACK */
					bdos(5,3);
					/* wait for status */
					while ( (inp(SIIAC) & DAV) == 0) {
						;
					}
					/* wait for NAK */
					while ( (inp(SIIA) & 0x7f) != 6) {
						;
					}
					return c;
#else
					return (bdos(5,c)); /* list dev.  */
#endif

			case DEV_PUN:	return (bdos(4,c)); /* to punch   */

			/* ----- CHANGE ----- do more here */
			case STD_ERR:	return _puterr(c);

			/* ----- CHANGE ----- add DEV_TTY  */
			case DEV_TTY:
				 	if (c == '\n') {
						bdos(2,'\r');
					}
					return bdos(2,c);
		 }	
	 }
	if (!iobuf -> _nleft--)		/*   if buffer full, flush it 	*/
	 {
		if ((write(iobuf -> _fd, iobuf -> _buff, NSECTS)) != NSECTS)
			return ERROR;
		iobuf -> _nleft = (NSECTS * SECSIZ - 1);
		iobuf -> _nextp = iobuf -> _buff;
	 }
	return *iobuf -> _nextp++ = c;
}


/* ----- CHANGE ----- this whole routine is new. */
/* Utility routine for putc. */

_puterr(c)
char c;
{
	if (_deflag && _despec == ERROR) {
		/* direct error output to a file. */
		return putc(c, _debuf);
	}
	else if (_deflag && _despec == DEV_LST) {
		return putc(c, DEV_LST);
	}
	else {
		/* Direct error output to the console. */
		return putc(c, DEV_TTY);
	}
}


/* ----- CHANGE ----- this whole routine is new */
/*
	Look for an interrupt from the keyboard.
	Suspend processing on ^S until another key hit.
	Abort program and any open pipe on ^C.
	This program uses calls to BIOS to avoid BDOS weirdnesses.
*/

chkkey()
{
	int c;

	/* Character ready ? */
	if (bios(2,0) == 0) {
		return;
	}
	/* Read the character.  Do NOT echo. */
	c = bios(3,0) & 0x7f;

	/* Abort pipe on control-C. */
	if (c == 3) {
		dioflush();
		exit();
	}

	/* Control S.  Wait for any character. */
	else if (c == 19) {
		while (bios(2,0) == 0) {
			;
		}
		/* Read the character to clear status. */
		bios(3,0);
		return;
	}

	/* All other characters. */
	return;
}
;
		}
		/* Read the character to clear status. */
		bios(3,0);
		return;
	}

	/* All other characters. *