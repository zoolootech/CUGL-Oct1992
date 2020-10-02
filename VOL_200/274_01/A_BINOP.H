/* Array Handling Package
(C) Copyright 1985,1987,1988 James P. Cruse - All Rights Reserved

a_binop.h 

A Part of the User-Supported Array Handling package. Please see
the file ARRAY.h for discussion concerning the registration and 
usage of the package.

declares all binary operations:

Notation:
	d[]	=>	all elements of d (from 0 to n-1)
	d[i]	=>	the i'th element of d (from 0 to n-1)
	d[i-1]	=>	the i-1'th element of d (from 1 to n-1)
	s	=>	parameter s
	f()	=>	function parameter f (i.e. cos)
	T	=>	function type T (i.e. float)

	running "sums" are d[0] = a[0], d[1] = d[0] "+" a[1], ...
		d[n-2] = d[n-3] "+" a[n-2], d[n-1] = d[n-2] "+" a[n-1]


Note:
	for each operation, there are 3 macros, for each combination
		of arrays and constants, for example:

	aa_op(n,d,a,b) 	array, array operation		(d[] = a[] + b[])
	ac_op(n,d,a,c)	array, constant operation 	(d[] = a[] * c)
	ca_op(n,d,c,a)	constant, array operation	(d[] = c - a[])

The functions are: 
	aa_add(n,d,a,b)	d[] = a[] + b[]		add a[] and b[]
	ac_add(n,d,a,c)	d[] = a[] + c  		add a[] and c  
	ca_add(n,d,c,b)	d[] = c   + b[]		add c and b[]

	aa_sub(n,d,a,b)	d[] = a[] - b[]		sub b[] from a[]
	ac_sub(n,d,a,c)	d[] = a[] - c  		sub c from b[]
	ca_sub(n,d,c,b)	d[] = c   - b[]		sub b[] from c

	aa_mul(n,d,a,b)	d[] = a[] * b[]		multiply a[] and b[]
	ac_mul(n,d,a,c)	d[] = a[] * c  		multiply a[] and c
	ca_mul(n,d,c,b)	d[] = c   * b[]		multiply c and b[]

	aa_div(n,d,a,b)	d[] = a[] / b[]		divide a[] by b[]
	ac_div(n,d,a,c)	d[] = a[] / c  		divide a[] by c
	ca_div(n,d,c,b)	d[] = c   / b[]		divide c by a[]

	aa_mod(n,d,a,b)	d[] = a[] % b[]		Modulo a[] by b[]
	ac_mod(n,d,a,c)	d[] = a[] % c  		Modulo a[] by c
	ca_mod(n,d,c,b)	d[] = c   % b[]		Modulo c by a[]

	aa_max(n,d,a,b)	d[] = ARR_MAX(a[],b[])	max of a[] and b[]
	ac_max(n,d,a,c)	d[] = ARR_MAX(a[],c)	max of a[] and c
	ca_max(n,d,c,b)	d[] = ARR_MAX(c,b[])	max of c and b[]

	aa_min(n,d,a,b)	d[] = ARR_MIN(a[],b[])	min of a[] and b[]
	ac_min(n,d,a,c)	d[] = ARR_MIN(a[],c)	min of a[] and c
	ca_min(n,d,c,b)	d[] = ARR_MIN(c,b[])	min of c and b[]

	aa_equ(n,d,a,b)	d[] = a[] == b[]	a[] equal b[]
	ac_equ(n,d,a,c)	d[] = a[] == c  	a[] equal c
	ca_equ(n,d,c,b)	d[] = c   == b[]	c equal b[]

	aa_geq(n,d,a,b)	d[] = a[] >= b[]	a[] greater than or equal b[]
	ac_geq(n,d,a,c)	d[] = a[] >= c  	a[] greater than or equal c
	ca_geq(n,d,c,b)	d[] = c   >= b[]	c   greater than or equal b[]

	aa_gtr(n,d,a,b)	d[] = a[] >  b[]	a[] greater than b[]
	ac_gtr(n,d,a,c)	d[] = a[] >  c  	a[] greater than c
	ca_gtr(n,d,c,b)	d[] = c   >  b[]	c   greater than b[]

	aa_leq(n,d,a,b)	d[] = a[] <= b[]	a[] less than or equal b[]
	ac_leq(n,d,a,c)	d[] = a[] <= c  	a[] less than or equal c
	ca_leq(n,d,c,b)	d[] = c   <= b[]	c   less than or equal b[]

	aa_les(n,d,a,b)	d[] = a[] <  b[]	a[] less than b[]
	ac_les(n,d,a,c)	d[] = a[] <  c  	a[] less than c
	ca_les(n,d,c,b)	d[] = c   <  b[]	c   less than b[]

	aa_fun(n,d,a,b,f())			call f on a[] and b[]
			d[] = f( a[] , b[] )
	ac_fun(n,d,a,c,f())			call f on a[] and c
			d[] = f( a[] , c )
	ca_fun(n,d,c,b,f())			call f on c and a[]
			d[] = f( c , a[] )

	aa_t_fun(n,d,a,b,f(),T)			call f on a[] and b[]
			d[] = f((T)a[],(T)b[])	forcing a[i],b[i] to type T
	ac_t_fun(n,d,a,c,f(),T)			call f on a[] and c
			d[] = f( (T)a[], (T)c )	forcing a[i],c to type T
	ca_t_fun(n,d,c,b,f(),T)			call f on c and a[]
			d[] = f( (T)c, (T)a[] )	forcing c,a[i] to type T


*/	

/* check to make sure array.h has been included */
#ifndef	ARR_IF_NEEDED
#include "array.h"
#endif


/* check to see if we have been loaded */
#ifndef	ARR_BINOP_LOADED
#define	ARR_BINOP_LOADED	1

/* addition */
#define	aa_add(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] + (B)[ARR_IND]; \
	} ARR_IF2
#define	ac_add(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] + (B); \
	} ARR_IF2
#define ca_add(N,D,A,B)	ac_add(N,D,B,A)		/* same operation */

/* subtraction */
#define	aa_sub(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] - (B)[ARR_IND]; \
	} ARR_IF2
#define	ac_sub(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] - (B); \
	} ARR_IF2
#define	ca_sub(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A) - (B)[ARR_IND]; \
	} ARR_IF2

/* Multiplication */
#define	aa_mul(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] * (B)[ARR_IND]; \
	} ARR_IF2
#define	ac_mul(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] * (B); \
	} ARR_IF2
#define ca_mul(N,D,A,B)	ac_mul(N,D,B,A)		/* same operation */

/* division */
#define	aa_div(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] / (B)[ARR_IND]; \
	} ARR_IF2
#define	ac_div(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] / (B); \
	} ARR_IF2
#define	ca_div(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A) / (B)[ARR_IND]; \
	} ARR_IF2

/* minimum */
#define	aa_min(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ARR_MIN( (A)[ARR_IND] , (B)[ARR_IND] ); \
	} ARR_IF2
#define	ac_min(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ARR_MIN( (A)[ARR_IND] , (B) ); \
	} ARR_IF2
#define ca_min(N,D,A,B)	ac_min(N,D,B,A)		/* same operation */

/* maximum */
#define	aa_max(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ARR_MAX( (A)[ARR_IND] , (B)[ARR_IND] ); \
	} ARR_IF2
#define	ac_max(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ARR_MAX( (A)[ARR_IND] , (B) ); \
	} ARR_IF2
#define ca_max(N,D,A,B)	ac_max(N,D,B,A)		/* same operation */

/* modulo */
#define	aa_mod(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] % (B)[ARR_IND] ; \
	} ARR_IF2
#define	ac_mod(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A)[ARR_IND] % (B) ; \
	} ARR_IF2
#define	ca_mod(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = (A) % (B)[ARR_IND] ; \
	} ARR_IF2

/* == */
#define	aa_equ(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] == (B)[ARR_IND]); \
	} ARR_IF2
#define	ac_equ(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] == (B)); \
	} ARR_IF2
#define ca_equ(N,D,A,B)	ac_equ(N,D,B,A)		/* same operation */

/* >= */
#define	aa_geq(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] >= (B)[ARR_IND]); \
	} ARR_IF2
#define	ac_geq(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] >= (B)); \
	} ARR_IF2
#define ca_geq(N,D,A,B)	ac_leq(N,D,B,A)		/* reversed, opposite test */


/* > */
#define	aa_gtr(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] > (B)[ARR_IND]); \
	} ARR_IF2
#define	ac_gtr(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] > (B)); \
	} ARR_IF2
#define ca_gtr(N,D,A,B)	ac_les(N,D,B,A)		/* reversed, opposite test */

/* <= */
#define	aa_leq(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] <= (B)[ARR_IND]); \
	} ARR_IF2
#define	ac_leq(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] <= (B)); \
	} ARR_IF2
#define ca_leq(N,D,A,B)	ac_geq(N,D,B,A)		/* reversed, opposite test */

/* < */
#define	aa_les(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] < (B)[ARR_IND]); \
	} ARR_IF2
#define	ac_les(N,D,A,B)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = ((A)[ARR_IND] < (B)); \
	} ARR_IF2
#define ca_les(N,D,A,B)	ac_gtr(N,D,B,A)		/* reversed, opposite test */

/* functions */
#define	aa_fun(N,D,A,B,F)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = F( (A)[ARR_IND] , (B)[ARR_IND] ); \
	} ARR_IF2
#define	ac_fun(N,D,A,B,F)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = F( (A)[ARR_IND] , (B) ); \
	} ARR_IF2
#define	ca_fun(N,D,A,B,F)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = F( (A)          , (B)[ARR_IND] ); \
	} ARR_IF2

/* typed functions */
#define	aa_t_fun(N,D,A,B,F,T)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = F( ((T)((A)[ARR_IND])), ((T)((B)[ARR_IND]))); \
	} ARR_IF2
#define	ac_t_fun(N,D,A,B,F,T)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = F( ( (T) ((A)[ARR_IND]) ) , ((T) (B)) ); \
	} ARR_IF2
#define	ca_t_fun(N,D,A,B,F,T)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while ( ARR_IND-- ) \
		(D)[ARR_IND] = F( ( (T) (A) ) , ((T) ((B)[ARR_IND]) ) ); \
	} ARR_IF2

#endif
