#include "timer1.h"

	long value = 1<<25 - 1;
	char buf[30];

	DO_IEXPR("Convert Long to ASCII")	ltoa(value, buf)	OD
}

/*
 *	convert a positive long integer into
 *	a sequence of ASCII digits (decimal)
 */

ltoa(value, buf)
long value;
char *buf;
{
	while (value > 0) {
		*buf++ = (int)(value % 10) + '0';
		value /= 10;
	}
}
