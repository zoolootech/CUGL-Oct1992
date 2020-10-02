/* qsort - sort array a (dimension n) using quicksort
 * based on Bentley, CACM April 84
 * Iterative version; avoids recursion, uses little stack
 * Comments use notation A[i], a (fictitious) array of things
 * that are of size elt_size.
 */
#include "local.h"

#define STACK_DEPTH (sizeof(size_t) * CHAR_BIT)	/* log2(sizeof(A)) */

static size_t elt_size = 0;		/* size of one element */
static int (*cmpfn)() = NULL;	/* the comparison function ptr */
/* swapfn - swap  elt_size bytes  a <--> b (internal routine) 
 */
static void swapfn(a, b)
	register char *a;	/* pointer to one element of A */
	register char *b;	/* pointer to another element of A */
	{
	register size_t i;
	char tmp;

	LOOPDN(i, elt_size)
		{
		SWAP(*a, *b, tmp);
		++a, ++b;
		}
	}
/* sort1 - partition one (sub)array, returning p_lastlo */
static char *sort1(p_lo, p_hi)
	char *p_lo;	/* ptr to low element of (sub)array */
	char *p_hi;	/* ptr to high element of (sub)array */
	{
	char *p_mid;
	register char *p_i;	/* pointer to A[i] */
	register char *p_lastlo;	/* pointer to A[lastlo] */

	p_mid = p_lo + ((((p_hi - p_lo) / elt_size) / 2) * elt_size);
	swapfn(p_lo, p_mid);	/* pick the middle element as pivot */
	p_lastlo = p_lo;
	for (p_i = p_lo + elt_size;  p_i <= p_hi; p_i += elt_size)
		{
		if ((*cmpfn)(p_lo, p_i) > 0)
			{
			p_lastlo += elt_size;
			swapfn(p_lastlo, p_i);
			}
		}
	swapfn(p_lo, p_lastlo);
	return (p_lastlo);
	}
/* (new page)
.PE
.PS 45
 */
/* qsort - the callable entry point */
void qsort(a, n, size, pf)
	data_ptr a;	/* address of array A to be sorted */
	size_t n;	/* number of elements in A */
	size_t size;	/* size of each element */
	int (*pf)();	/* comparison function ptr */
	{
	static char *histack[STACK_DEPTH] = {0};
	static char *lostack[STACK_DEPTH] = {0};
	int istack;	/* index of next free stack cell */
	char *p_lo;	/* ptr to A[lo] */
	char *p_hi;	/* ptr to A[hi] */
	char *p_lastlo;	/* ptr to A[lastlo] */
	char *p_lo1, *p_hi1;	/* partition 1 */
	char *p_lo2, *p_hi2;	/* partition 2 */
	char *tpc;	/* tmp ptr for swaps */

	elt_size = size;
	cmpfn = pf;
	istack = 0;
	p_lo = a;
	p_hi = (char *)a + (n-1) * elt_size;
	/* (new page)
.PE
.PS 45
	 */
	/* loop until no non-trivial partitions remain */
	while (istack != 0 || p_lo < p_hi)
		{
		p_lastlo = sort1(p_lo, p_hi);
		p_lo1 = p_lo;
		p_hi1 = p_lastlo - elt_size;
		p_lo2 = p_lastlo + elt_size;
		p_hi2 = p_hi;
		if (p_hi1 - p_lo1 > p_hi2 - p_lo2)
			{
			SWAP(p_lo1, p_lo2, tpc);
			SWAP(p_hi1, p_hi2, tpc);
			}
		/* now [p_lo1,p_hi1] is smaller partition */
		if (p_lo1 >= p_hi1)
			{
			if (p_lo2 < p_hi2)
				{
				/* do next iteration on [p_lo2, p_hi2] */
				p_lo = p_lo2;
				p_hi = p_hi2;
				}
			else if (istack > 0)
				{
				/* take next iteration from stack */
				--istack;
				p_hi = histack[istack];
				p_lo = lostack[istack];
				}
			else
				p_hi = p_lo;	/* done */
			}
		else
			{
			/* push [p_lo2, p_hi2] on stack */
			histack[istack] = p_hi2;
			lostack[istack] = p_lo2;
			++istack;
			/* take next iteration from [p_lo1, p_hi1] */
			p_lo = p_lo1;
			p_hi = p_hi1;
			}
		}
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
