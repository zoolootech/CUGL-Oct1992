/* qsort - sort array a (dimension n) using quicksort
 * based on Bentley, CACM April 84
 * Comments use notation A[i], a (fictitious) array of things
 * that are of size elt_size.
 */
#include "local.h"
/* swapfn - swap  elt_size bytes  a <--> b (internal routine) 
 */
static void swapfn(a, b, elt_size)
	register char *a;	/* pointer to one element of A */
	register char *b;	/* pointer to another element of A */
	size_t elt_size;	/* size of each element */
	{
	register size_t i;
	char tmp;

	LOOPDN(i, elt_size)
		{
		SWAP(*a, *b, tmp);
		++a, ++b;
		}
	}																		/*
.PE
.PS 25
/* iqsort - internal quicksort routine */
static void iqsort(p_lo, p_hi, elt_size, cmpfn)
	char *p_lo;			/* ptr to low element of (sub)array */
	char *p_hi;			/* ptr to high element of (sub)array */
	size_t elt_size;	/* size of each element */
	int (*cmpfn)();		/* comparison function ptr */
	{
	char *p_mid;				/* pointer to middle element */
	register char *p_i;			/* pointer to A[i] */
	register char *p_lastlo;	/* pointer to A[lastlo] */

	if (p_hi <= p_lo)			/* is partition trivial? */
		return;
	p_mid = p_lo + ((((p_hi - p_lo) / elt_size) / 2) * elt_size);
	swapfn(p_lo, p_mid, elt_size);	/* pick the middle element as pivot */
	p_lastlo = p_lo;
	for (p_i = p_lo + elt_size;  p_i <= p_hi; p_i += elt_size)
		{
		if ((*cmpfn)(p_lo, p_i) > 0)
			{
			p_lastlo += elt_size;
			swapfn(p_lastlo, p_i, elt_size);
			}
		}
	swapfn(p_lo, p_lastlo, elt_size);
	iqsort(p_lo, p_lastlo - elt_size, elt_size, cmpfn);
	iqsort(p_lastlo + elt_size, p_hi, elt_size, cmpfn);
	}																		/*
.PE
.PS 9
/* qsort - the callable entry point */
void qsort(a, n, size, pf)
	data_ptr a;		/* address of array A to be sorted */
	size_t n;		/* number of elements in A */
	size_t size;	/* size of each element */
	int (*pf)();	/* comparison function ptr */
	{
	iqsort((char *)a, (char *)a + (n-1) * size, size, pf);
	}
/* qsortm - test the qsort function
 */
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
	}
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
	exit(SUCCEED);
	}
