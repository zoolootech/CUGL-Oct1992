/* boolbits - time two different versions of bool macros */
#include "timer1.h"
	static char a[5] = {0};
	short i = 4;

#define SETTRUE(barr, index)  (barr[(index)>>3] |= (1 << ((index) & 0x7)))
#define SETFALSE(barr, index) (barr[(index)>>3] &= ~(1 << ((index) & 0x7)))
#define TEST(barr, index) ((barr[(index)>>3] & (1 << ((index) & 0x7))) != 0)

	DO_IEXPR("SETTRUE-packed")	SETTRUE(a, i)	OD
	DO_IEXPR("SETFALSE-packed")	SETFALSE(a, i)	OD
	DO_IEXPR("TEST-packed")	TEST(a, i)	OD

#undef SETTRUE
#undef SETFALSE
#undef TEST
#define SETTRUE(barr, index)  (barr[index] = 1)
#define SETFALSE(barr, index) (barr[index] = 0)
#define TEST(barr, index) (barr[index])

	DO_IEXPR("SETTRUE-unpacked")	SETTRUE(a, i)	OD
	DO_IEXPR("SETFALSE-unpacked")	SETFALSE(a, i)	OD
	DO_IEXPR("TEST-unpacked")	TEST(a, i)	OD

	}
