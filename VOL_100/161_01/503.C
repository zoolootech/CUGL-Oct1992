#include "timer1.h"

# define ASIZE 100
	
int a[ASIZE];
register int *p;

DO_STMT("Pointer loop")
	for (p = &a[0]; p < &a[ASIZE]; p++)
		*p = 0;

OD

}
