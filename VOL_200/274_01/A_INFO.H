/* Array Handling Package
(C) Copyright 1985,1987,1988 James P. Cruse - All Rights Reserved

a_info.h  

A Part of the User-Supported Array Handling package. Please see
the file ARRAY.h for discussion concerning the registration and 
usage of the package.

declares all info operations:

Notation:
	d	=>	destination parameter d
	a[+]	=>	all elements of a[] are [+]
	d[]	=>	all elements of d (from 0 to n-1)
	d[i]	=>	the i'th element of d (from 0 to n-1)
	d[i-1]	=>	the i-1'th element of d (from 1 to n-1)
	s	=>	parameter s
	f()	=>	function parameter f (i.e. cos)
	T	=>	function type T (i.e. float)

The functions are: 
	a_sum(n,d,a)	d = a[+]		sum of array
	a_prod(n,d,a)	d = a[*]		product of array
	a_max(n,d,a)	d = max(a[])		max value in array
	a_min(n,d,a)	d = min(a[])		min value in array
	a_minmax(n,l,u,a)			min and max of array
			l = min(a[]), u = max(a[])

*/	

/* check to make sure array.h has been included */
#ifndef	ARR_IF_NEEDED
#include "array.h"
#endif


/* check to see if we have been loaded */
#ifndef	ARR_INFO_LOADED
#define	ARR_INFO_LOADED	1

/* sum */
#define	a_sum(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	(D) = (A)[--ARR_IND]; \
	while ( ARR_IND-- ) \
		(D) += (A)[ARR_IND]; \
	} ARR_IF2

/* product */
#define	a_prod(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	(D) = (A)[--ARR_IND]; \
	while ( ARR_IND-- ) \
		(D) *= (A)[ARR_IND]; \
	} ARR_IF2

/* min */
#define	a_min(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	(D) = (A)[--ARR_IND]; \
	while ( ARR_IND-- ) \
		if ( (D) > (A)[ARR_IND] ) \
			(D) = (A)[ARR_IND]; \
	} ARR_IF2

/* max */
#define	a_max(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	(D) = (A)[--ARR_IND]; \
	while ( ARR_IND-- ) \
		if ( (D) < (A)[ARR_IND] ) \
			(D) = (A)[ARR_IND]; \
	} ARR_IF2

/* minmax */
#define	a_minmax(N,L,U,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	(L) = (U) = (A)[--ARR_IND]; \
	while ( ARR_IND-- ) \
		if ( (L) > (A)[ARR_IND] ) \
			(L) = (A)[ARR_IND]; \
		else if ( (U) < (A)[ARR_IND] ) \
			(U) = (A)[ARR_IND]; \
	} ARR_IF2


#endif	/* ARR_INFO_LOADED */
