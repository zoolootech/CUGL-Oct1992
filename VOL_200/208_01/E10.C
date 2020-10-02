/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e10.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*        FUNCTIONS: efopen,efcreat,efflush,efclose,eputc,egetc
	PURPOSE:   buffered disk IO
*/

#include "e.h"

extern char *rindex();

efopen(s,iobuf)
struct iobuffer *iobuf;
char *s;
{
#if CPM68K
char *p;
char temp[FILELEN];
	strcpy(temp, s);
	if( p = rindex(temp, ':') ) {
		*p = 0;
		if(chdir(temp)== FAIL)
			return(FAIL);
		chdir(curdir);
	}
#endif
	iobuf->_nleft = 0;
        return (iobuf->_fd = open(s,0));
}


efcreat(s,iobuf)
char *s;
struct iobuffer *iobuf;
{
#if CPM68K
char *p;
char temp[FILELEN];
	strcpy(temp, s);
	if( p = rindex(temp, ':') ) {
		*p = 0;
		if(chdir(temp)== FAIL)
			return(FAIL);
		chdir(curdir);
	}
#endif
        if ((iobuf -> _fd = creat(s,0666)) < 0 )
                return(FAIL);
        iobuf -> _nextp = iobuf -> _buff;
        iobuf -> _nleft = (NSECTS * SECSIZ);
        return(iobuf -> _fd);
}

efflush(iobuf)       /*does NOT allow more writing*/

struct iobuffer *iobuf;
{
        int i;

        if (iobuf -> _nleft == (NSECTS * SECSIZ)) return(YES);
        i = NSECTS*SECSIZ - iobuf->_nleft;
        if (write(iobuf -> _fd, iobuf -> _buff, i) != i) return(FAIL);
        return(YES);
}

efclose(iobuf)
struct iobuffer *iobuf;
{
register int f;
	/* close opened file */
	return ((f= iobuf->_fd)? close(f) : -1 );
}

eputc(c,iobuf)  /*stripped down version of standard putc*/
char c;
struct iobuffer *iobuf;
{
        if (iobuf -> _nleft--) return(*iobuf -> _nextp++ = c);
    if ((write(iobuf -> _fd, iobuf -> _buff, NSECTS*SECSIZ)) != NSECTS*SECSIZ)
                return(FAIL);
        iobuf -> _nleft = (NSECTS * SECSIZ -1);
        iobuf -> _nextp = iobuf -> _buff;
        return(*iobuf -> _nextp++ = c);
}

egetc(iobuf)        /*the standard getc, trimmed for speed*/
struct iobuffer  *iobuf;
{
        int nbytes;

        if (iobuf -> _nleft--) return(*iobuf -> _nextp++);
        if ((nbytes = read(iobuf -> _fd, iobuf -> _buff, NSECTS*SECSIZ))==FAIL) {
                iobuf -> _nleft++;
                return(EOF);
                }
        if (nbytes == 0) {
                iobuf -> _nleft++;
                return(EOF);
                }
        iobuf -> _nleft = nbytes - 1;
        iobuf -> _nextp = iobuf -> _buff;
        return(*iobuf->_nextp++);
}
