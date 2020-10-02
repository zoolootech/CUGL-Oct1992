/* This is a function to open for appending. It is identical to r_open (BDSC
fopen) except for the mode in the call to "open". E. Martz */

#include "bdscio.h"

int a_open(filename,iobuf)
struct _buf *iobuf;
char *filename;
{
	if ((iobuf -> _fd = open(filename,2))<0) return ERROR;
	iobuf -> _nleft = 0;

	/* added for bdsc 1.5 to force write when fflush() is called */
	iobuf -> _flags = _WRITE + _READ;

	return iobuf -> _fd;
}
				return w;Š	return ERROR;Š}ŠŠŠint fflush(iobuf)Šstruct _buf *