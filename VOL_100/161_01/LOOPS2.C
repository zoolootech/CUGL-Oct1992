#include "timer1.h"
#define LOOPDOWN(a, b) for (a = (b)+1; --a > 0; )
register int i;

DO_STMT("register up to 10")
	for (i = 1; i <= 10; ++i)
		;
OD

DO_STMT("register toward zero")
	for (i = 10; i > 0; --i)
		;
OD

DO_STMT("register LOOPDOWN")
	LOOPDOWN(i, 10);
OD

}
