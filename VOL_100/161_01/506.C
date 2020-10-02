#include "timer1.h"

DO_IEXPR("Ptr looping 2 dimensional")	ptrloop()	OD
}

# define NROW 10
# define NCOLUMN 10
	
void ptrloop()
	{
	int a[NROW][NCOLUMN];
	register int *p;
	
	p = &a[0][0];
	while (p < &a[NROW][0])
		*p++ = 0;
	}
