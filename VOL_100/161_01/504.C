#include "timer1.h"

DO_STMT("Pointer loop")		ptrloop()	OD
}

# define ASIZE 100

void ptrloop()
	{
	int a[ASIZE];
	register int *p;
	
	p = &a[0];
	while (p < &a[ASIZE])
		*p++ = 0;
	}
