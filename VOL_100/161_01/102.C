#include "timer1.h"
	char c = 'c';
	short j = 255;
	short k = 7;

	DO_IEXPR("Bit Count V1")	bitcnt(c)	OD
	DO_IEXPR("++k")			++k  			OD
	k = 7;
	DO_IEXPR("j >> k")		j >> k  		OD
	k = 255;
	DO_IEXPR("j <= k")		j <= k  		OD
	DO_IEXPR("j & k")		j & k  			OD
}


/*	bitcnt - return the bit sum of a byte argument
 *	version 1
 */
#define CHAR_BIT 8
int bitcnt(c)
	char c;
	{
	int b;
	int i;

	b = 0;
	for (i = 0; i < CHAR_BIT; ++i)
		if ((c >> i) & 01)
			++b;
	return (b);
	}
