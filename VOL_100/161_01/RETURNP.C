/* returnp - reveal the return pointer of my calling function */
#include "config.h"
int returnp(pa)
	int *pa;
	{
	return (pa[RETOFFSET]);
	}
