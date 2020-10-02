
/*
 *	return the bit sum of a byte argument
 *	version 6
 */

# include "timer1.h"
# define bitcnt(x) (bitsum[(x) & BYTEMASK])
# define BYTEMASK 0377

char c = 15;

static char bitsum[256] = {
	'\1', '\0', '\1', '\1', '\2', '\1', '\0', '\1', '\1', '\2', '\1'
};

DO_IEXPR("Bit count V6") bitcnt(c)	OD
}
