#include "timer1.h"
char c = 'c';
DO_IEXPR("Bit Count V3")	bitcnt(c)	OD
}

/*	bitcnt - return the bit sum of a byte argument
 *	version 3
 */
#define BYTEMASK 0377
int bitcnt(c)
	register char c;
	{
	register int b;
	register unsigned uc;

	uc = c & BYTEMASK;
	b = 0;
	while (uc != 0) 
		{
		if (uc & 01)
			b++;
		uc >>= 1;
		}
	return (b);
	}
