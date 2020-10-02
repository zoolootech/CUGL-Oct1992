/* Array Handling Package
(C) Copyright 1985,1987,1988 James P. Cruse - All Rights Reserved

array.h

Version 1.00

This set of C header files is used to simplify array handling in C.

Included are routines to add, subtract, fill, negate, etc. 


First, the Legal Issues:


The routines are copyrighted by the author, and have been released as a
user supported program. You may use this program on a trial basis, and if 
you find it to be useful, or you continue using it, you must register it.
Any Commercial enterprises using this software must register it immediately.
You may not include the Source code of these routines with any 
commercial package without written consent of the author. You may use 
them to develop commercial programs upon registration.
Simple registration of these routines is $10.00. If you wish to recieve 
an upgrade of the routines, and trial usage of the STAT and function 
equivalents, the registration fee is $25.00. To recieve a copy of the 
Scientific Functions is $50.00. (See Description below).

Registration should be mailed to:

	James P. Cruse
	919 Capitola Avenue
	Suite 41
	Capitola, CA 95010
	(408) 475-7444 (m)


I am currently developing other routines for C, as well as improving
these. Routines that I am developing include: integer, float, and
"user" defined functions (not macros) that are equivalent to these routines;
Simple Statistical functions (mean,median,mode,r^2,s^2,standard deviation);
Scientific Functions (polynomials, curve fits, convolutions); and
Plotting routines for the IBM PC/XT/AT and compatibles (routines to plot on
screen, printer or HP plotter x-y plots of up to 16 parameters in a 4
windows).



Back to the routines:


The routines are broken up into the following types:

Binary Operations	Add, Subtract, Multiply, etc. between 2 arrays
Unary Operations	Negate, Assign, absolute, etc. an array
Fill Operations		Fill array with index, index+off, values between x&y
Info about Array	Min, Max, etc of an array

There are also routines to call a function on 1 or 2 arrays.

The routines work by substituting your call into a while loop. This was the 
Only way I could find to use different types of arrays/constants in an 
expression that would be compatible across a wide range of compilers/systems. 
(I spent 3 months trying all kinds of different ways, but none would 
work on ALL of the compilers I could test with, except this method).

The Functions all have a uniform naming format:

	X_NAME, where X is a, for single array operations, aa for 
		2 array, ac for array & constant, and ca for contant & array
		And NAME is the operation to be done. For Example:

		aa_add()	add two arrays
		ac_add()	add array & constant
		a_neg()		negate array
		a_index()	fill array with index

They also have a uniform parameter format:

X_NAME(s,d,...)	where s is length of array, d is destination array, and
	"..." varies for each type of operation:
		Binary operations have 2 parameters, a and b or a and c
		Unary operations have a single parameter a
		info operations have a single parameter
		and fill operations take multiple parameters.

	The function can also take other parameters, depending on the 
	operation. 


The Functions Are:


Binary Operations: (a_binop.h)
	add, sub, mul, div, min, max, mod (%,modulo),
	equ (==), geq (>=), leq (<=), gtr (>), les (<),
	fun (function), t_fun (typed function)

	aa_add(n,d,a,b)		d[] = a[] + b[]
	ac_min(n,d,a,c)		d[] = ARR_MIN(a[],c)
	ca_sub(n,d,c,a)		d[] = c - a[]

	aa_fun(n,d,a,b,f)	d[] = f(a[],b[])
	ac_t_fun(n,d,a,c,f,t)	d[] = f( (t) a[] , (t) c )


Unary Operations: (a_unop.h)
	cpy (copy,assign), neg (negate), abs (absolute value), 
	rsum (running sum), rprod (running product),
	rmin (running min), rmax (running max),
	fun (function), t_function (typed function)
	scale (scale&offset)

	a_cpy(n,d,a)		d[] = a[] 
	a_rsum			d[i] = d[i-1] + a[i]
	a_rmax			d[i] = ARR_MAX( d[i-1] , a[i] )

	a_scale(n,d,a,s,o)	d[] = o + s*a[]

	a_fun(n,d,a,f)		d[] = f(a[]])
	a_t_fun(n,d,a,f,t)	d[] = (t) f( (t) a[] )


Fill Operations: (a_fill.h)
	index, indoff (index+offset), fill (exclusive), 
	ifill (inclusive), t_fill (typed fill), t_ifill (typed inc. fill)
	scale (scale&offset), 
	assign, f_fun (function), i_fun, (function w/index param),
	t_i_fun (typed function w/index param)

	a_indoff(n,d,c)		d[i] = i + c
	a_assign(n,d,c)		d[] = c
	a_i_scale(n,d,s,o)	d[i] = s*i + o

	a_fill(n,d,s,e)		d[0] = s, d[i] = s+(e-s)/n, d[n] (would be) e 
	a_t_ifill(n,d,s,e)	d[0] = s, d[i] = s+((t)(e-s))/(n-1), d[n-1] = e

	a_f_fun(n,d,f)		d[] = f()
	a_t_i_fun(n,d,f,t)	d[i] = f( (t) i )


Info Operations: (a_info.h)
	sum, prod, min, max, minmax

	a_sum(n,d,a)		d = a[0] + a[1] + ... + a[n-1]
	a_min(n,d,a)		d = min( a[0] , a[1] , ... , a[n-1] )
	a_minmax(n,l,u,a)	l = min( a[0] , a[1] , ... , a[n-1] ),
			and	u = max( a[0] , a[1] , ... , a[n-1] )


OPTIONS:

Compiler Dependent Options:
	Set ARR_IF_NEEDED to 1 if you cannot have a curly brace without
a proceeeding if or while (see ARR_IF_NEEDED example below).
	There are macros for taking the MIN and MAX of two parameters, and
for getting the ABS of a parameter. You can modify them if you need to.
Note: they are only used in the min and max calculations, but NOT the 
the combined minmax functions. I use an if-else-then there.

Customization:
	You can turn on or off the various #defines, so that only 
certain of the routines are included by default. You can override these 
defaults by defining the #define to 1 prior to including array.h, or you
may include the file directly.

	You may modify the variable name the program uses for declaring
it's index to anything you want, I found the default to be acceptable.
	
	You may also modify the "type" of the index variable, this allows
you to declare it as a register variable, or convert it to a non-int.


COMMENTS:

	See each file for further discussion of each type of functions.

	The routines as written should be usable on a wide variety of
machines (I believe they are both K&R and ANSII compatible).

	If you have any problems, comments, or routines you would
like to have (now or in the future), I would be glad to hear from you.


			Sincerely,

			James P. Cruse

*/	

/* have we been loaded? */
#ifndef	ARR_IF_NEEDED




/* Global Declarations */

/* set to true if you cannot have {} in any location in a function */
/* e.g.:
	main()
	{
	int i;
	{			does this curly brace generate an error?
	i = 7;
	}
	printf("%d",i);
	}
*/
#define	ARR_IF_NEEDED	0	/* needs an if */


/* what type and name do you want for the indexing variable? */
#define	ARR_IND		ijklmnop
#define	ARR_TYPE_IND	int


/* How do you want to determine the MIN/MAX of two elements */
#define	ARR_MIN(X,Y)	( (X) < (Y) ? (X) : (Y) )	
#define	ARR_MAX(X,Y)	( (X) > (Y) ? (X) : (Y) )


/* how do you want to determine the ABSolute value of an element */
#define	ARR_ABS(X)	( (X) < 0 ? -(X) : (X) )


/* which functions do you want loaded by default? */


/* Unary Operations */

#ifndef	ARR_UNOP	/* overriden? */

#define	ARR_UNOP	1	/* unary (d[] = a[]) */

#endif



/* Binary Operations */

#ifndef	ARR_BINOP	/* overridden */

#define	ARR_BINOP	1	/* Binary (d[] = a[] + b[]) */

#endif



/* Fill Operations */

#ifndef	ARR_FILL 	/* overridden */

#define	ARR_FILL	1	/* fill array (d[] = i*x+b) */

#endif




/* Information Operations */

#ifndef	ARR_INFO 	/* overridden */

#define	ARR_INFO	1	/* info about array (min,max) */

#endif




/* Internal defines */
#if	ARR_IF_NEEDED		/* do we need an if statement? */
#define	ARR_IF1	if(1) 			/* declare if */
#define	ARR_IF2	else ;			/* close if */
#else				/* nope -> do nothing */
#define	ARR_IF1	
#define	ARR_IF2 
#endif


/* now we shall include wanted files */

#if	ARR_UNOP

#include "a_unop.h"

#endif

#if	ARR_BINOP	

#include "a_binop.h"

#endif

#if	ARR_FILL 	

#include "a_fill.h"

#endif

#if	ARR_INFO 	

#include "a_info.h"

#endif





/* done */
#endif	/* ARR_IF_NEEDED , -> loaded already */
