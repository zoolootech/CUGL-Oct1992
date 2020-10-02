#include <stdio.h>
#include "cputim.h"

#define LOOPCNT 100000

main()
	{
	double time0, timediv, timeadd;
	long a, b = 255, c = 255, i;

	cputim();  /* time the timing harness */
	for (i = 1; i <= LOOPCNT; ++i)
		a = b;
	time0 = cputim() * (1e6 / CLOCK_TICKS_PER_SECOND);

	cputim();  /* time the divide operator */
	for (i = 1; i <= LOOPCNT; ++i)
		a = b / c;
	timediv = cputim() * (1e6 / CLOCK_TICKS_PER_SECOND);

	cputim();  /* time the addition operator */
	for (i = 1; i <= LOOPCNT; ++i)
		a = b + c;
	timeadd = cputim() * (1e6 / CLOCK_TICKS_PER_SECOND);

	printf("long divides require %.1f microseconds \n",
		(timediv - time0)/LOOPCNT);
	printf("long additions require %.1f microseconds \n",
		(timeadd - time0)/LOOPCNT);
	}
