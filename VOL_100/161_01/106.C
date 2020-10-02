#include "timer1.h"
char c = 'c';
DO_IEXPR("Bit Count V4")	bitcnt(c)	OD
}


/*	bitcnt - return the bit sum of a byte argument
 *	version 4
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
		b++;
		uc &= uc - 1;
		}
	return b;
	}
