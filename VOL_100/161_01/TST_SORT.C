/* tst_sort - returns YES if array a is sorted
 * specialized "short" version
 */
#include "local.h"
bool tst_sort(a, n)
	short a[];
	index_t n;
	{
	index_t i;

	if (n <= 0)
		return (NO);
	for (i = 1; i < n; ++i)
		{
		/* a[0:i-1] => sorted */
		if (a[i] < a[i-1])		/* compare adjacent elements */
			return (NO);
		}
	/* a[0:n-1] => sorted */
	return (YES);
	}
