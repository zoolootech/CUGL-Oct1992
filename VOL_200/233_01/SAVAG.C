/*  VERSION 0001  (DATE: 17/05/87)  (TIME: 14:35)  */
/* Savage floating point */
#include <stdio.h>
#include <math.h>
#define		MAX_LOOP 2500

double	a;
int		gob,i;

main()
{
	a=1.0;
	printf("Type <CR> to begin %d iterations.",MAX_LOOP);
	gob=getchar();
	for(i=0;i<MAX_LOOP;i++)
		a= tan(atan(exp(log(sqrt(a*a))))) + 1.0;
	printf("\na=%20.15f\n",a);
}
