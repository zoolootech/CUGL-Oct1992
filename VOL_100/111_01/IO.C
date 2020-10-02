/*
HEADER:		;
TITLE:		Squeezer;
DESCRIPTION:	"Auxiliary file for the SQ.C and USQ.C package.";
SYSTEM:		CP/M-80;
FILENAME:	IO.C;
AUTHORS:	Dick Greenlaw;
COMPILERS:	BDS C;
*/
#include <bdscio.h>
#include <dio.h>
#include "sqcom.h"
#include "sq.h"
#define STDOUT	 4	/* console only (error) stream */

/* Get next byte from file and update checksum */

int
getc_crc(ib)
struct _buf *ib;
{
	int c;

	c = getc(ib);
	if(c != EOF)
		crc += c;	/* checksum */
	return c;
}

/* Output functions with error reporting */

putce(c, iob)
int c;
struct _buf *iob;
{
	if(putc(c, iob) == ERROR) {
		fprintf(STDOUT, "Write error in putc()\n");
		exit(1);
	}
}

putwe(w, iob)
int w;
struct _buf *iob;
{
	if(putw(w, iob) != w) {
		fprintf(STDOUT,"Write error in putw()\n");
		exit(1);
	}
}
