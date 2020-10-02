#include "timer1.h"
#define INT_CHAR (sizeof(int) * 8)
#define HI_BIT_MASK (01 << (INT_CHAR - 1))
#define HI1_BIT_MASK (01 << (INT_CHAR - 2))
#define HI3_BIT_MASK (07 << (INT_CHAR - 5))
#define PAD (INT_CHAR - 10)
struct fields
	{
	unsigned f1 : 1;
	unsigned f2 : 1;
	unsigned f3 : 3;
	unsigned fpad : PAD;
	unsigned f4 : 3;
	unsigned f5 : 1;
	unsigned f6 : 1;
	};
struct fields sf;
register int reg;
unsigned ui, ui2;

DO_STMT("turn on sf.f1")
sf.f1 = 1;
OD

DO_STMT("turn on sf.f2")
sf.f2 = 1;
OD

DO_STMT("turn on sf.f5")
sf.f5 = 1;
OD

DO_STMT("turn on sf.f6")
sf.f6 = 1;
OD

DO_STMT("turn on ui hi bit")
ui |= HI_BIT_MASK;
OD

DO_STMT("turn on ui hi-1 bit")
ui |= HI1_BIT_MASK;
OD

DO_STMT("turn on ui lo+1 bit")
ui |= 02;
OD

DO_STMT("turn on ui lo bit")
ui |= 01;
OD

sf.f1 = sf.f2 = sf.f3 = sf.f4 = sf.f5 = sf.f6 = ui = 0;
DO_STMT("test sf.f1")
if (sf.f1 != 0) ++reg;
OD

DO_STMT("test sf.f2")
if (sf.f2 != 0) ++reg;
OD

DO_STMT("test sf.f5")
if (sf.f5 != 0) ++reg;
OD

DO_STMT("test sf.f6")
if (sf.f6 != 0) ++reg;
OD

DO_STMT("test ui hi bit")
if ((ui & HI_BIT_MASK) != 0) ++reg;
OD

DO_STMT("test ui hi-1 bit")
if ((ui & HI1_BIT_MASK) != 0) ++reg;
OD

DO_STMT("test ui lo+1 bit")
if ((ui & 02) != 0) ++reg;
OD

DO_STMT("test ui lo bit")
if ((ui & 01) != 0) ++reg;
OD

DO_STMT("extract field f3")
ui2 = sf.f3;
OD

DO_STMT("extract field f4")
ui2 = sf.f4;
OD

DO_STMT("mask & shift mid1")
ui2 = ui & 034 >> 2;
OD

DO_STMT("mask & shift mid2")
ui2 = ui & HI3_BIT_MASK >> (INT_CHAR - 5);
OD

DO_STMT("shift mid1 & mask")
ui2 = (ui >> 2) & 07;
OD

DO_STMT("shift mid2 & mask")
ui2 = (ui >> (INT_CHAR - 5)) & 07;
OD

}
