#include "timer1.h"

#define ASIZE 100

int a[ASIZE];
register int i;
	
DO_STMT("Register indexed looping")
	for (i = 0; i < ASIZE; i++)
		a[i] = 0;
OD
}
