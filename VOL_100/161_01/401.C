#include "timer1.h"
char c = 15;
DO_IEXPR("Bit Count V5")	bitcnt(c)	OD
}

/*
 *	precomputed bit sum values
 */

#define BYTEMASK 0377

char bitsum[256] = {
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1, 
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1,
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1,
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1,
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1,
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1,
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1,
	1, 0, 1, 1, 2, 1, 0, 1, 1, 2, 1
	/* note - phoney values just for timing */
};

/*
 *	return the bit sum of a byte argument
 *	version 5
 */

int bitcnt(c)
char c;
{
	extern char bitsum[256];

	return (bitsum[c & BYTEMASK]);
}
