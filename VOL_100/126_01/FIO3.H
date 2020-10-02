/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*
"fio.3" INCLUDES fopen AND fclose, AND SHOULD BE INCLUDED BY THE LAST LINE
IN THE MAIN SOURCE FILE. INCLUSION OF fio.3 IN MAIN IS NECESSARY TO GIVE THESE
FUNCTIONS ACCESS TO NIOBUFS IN THE EXTERNAL VARIABLE bufuse[0], SINCE PASSING
NIOBUFS AS AN ARGUMENT WOULD MAKE THE FUNCTIONS INCOMPATIBLE WITH STANDARD K&R
C.

"bdscio.h" SHOULD BE MODIFIED TO INCLUDE THIS DEFINITION:

#define FILE struct _buf (SEE BDSCIO+.H)

There is one non-standard feature in the present fopen: if the function is
called as 'fp = fopen(filename, "r");' and no such file exists, fopen will
print an error message at the console before returning the error value of 0.
If you wish to suppress this error message (e.g. your calling routine will
detect the returned 0 and issue its own message) use a two character string as
the mode, e. g. 'fp = fopen(filename, "r-");'.  The second character, which
can be anything, causes fopen to suppress its internal error message.

N.B.! For the message suppression to work, putc.c in stdlib1.c must be
modified to include the following line as the first executable statement:

	if (iobuf == 0) return(0); /* DUMP OUTPUT */

It is unfortunate that the BDSC functions supplied with the names "fopen" and
"fclose" are not equivalent to the standard K&R functions. Therefore, in order
to use the standard functions below, the BDSC-supplied functions must be
renamed: fopen becomes r_open (open for read) and fclose becomes bf_close
(close to free buffer). You must do this in your deff.crl using the rename
command of clib. Unfortunately, this means that older programs which use the
non-standard BDSC-supplied functions will have to be modified if they are to
be relinked after these name changes.

If you wish to open for appending, you should use mode "a" in fopen, and
compile and install the following function in your deff.crl:

#include "bdscio.h"

int a_open(filename,iobuf)
struct _buf *iobuf;
char *filename;
{
	if ((iobuf -> _fd = open(filename,2))<0) return ERROR;
	iobuf -> _nleft = 0;
	return iobuf -> _fd;
}

Beware: CP/M does not like names like a_open.c; use aopen.c or a-open.c
for the source file name, and then rename the crl module to a_open using
clib.
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Here is an example of how to include and use the FIO's in a source code:
		
		#include <bdscio.h>		

		#define NIOBUFS 1 /* or more, equal to the maximum number of
						files you will need to have open simultaneously;
						note that each takes about 1K of memory! */
		#include <fio1.h>

		main(argc,argv)
			int argc;
			char **argv;
			{
			FILE *fp;
			char filename[FILENAME];
		#include <fio2.h>
			/* code which gets filename goes here */
			if (badname(filename)) exit(0); /* optional */
			fp = fopen(filename, "w");
			/* code which writes to file goes here */
			fclose(fp);
		}
		#include <fio3.h>

*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

FILE *fopen(filename,mode)
	char *filename;
	char *mode;
	{
	FILE *fp;
	int i, bufat, Stderr;

#ifdef DEBUG
	if (Debug) fprintf(STDERR, "-->Opening file \"%s\" (mode %s)",
		filename, mode);
#endif

	/* FIND UNUSED BUFFER */
	for(bufat=1;bufuse[bufat] == 1;bufat++) {
		if (bufat == bufuse[0]) {
			printf(
				"\nFOPEN ERROR (%s): NO MORE BUFFERS (%d in use)\n",
				filename,bufuse[0]);
			return(0);
		}
	}
	/* ASSIGN FILE POINTER */
	fp = _iob[bufat-1]; /* FOR EXPLANATION OF -1, SEE FILE FIO.1 */

	Stderr = stderr;
	if (mode[1] > 0) Stderr = 0; /* DUMP ERROR MESSAGE */
	switch (mode[0]) {
		case 'r':
			i = r_open(filename,fp);
			if (i == ERROR) {
				fprintf(Stderr,
					"\nFOPEN ERROR: CAN'T R_OPEN \"%s\"\n",filename);
				return(0);
			}
			break;
		case 'w':
			i = fcreat(filename,fp);
			if (i == ERROR) {
				fprintf(Stderr,
					"\nFOPEN ERROR: CAN'T FCREAT \"%s\"\n",filename);
				fprintf(Stderr,
"This usually means there is no more room on the disk.\n");
				return(0);
			}
			break;
		case 'a':
			i = a_open(filename,fp);
			if (i == ERROR) {
				fprintf(Stderr,
					"\nFOPEN ERROR: CAN'T A_OPEN \"%s\"\n",filename);
				return(0);
			}
			break;
		default:
			fprintf(Stderr,
				"\nFOPEN ERROR: BAD MODE '%s' FOR '%s'\n",
				mode,filename);
			return(0);
	}
	bufuse[bufat] = 1; /* SET BUFFER USE FLAG */
	
#ifdef DEBUG
	if (Debug) {
		if (fp) fprintf(STDERR, " FD=%d\n", fp->_fd);
		else fprintf(STDERR, " UNSUCCESSFUL\n");
	}
#endif
	return(fp);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CLOSING STRATEGY:

	_fclose(fp) is the low level close which:
		1. calls bf_close (which should be named bds_close)
			which in turn flushes if(_WRITE) in _flags.
		2. deallocates fio buffer.
	
	fpfree(fp) prevents a buffer flush in case file was opened
		for append and only random writes were done.
		
	fclose(fp) appends ^Z and calls _fclose (assuming _flags
		is as desired).
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
fpfree(fp)
	FILE *fp;
	{
	/* TO AVOID FLUSHING BUFFER AFTER AN OPEN FOR APPEND
	IN BDSC VERSION 1.5 */
	fp->_flags = _READ;
	
	_fclose(fp);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
_fclose(fp)
	FILE *fp;
	{
	int bufat;

#ifdef DEBUG
	if (Debug) fprintf(STDERR, "<--Closing FD=%d\n", fp->_fd);
#endif

	bf_close(fp); /* BDS C */
	
	/* This next is required to fix a bug in CP/M. Use of putchar() can cause
	problems when using DIO. */
	bdos(2, 0); /* 2 = console out */

	for(bufat=1;fp != _iob[bufat-1];bufat++); /* FIND bufuse INDEX */
	bufuse[bufat] = 0; /* SHOW BUFFER NO LONGER IN USE */
	return(0);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
fclose(fp)
	FILE *fp;
	{
	putc('\032',fp); /* CTRL Z */

/*	fflush(fp);	NOT NECC FOR BDSC 1.5 WHICH CALLS IT IN BF_CLOSE

	THE FOLLOWING MOVED TO FPFREE FOR BDSC 1.5
	putchar('\0'); /* This is required to fix a bug in CP/M */ 
*/
	_fclose(fp);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*	END OF FIO3.H	*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

- - - - - - - - - - - - - - - - - -