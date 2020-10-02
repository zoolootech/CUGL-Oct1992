/* qsort - sort array a (dimension n) using quicksort
 * based on Bentley, CACM April 84
 * Comments use notation A[i], a (fictitious) array of things
 * that are of size elt_size.
 */
#include "local.h"
/* swapfn - swap  elt_size bytes  a <--> b (internal routine) 
 */
void swapfn(a, b, elt_size)
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
	}																				                                                          /*
.PE
.PS 25
/* iqsort - internal quicksort routine */
void iqsort(p_lo, p_hi, elt_size, cmpfn)
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
	}																				                                                          /*
.PE
.PS 9
/* qsort - the callable entry point */
void qsort(a, n, size, pf)
	data_ptr a;		/* address of array A to be sorted */
	size_t n;		/* number of elements in A */
	size_t size;	/* size of each element */
	int (*pf)();	/* comparison function ptr */
	{
	iqsort((data_ptr)a, (char *)a + (n-1) * size, size, pf);
	}																				                                                          /*
.PE
.PS 14
/* qsortm - test the qsort function */
static int a[100] = 
	{
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
	10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
	50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
	60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
	90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
	};																				                                                          /*
.PE
.PS 11
/* intcmp - compare two ints */
int intcmp(pi, pj)
	register int *pi, *pj;
	{
	if (*pi < *pj)
		return (-1);
	else if (*pi == *pj)
		return (0);
	else
		return (1);
	}																				                                                          /*
.PE
.PS 10
/* qsortm (main) - run the test */
main(ac, av)
	int ac;
	char *av[];
	{
	int i;

	for (i = 1; i <= 1000; ++i)
		qsort((data_ptr)a, 100, sizeof(int), intcmp);
	}
