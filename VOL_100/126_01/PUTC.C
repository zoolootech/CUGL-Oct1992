/* BDSC putc from stdlib1.c; modified for iobuf == 0 or 4 */

#include "bdscio.h"

int putc(c,iobuf)
char c;
struct _buf *iobuf;
{
	if (iobuf == 0) return(0); /* DUMP OUTPUT */
	if (iobuf == 1) return putchar(c);
	if (iobuf == 2) return (bdos(5,c));
	if (iobuf == 3) return (bdos(4,c));
	if (iobuf == 4) {
		if (c == '\n') bdos(2,'\r');
		return bdos(2,c);
	 }	
	if (iobuf -> _nleft--) return *iobuf -> _nextp++ = c;
	if ((write(iobuf -> _fd, iobuf -> _buff, NSECTS)) != NSECTS)
			return ERROR;
	iobuf -> _nleft = (NSECTS * SECSIZ - 1);
	iobuf -> _nextp = iobuf -> _buff;
	return *iobuf -> _nextp++ = c;
}
*********