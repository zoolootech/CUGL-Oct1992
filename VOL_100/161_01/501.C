#include "timer1.h"

# define ASIZE 100

int a[ASIZE], i;

DO_STMT("Array indexing loop")
	for (i = 0; i < ASIZE; i++)
		a[i] = 0;

OD

}
