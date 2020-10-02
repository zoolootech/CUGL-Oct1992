/* DIOR.C         VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:32 PM */
/*
%CC1 $1.C -X
*/
/* 
Description:

IO redirection library from BDS C and Van Nuys Toolkit, 
by Eugene H. Mallory.

Minor modification:
	avoid CR-CR pair before LF by patch in putchar() of DIOR.C and by 
		adding declaration of _putc_last in DIO.H;

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/
/*********************************************************************
*                               DIO                                  *
*                Written by Gene Mallory  12/24/83                   *
**********************************************************************
*                  COPYRIGHT 1983 EUGENE H. MALLORY                  *
*********************************************************************/
/*
	Redirection can take the following forms:
	
	    > fid	Output redirection to file
	    < fid	Input rediredtion from file
	    + fid	Output redirection to file and screen
	    ~ fid	Output redirection to file and printer
	    >> fid	Append to file
	    +> fid	Append to file and output to screen
	    ~> fid	Append to file and output to printer
	    > LST:	Output to LST device
	    >> LST:	As above
	    + LST:	Output to LST device and screen
	    > LP1:	Output to LP1 (IOBYTE = 0x80)
	    >> LP1:	As above
	    + LP1:	As above and to screen
	    > LP2:	Output to LP2 (IOBYTE = 0xC0)
	    >> LP2:	As above
	    + LP2:	As above and to screen
	    > OUT:	Output to PUNCH DEVICE
	    >> OUT:	As above
	    + OUT:	As above and to screen
	    ~ OUT:	As above and to printer
	    < IN:	Input from READER device
	    
	    | prog	Pipe output to next program
	    +| prog	As above and to screen
	    ~| prog	As above and to printer
	
	Redirection and pipe character may be separated from
	the file or program name by white space or not.
	
	File specifications may be complete with user number (##) and 
	disk name (d) in the form:
	
			##/d:name
	
	Pipe specification will be first tried as is, if the file 
	does not exist, disk A, user 0 will be tried using the same 
	name.
	
	Control C will abort program and submit file execution.
	When inputing from keyboard use Control C except at
	column 1. (CP/M catches it there.)  If an old version of
	the output file existed, it will be recovered by Control C.
	
	It is permissable to input and output from the same file,
	BUT the input specification must be first and the output
	specification must be second!  otherwise the order and 
	location of the redirection specifications is not
	significant.
	
	    
	
	
	Thus, a C program using redirection has the following form:

	    #include "bdscio.h"		/* standard header file	*/
	    #include "dio.h"		/* directed I/O header	*/
	    
		...				/* other externals, if any */

	    main(argc,argv)
		char **argv;
		BEGIN
		    ...			/* declarations		     */
		    dioinit(&argc,argv)	/* initialize redirection    */
		    wildexp(&argc,&argv)/* wildexp goes here if used */
		    ...			/* body of program	     */
		    dioflush();		/* Must exit here to close   */
		END
*/
/*
	NOTES:

	1. Redirection and pipes work only for TEXT. This mechanism 
	should not be used for binary data.

	2. Use CLINK FOO DIO or L2 FOO DIO to force 
	linking with all the dio routines before the DEFFs are 
	searched.
	
	3. Library routines GETCHAR, PUTCHAR, PRINTF, PUTS are
	caught and redirected.
	
	4. New routines TYPEF, TYPECHAR FORCE OUTPUT TO 
	SCREEN.
	
	5. New routine ERROR, may be used like TYPEF to 
	output a message, close out the DIO operations
	and exit.
	

*/

#include "bdscio.h"
#include "dio.h"

/*********************************************************************
*                           DIOINIT                                  *
*********************************************************************/
dioinit(argcptr, argvect)
int *argcptr;
char **argvect;
{
	int i, c0, c1;
	_outptr = STDERR;
	_conline = MAXLINE;
	_conbufp = _conbuf;
	_conbuf[0] = 0;
	_iobyte = bdos(7, 0);        /* save iobyte */
	_screenflag = _pipeflag = _dioinflag = FALSE;
	_printerflag = _appendflag = _rdrflag = FALSE;

	for (i = 1; i < *argcptr;)
	{
		c0 = argvect[i][0];
		c1 = argvect[i][1];
		if (c0 == '+')
			_screenflag = TRUE;
		if (c0 == '~')
			_printerflag = TRUE;

		if ((c0 == '+' || c0 == '~') && c1 == '|')
		{
			strshift(argvect[i], 1);
			_dio_pipe(i, argcptr, argvect);
		}
		else
			if ((c0 == '+' || c0 == '~') && c1 == '>')
			{
				_dioappend(i, argcptr, argvect);
			}
		else
		    if (c0 == '>' && c1 == '>')
		{
			_dioappend(i, argcptr, argvect);
		}
		else
		    if (c0 == '>' || (c0 == '+' || c0 == '~'))
		{
			_dioout(i, argcptr, argvect);
		}
		else
		    if (c0 == '<')
		{
			_dioin(i, argcptr, argvect);
		}
		else
		    if (c0 == '|')
		{
			_dio_pipe(i, argcptr, argvect);
		}
		else
		{
			i++;
		}
	}
}

/*********************************************************************
*                           _DIOOUT                                  *
*********************************************************************/
_dioout(n, argcptr, argvect)
int *argcptr, n;
char **argvect;
{
	char backupfile[20];
	_outptr = _dobuf;

	if (argvect[n][0] == '+')
		_screenflag = TRUE;

	if (strlen(argvect[n]) == 1)
		shiftarg(n, argcptr, argvect);
	else
	    strshift(argvect[n], 1);

	if (!strcmp(argvect[n], "LST:"))
		_outptr = STDLST;
	else
	    if (!strcmp(argvect[n], "OUT:"))
		_outptr = STDPUN;
	else
	    if (!strcmp(argvect[n], "LP1:"))
	{
		_outptr = STDLST;
		bdos(8, (_iobyte & 0x3f) | 0x80);
	}
	else
	    if (!strcmp(argvect[n], "LP2:"))
	{
		_outptr = STDLST;
		bdos(8, (_iobyte & 0x3f) | 0xC0);
	}
	else
	    {
		strcpy(_oldfile, argvect[n]);
		getprefix(backupfile, _oldfile);
		strcat(backupfile, "BACKUP.$$$");
		unlink(backupfile);
		if (DIOIN && (strcmp(argvect[n], _infile) == 0))
		{
			fclose(_dibuf);
			if (rename(argvect[n], backupfile) == -1)
			{
				fprintf(STDERR, "DIO: Failed to rename %s to %s.\n",
				argvect[n], backupfile);
				dioexit();
			}
			if (fopen(backupfile, _dibuf) == ERROR)
			{
				fprintf(STDERR, "DIO: Unable to open file %s for input.\n"
				    , backupfile);
				fprintf(STDERR, "DIO: Error caused by %s.\n", errmsg(errno()));
				dioexit();
			}
		}
		else
		    {
			rename(argvect[n], backupfile);
		}
		if (fcreat(argvect[n], _dobuf) == ERROR)
		{
			fprintf(STDERR, "DIO: Unable to open file %s for output.\n"
			    , argvect[n]);
			fprintf(STDERR, "DIO: Error caused by %s.\n", errmsg(errno()));
			dioexit();
		}
	}
	shiftarg(n, argcptr, argvect);
}

/*********************************************************************
*                           _DIO_PIPE
*********************************************************************/
_dio_pipe(n, argcptr, argvect)
int *argcptr, n;
char **argvect;
{
	if (strlen(argvect[n]) == 1)
		shiftarg(n, argcptr, argvect);
	else
	    strshift(argvect[n], 1);

	_pipeflag = 1;
	_outptr = _dobuf;

	_pargv = &argvect[n];
	_pargc = *argcptr - n;
	*argcptr = n;

	if (fcreat("TEMPOUT.$$$", _dobuf) == ERROR)
	{
		fprintf(STDERR, "DIO: Unable to open file TEMPOUT.$$$ for pipe.\n");
		fprintf(STDERR, "DIO: Error caused by %s.\n", errmsg(errno()));
		dioexit();
	}
}

/*********************************************************************
*                           _DIOAPPEND
*********************************************************************/
_dioappend(n, argcptr, argvect)
int *argcptr, n;
char **argvect;
{
	_appendflag = TRUE;
	_outptr = _dobuf;

	if (argvect[n][0] == '+')
		_screenflag = TRUE;

	if (strlen(argvect[n]) == 2)
		shiftarg(n, argcptr, argvect);
	else
	    strshift(argvect[n], 2);

	if (!strcmp(argvect[n], "LST:"))
		_outptr = STDLST;
	else
	    if (!strcmp(argvect[n], "OUT:"))
		_outptr = STDPUN;
	else
	    if (!strcmp(argvect[n], "LP1:"))
	{
		_outptr = STDLST;
		bdos(8, (_iobyte & 0x3f) | 0x80);
	}
	else
	    if (!strcmp(argvect[n], "LP2:"))
	{
		_outptr = STDLST;
		bdos(8, (_iobyte & 0x3f) | 0xC0);
	}
	else
	    if (fappend(argvect[n], _dobuf) == ERROR)
	{
		fprintf(STDERR, "DIO: Unable to open file %s for append.\n"
		    , argvect[n]);
		fprintf(STDERR, "DIO: Error caused by %s.\n", errmsg(errno()));
		dioexit();
	}
	shiftarg(n, argcptr, argvect);
}

/*********************************************************************
*                           _DIOIN
*********************************************************************/
_dioin(n, argcptr, argvect)
int *argcptr, n;
char **argvect;
{
	if (strlen(argvect[n]) == 1)
		shiftarg(n, argcptr, argvect);
	else
	    strshift(argvect[n], 1);

	_dioinflag = 1;

	if (!strcmp(argvect[n], "IN:"))
		_rdrflag = TRUE;
	else
	    if (fopen(argvect[n], _dibuf) == ERROR)
	{
		fprintf(STDERR, "DIO: Unable to open file %s for input.\n"
		    , argvect[n]);
		fprintf(STDERR, "DIO: Error caused by %s.\n", errmsg(errno()));
		dioexit();
	}
	strcpy(_infile, argvect[n]);
	shiftarg(n, argcptr, argvect);
}

/*********************************************************************
*                           SHIFTARG
*********************************************************************/
shiftarg(n, argcptr, argvect)
int n, *argcptr;
char **argvect;
{
	int i;
	*argcptr = *argcptr - 1;
	for (i = n; i < *argcptr; i++)
	{
		argvect[i] = argvect[i + 1];
	}
}

/*********************************************************************
*                           STRSHIFT
*********************************************************************/
strshift(string, n)
int n;
char *string;
{
	int i;
	for (i = n; string[i]; i++)
		string[i - n] = string[i];
	string[i - n] = 0;
}

/*********************************************************************
*                           DIOFLUSH
*********************************************************************/
dioflush()
{
	int fid;
	char fname[20];
	if (_dioinflag && !_rdrflag)
	{
		fabort(_dibuf);
	}
	if (_outptr == _dobuf)
	{
		putc(CPMEOF, _dobuf);
		fclose(_dobuf);
	}
	bdos(8, _iobyte);        /* restore iobyte */
	if (_pipeflag)
	{
		unlink("TEMPIN.$$$");
		rename("TEMPOUT.$$$", "TEMPIN.$$$");
		strcpy(fname, _pargv[0]);
		strcat(fname, ".com");
		if ((fid = open(fname, READ)) == ERROR)
		{
			close(fid);
			strcpy(fname, "0/A:");
			strcat(fname, _pargv[0]);
			strcat(fname, ".com");
		}
		close(fid);
		_pargv[0] = "<TEMPIN.$$$";
		_pargv[_pargc] = 0;
		if (execv(fname, _pargv) == -1)
		{
			fprintf(STDERR, "DIO: Unable to _pipe to %s.\n", fname);
			dioexit();
		}
	}
	else
	    {
		unlink("TEMPOUT.$$$");
		unlink("TEMPIN.$$$");
		unlink("BACKUP.$$$");
	}
	exit();
}

/*********************************************************************
*                           PUTCHAR
*********************************************************************/
int putchar(c)
char c;
{
	if (_screenflag)
	{
		if (c == '\n')
			putc('\r', STDERR);
		putc(c, STDERR);
	}
	if (_printerflag)
	{
		if (c == '\n')
			putc('\r', STDLST);
		putc(c, STDLST);
	}
	if (c == '\n')
	{
		if (bdos(11) && bdos(1) == 3)
		{
			typef("\nDIO: Control C intercepted.\n");
			dioexit();
		}
		/*added by JAR to avoid CR-CR pair before LF*/
		if ((_putc_last & 0x7f) != '\r')
			putc('\r', _outptr);
	}
	_putc_last = c;        /*added by JAR, see above*/
	return putc(c, _outptr);
}

/*********************************************************************
*                           GETCHAR
*********************************************************************/
int getchar()
{
	int c, c2;
	if (_dioinflag && !_rdrflag)
	{
		c = getc(_dibuf);
		if (c == '\r')
		{
			if (bdos(11) && bdos(1) == 3)
			{
				typef("\nDIO: Control C intercepted.\n");
				dioexit();
			}
			if ((c2 = getc(_dibuf)) == '\n')
				c = c2;
			else
			    ungetc(c2, _dibuf);
		}
	}
	else
	    if (_rdrflag)
	{
		c = getc(STDRDR);
		if (c == '\n')
			c = getc(STDRDR);
		if (c == '\r')
		{
			if (bdos(11) && bdos(1) == 3)
			{
				typef("\nDIO: Control C intercepted.\n");
				dioexit();
			}
			c = '\n';
		}
	}
	else
	{
		/* from the BDOS (using "gets"), & insert newline:  */
		if (!*_conbufp)
		{
			typechar('*');
			bdos(10, &_conline);
			bdos(2, '\n');        /* Echo lf after line. */
			_conbuf[_concount] = '\n';
			_conbuf[_concount + 1] = 0;
			_conbufp = _conbuf;
		}
		c = *_conbufp++;
		if (c == 3)
		{
			typef("\nDIO: Control C intercepted.\n");
			dioexit();
		}
	}
	if (c == CPMEOF)
		return EOF;
	return c;
}

/****************************************************************/
/*	DIO DIRECTED GET STRING FUNCTION                        */
/*      Returns 1 at end of file, 0 normally.                   */
/****************************************************************/
int getstring(cptr)
char *cptr;
{
	int c;
	while (TRUE)
	{
		c = getchar();
		if (c == EOF || c == CPMEOF)
		{
			*cptr++ = 0;
			return 1;
		}
		if (c == '\n')
		{
			*cptr++ = c;
			*cptr++ = 0;
			return 0;
		}
		*cptr++ = c;
	}
}

/*********************************************************************
*                           TYPECHAR
*********************************************************************/
int typechar(c)
char c;
{
	return putc(c, STDERR);
}

/*********************************************************************
*                           TYPEF
*********************************************************************/
typef(format)
char *format;
{
	return _spr(&format, &typechar);
}

/*********************************************************************
*                           ERROR
*********************************************************************/
error(format)
char *format;
{
	_spr(&format, &typechar);
	typechar('\n');
	dioexit();
}

/*********************************************************************
*                           DIOEXIT
*********************************************************************/
dioexit()
{
	char backupfile[20];
	if ((_outptr == _dobuf) && !_pipeflag && !_appendflag)
	{
		putc(CPMEOF, _dobuf);
		fclose(_dobuf);
		getprefix(backupfile, _oldfile);
		strcat(backupfile, "BACKUP.$$$");
		unlink(_oldfile);
		if (rename(backupfile, _oldfile) != ERROR)
		{
			typef("DIO: Oldfile, %s, recovered from %s.\n"
			    , _oldfile, backupfile);
		}
	}
	unlink("TEMPOUT.$$$");
	unlink("TEMPIN.$$$");
	unlink("0/A:$$$.SUB");
	typef("DIO: Aborting job.\n");
	exit();
}

/*********************************************************************
*                           GETPREFIX
*********************************************************************/
getprefix(s1, s2)
char *s1, *s2;
{
	char *s1save, c;
	s1save = s1;
	while (c = *s2++)
	{
		*s1++ = c;
		if (c == '/')
		{
			s1save = s1;
		}
		if (c == ':')
		{
			s1save = s1;
			break;
		}
	}
	*s1save = 0;
	return;
}


