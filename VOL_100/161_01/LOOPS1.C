#include "timer1.h"
#define LOOPDOWN(a, b) for (a = (b)+1; --a > 0; )
short i;

DO_STMT("up to 10")
	for (i = 1; i <= 10; ++i)
		;
OD

DO_STMT("down toward zero")
	for (i = 10; i > 0; --i)
		;
OD

DO_STMT("LOOPDOWN")
	LOOPDOWN(i, 10);
OD

}
