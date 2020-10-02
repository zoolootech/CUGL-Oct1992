/* Array Handling Package
(C) Copyright 1985,1987,1988 James P. Cruse - All Rights Reserved

a_unop.h

A Part of the User-Supported Array Handling package. Please see
the file ARRAY.h for discussion concerning the registration and 
usage of the package.

declares all unary operations:

Notation:
	d[]	=>	all elements of d (from 0 to n-1)
	d[i]	=>	the i'th element of d (from 0 to n-1)
	d[i-1]	=>	the i-1'th element of d (from 1 to n-1)
	s	=>	parameter s
	f()	=>	function parameter f (i.e. cos)
	T	=>	function type T (i.e. float)

	running "sums" are d[0] = a[0], d[1] = d[0] "+" a[1], ...
		d[n-2] = d[n-3] "+" a[n-2], d[n-1] = d[n-2] "+" a[n-1]

The functions are: 
	a_cpy(n,d,a)	d[] = a[];              copy a to d
	a_neg(n,d,a)	d[] = - a[];		negate a to d
	a_abs(n,d,a)	d[] = ARR_ABS(a[])	absolute value of a to d
	a_scale(n,d,a,s,o)			scale & offset of a to d
			d[] = o + s*a[]		=> linear poly of a to d
	a_rsum(n,d,a)	d[i] = d[i-1] + a[i]	running sum of a to d
	a_rprod(n,d,a)	d[i] = d[i-1] * a[i]	running sum of a to d
	a_rmin(n,d,a)	d[i] = ARR_MIN(d[i-1],d[i])
						running minimum of a to d
	a_rmax(n,d,a)	d[i] = ARR_MAX(d[i-1],d[i])
						running maximum of a to d
	a_fun(n,d,a,f())			call function f on a to d
			d[i] = f ( a[i] )	
	a_t_fun(n,d,a,f(),T)			call function f on a to d
			d[i] = f ( (T)a[i] )	forcing a to type T

*/	

/* check to make sure array.h has been included */
#ifndef	ARR_IF_NEEDED
#include "array.h"
#endif


/* check to see if we have been loaded */
#ifndef	ARR_UNOP_LOADED
#define	ARR_UNOP_LOADED	1

/* copy */
#define	a_cpy(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] ; \
	} ARR_IF2
/* negate */
#define	a_neg(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = - (A)[ARR_IND] ;  \
	} ARR_IF2

/* absolute value */
#define	a_abs(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ARR_ABS( (A)[ARR_IND] ) ;  \
	} ARR_IF2

/* scale & offset */
#define	a_scale(N,D,A,S,O)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (O) + (S)*((A)[ARR_IND]) ; \
	} ARR_IF2

/* running sum */
#define	a_rsum(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND; (D)[0] = (A)[0]; \
	for ( ARR_IND = 1; ARR_IND < (N); ++ARR_IND ) \
		(D)[ARR_IND] = (D)[ARR_IND - 1] + (A)[ARR_IND] ;  \
	} ARR_IF2

/* running product */
#define	a_rprod(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND; (D)[0] = (A)[0]; \
	for ( ARR_IND = 1; ARR_IND < (N); ++ARR_IND ) \
		(D)[ARR_IND] = (D)[ARR_IND - 1] * (A)[ARR_IND] ;  \
	} ARR_IF2

/* running min */
#define	a_rmin(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND; (D)[0] = (A)[0]; \
	for ( ARR_IND = 1; ARR_IND < (N); ++ARR_IND ) \
		(D)[ARR_IND] = ARR_MIN( (D)[ARR_IND - 1] , (A)[ARR_IND] );  \
	} ARR_IF2

/* running max */
#define	a_rmax(N,D,A)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND; (D)[0] = (A)[0]; \
	for ( ARR_IND = 1; ARR_IND < (N); ++ARR_IND ) \
		(D)[ARR_IND] = ARR_MAX( (D)[ARR_IND - 1] , (A)[ARR_IND] );  \
	} ARR_IF2

/* unary function */
#define	a_fun(N,D,A,F)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = F( (A)[ARR_IND] );  \
	} ARR_IF2

/* typed unary function */
#define	a_t_fun(N,D,A,F,T)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = F( ((T) ((A)[ARR_IND]) ) ); \
	} ARR_IF2


#endif
