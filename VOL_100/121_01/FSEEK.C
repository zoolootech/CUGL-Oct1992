/*
HEADER: CUG 121.??;

    TITLE:	fseek - routines for more.c;
    VERSION:	1.0;
    DATE:	08/01/85;
    DESCRIPTION: "This module provides two functions: (1) fseek - seek to
		a character position in file, and (2) ftell - says where
		in file you are.  These provide Unix-like facilities for
		character seek & positional query to BDS C programs.";
    KEYWORDS:	buffered, seek;
    SYSTEM:	CP/M;
    FILENAME:	FSEEK.C;
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/
/*
 * fseek.c - subroutines to provide Unix-like facilities for BDS C
 *
 *	Description
 *	  This module provides two functions:
 *		fseek - seek to character position in file
 *		ftell - says where in file you are
 *
 *
 * fseek - a character seek for the BDS C buffered I/O routines
 *
 *	  This routine is used by more.c.  It provides a Unix-like character
 *	seek for BDS C.  Arguments -
 *		f - the I/O Buffer pointer
 *		tosec - an int saying where we want to be
 *		code - Tells how to interpret tosec:
 *			code = 0) absolute, characters
 *			code = 1) relative, characters
 *			code = 2) * from end, characters
 *			code = 3) absolute, sectors
 *			code = 4) relative, sectors
 *			code = 5) * from end, sectors
 *	* - not implemented, but there to model the
 *		Unix V6 seek call.
 *	mwm 2/81
 */

#include <bdscio.h>

fseek(f, tosec, code)
struct _buf *f;
{
int bchar, cchar, tochar;
unsigned bsec, csec, wsec;
/*
 * The variable names -
 *	bX is the buffer length, in X's.
 *	Xchar is a character offset in a sector.
 *	Xsec is a sector offset in the file.
 *	toX is where we want to go to.
 *	wsec is the next sector to be read.
 *	cX is where we are.
 */
	/* extract some info about the buffer */
	bsec = (bchar = f -> _nextp - f -> _buff + f -> _nleft) / SECSIZ;

	/* now, find out where we are */
	if ((wsec = tell(f -> _fd)) == ERROR)
		return(ERROR);
	cchar = bchar - f -> _nleft;
	csec = wsec - bsec + cchar / SECSIZ;
	cchar %= SECSIZ;

	/* Next, where do we want to be */
	if (code > 2) {	/* by sectors, the easy part */
		tochar = cchar;
		if (code == 4)
			/* don't seek past 0 */
			if (tosec < 0 && abs(tosec) > csec)
				tochar = tosec = 0;
			else tosec += csec;
		else if (code != 3)
			return(ERROR);
	}
	else {		/* by characters.  Hard */
		tochar = tosec % SECSIZ;
		tosec /= SECSIZ;
		if (code == 1) {
			tochar += cchar;
			tosec += tochar / SECSIZ;
			tochar %= SECSIZ;
			if (tosec < 0 && abs(tosec) > csec)
				tochar = tosec = 0;
			else tosec += csec;
		}
		else if (code != 0)
			return(ERROR);
	}

	/* Now go there! */
	if (tosec < wsec && tosec > wsec - bsec) {
		/* in the buffer, just rearrange things */
		tochar += (bsec - (wsec - tosec)) * SECSIZ;
    		f -> _nleft = bchar - tochar;
		f -> _nextp = f -> _buff + tochar;
		return(OK);
	}
		/* Not in the buffer, so set the I/O pointer,
		   and then rearrange the buffer */
	if (seek(f -> _fd, tosec, 0) == ERROR)
		return(ERROR);
	f -> _nleft = 0;
	getc(f);	/* force the buffer to be filled */
	f -> _nextp = f -> _buff + tochar;
	f -> _nleft++;
	f -> _nleft -= tochar;
	return(OK);
}
/*
 * ftell - a tell for BDS C buffered I/O
 *
 *	Argument -
 *		file - the I/O Buffer pointer
 */
unsigned ftell(file)
struct _buf *file;
{
unsigned wsec, bsec, csec;

	if ((wsec = tell(file -> _fd)) == ERROR)
		return(ERROR);
	return(SECSIZ * wsec - file -> _nleft);
}
                                                                                                                          