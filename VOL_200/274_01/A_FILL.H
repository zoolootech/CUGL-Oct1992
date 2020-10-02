/* Array Handling Package
(C) Copyright 1985,1987,1988 James P. Cruse - All Rights Reserved

a_fill.h  

A Part of the User-Supported Array Handling package. Please see
the file ARRAY.h for discussion concerning the registration and 
usage of the package.

declares all the various fill operations

Notation:
	d[]	=>	all elements of d (from 0 to n-1)
	d[i]	=>	the i'th element of d (from 0 to n-1)
	d[i-1]	=>	the i-1'th element of d (from 1 to n-1)
	s	=>	parameter s
	f()	=>	function parameter f (i.e. cos)
	T	=>	function type T (i.e. float)

The functions are: 
	a_index(n,d)	d[i] = i		index
	a_indoff(n,d,o)	d[i] = o + i		index + offset
	a_assign(n,d,c)	d[] = c			assign constant to d[]
	a_i_scale(n,d,s,o)			scale with index
			d[i] = o + i*s		linear poly on index
	a_fill(n,d,b,e)	d[i] = b + i*((e-b)/n)	exclusive fill
				d[0] = b, d[n] would be e
	a_ifill(n,d,b,e) d[i] = b + i*((e-b)/(n-1) inclusive fill
				d[0] = b, d[n-1] = e
	a_t_fill(n,d,b,e,T)			typed exclusive fill
			d[i] = b + (T)i*(((T)(e-b))/(n))
	a_t_ifill(n,d,b,e,T)			typed inclusive fill
			d[i] = b + (T)i*(((T)(e-b))/(n-1))
	a_f_fun(n,d,f()) d[] = f()		fill with return of function
	a_i_fun(n,d,f()) d[i] = f(i)		function fill with index parm
	a_t_i_fun(n,d,f(),T) d[i] = f( (T)i )	typed function fill, type index


Note:
	the various fill behave somewhat non-intuitivally due to using
	integers as index. The following examples should clear up the 
	differences. f is a float destination array, i is the index, b = 1,
	and e = 10
						   f(x)
call:		rval:i=>   0	1	3	  5	    8	      9

a_fill(10,f,0,10)	=> 0  1.000000  3.000000  5.000000  8.000000  9
a_ifill(10,f,0,10)	=> 0  1.000000  3.000000  5.000000  8.000000  10
a_t_fill(10,f,0,10,float)  0  1.000000  3.000000  5.000000  8.000000  9
a_t_ifill(10,f,0,10,float) 0  1.111111  3.333333  5.555555  8.888889  10
a_fill(10,f,0.0,10.0)	   0  1.000000  3.000000  5.000000  8.000000  9
a_ifill(10,f,0.0,10.0)	   0  1.111111  3.333333  5.555555  8.888889  10

What this all means is two things: 
1> if you do not type the fill to a float or use a float b or e parameter, 
the numbers you get back may not be what you expect. 
2> The difference between fill and ifill is whether or not the last 
index is going to be exactly the end point or not:
	d[n-1] = b + (n-1)*(e-b)/(n-1) 	=> e 		in ifill
	d[n-1] = b + (n-1)*(e-b)/n 	=> e - (e-b)/n 	in fill.

*/	

/* check to make sure array.h has been included */
#ifndef	ARR_IF_NEEDED
#include "array.h"
#endif



/* check to see if we have been loaded */
#ifndef	ARR_FILL_LOADED
#define	ARR_FILL_LOADED	1

/* index */
#define	a_index(N,D)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N);  \
	while (ARR_IND--) \
		(D)[ARR_IND] = ARR_IND; \
	}  ARR_IF2

/* index + offset */
#define	a_indoff(N,D,O)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N);  \
	while (ARR_IND--) \
		(D)[ARR_IND] = ARR_IND + (O); \
	}  ARR_IF2

/* fill, exclusive */
#define	a_fill(N,D,B,E)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N);  \
	while (ARR_IND--) \
		(D)[ARR_IND] = (B) + ( (ARR_IND * ((E)-(B)) ) / (N) ); \
	}  ARR_IF2

/* fill, inclusive */
#define	a_ifill(N,D,B,E)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while (ARR_IND--) \
		(D)[ARR_IND] = (B) + ( (ARR_IND * ((E)-(B)) ) / (N-1) ); \
	} ARR_IF2

/* typed fill, exclusive */
#define	a_t_fill(N,D,B,E,T)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while (ARR_IND--) \
	 (D)[ARR_IND] = (T) ( (B) + ( (ARR_IND * ((T) ((E)-(B))) ) / (N) ) ); \
	} ARR_IF2

/* typed fill, inclusive */
#define	a_t_ifill(N,D,B,E,T)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while (ARR_IND--) \
	 (D)[ARR_IND] = (T)((B) + ( (ARR_IND * ((T) ((E)-(B))) ) / (N-1) ) ); \
	} ARR_IF2

/* assign */
#define	a_assign(N,D,C)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N);  \
	while (ARR_IND--) \
		(D)[ARR_IND] = (C); \
	}  ARR_IF2

/* scale & offset */
#define	a_i_scale(N,D,S,O)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while (ARR_IND--) \
		(D)[ARR_IND] = (S)*ARR_IND + (O); \
	} ARR_IF2

/* function */
#define	a_f_fun(N,D,F)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while (ARR_IND--) \
		(D)[ARR_IND] = F(); \
	} ARR_IF2

/* index function */
#define	a_i_fun(N,D,F)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while (ARR_IND--) \
		(D)[ARR_IND] = F(ARR_IND); \
	} ARR_IF2

/* typed index function */
#define	a_t_i_fun(N,D,F,T)	ARR_IF1 { \
	ARR_TYPE_IND ARR_IND = (N); \
	while (ARR_IND--) \
		(D)[ARR_IND] = F((T)ARR_IND); \
	} ARR_IF2

#endif
