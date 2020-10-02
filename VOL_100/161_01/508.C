#include "timer1.h"

	long value = 1<<25 - 1;
	char buf[30];
	DO_STMT("Long to ASCII using MAXINT")	ltoa(value, buf)	OD
}
	
/* largest (normal length) positive integer */
# define MAXINT 32767

/*
 *	convert a positive long integer into
 *	a sequence of ASCII digits (decimal)
 */

ltoa(value, buf)
long value;
char *buf;
{
	int svalue;

	while (value > MAXINT) {
		*buf++ = (int)(value % 10) + '0';
		value /= 10;
	}
	svalue = value;
	while (svalue > 0) {
		*buf++ = svalue % 10 + '0';
		svalue /= 10;
	}
}
