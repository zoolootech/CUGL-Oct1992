/* qsortm - test the qsort function */
#include "local.h"
#include "cputim.h"
static long n_compares = 0;
static int a[10000] = {0};
/* intcmp - compare two ints */
int intcmp(pi, pj)
	register int *pi, *pj;
	{
	++n_compares;
	if (*pi < *pj)
		return (-1);
	else if (*pi == *pj)
		return (0);
	else
		return (1);
	}																				                                                  /*
.PE
.PS 32
/* qsortm (main) - run the test */
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	double nlogn;
	double tim;	/* in usec */
	cputim_t cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
	printf("start:\n");
	cputim();
	qsort((data_ptr)a, lim, sizeof(int), intcmp);
	tim = 1e6 * (double)cputim() / CLOCK_TICKS_PER_SECOND;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	nlogn = lim * log((double)lim) / log(2.);
	printf("n_compares = %ld\n", n_compares);
	printf("cpu time = %.3f (usec) per compare\n", tim / n_compares);
	printf("log N = %.3f\n", log((double)lim) / log(2.));
	printf("N log N = %.3f\n", nlogn);
	printf("cpu time = %.2f N log N (usec)\n", tim / nlogn);
	printf("ratio of n_compares to N log N = %.3f\n\n", n_compares / nlogn);
	for (i = 0; i < lim-1; ++i)
		if (a[i] > a[i+1])
			error("bad value", "");
	exit(0);
	}
