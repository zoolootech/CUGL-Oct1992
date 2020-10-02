#include "timer1.h"
char c = 'c';
DO_IEXPR("Bit Count V2")	bitcnt(c)	OD
}

/*	bitcnt - return the bit sum of a byte argument
 *	version 2
 */
#define BYTEMASK 0377
int bitcnt(c)
	char c;
	{
	int b;
	unsigned uc;

	uc = c & BYTEMASK;
	b = 0;
	while (uc != 0)
		{
		if (uc & 01)
			++b;
		uc >>= 1;
		}
	return (b);
	}
