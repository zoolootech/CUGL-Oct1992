/* Array Handling Package
(C) Copyright 1985,1987,1988 James P. Cruse - All Rights Reserved

a_testf.c  

A Part of the User-Supported Array Handling package. Please see
the file ARRAY.h for discussion concerning the registration and 
usage of the package.

This program does a Quick test of some of the Array Package routines.

Tests using floating point operations.

Numerically proves that sin^2+cos^2 = 1.

*/	

#include <stdio.h>
#include <math.h>

#include "array.h"

#define 	ARRSIZE		100 	/* Number of Elements in Array */

/* global arrays */
double	sin_array[ARRSIZE];
double 	cos_array[ARRSIZE];
double	sum_array[ARRSIZE];


/* get 2pi */
#define	PI2	(2*3.1415)

main()
{
double sum;

/* Test whether or not your compile can handle curly braces without */
/* having a conditional prior to them */
/* If your compiler generates an error, try replacing ARR_IF_NEEDED */
/* with 1 in array.h */

ARR_IF1
{		
sum = 1.0*ARRSIZE;		/* do something */
} ARR_IF2


printf("Test the Hypothesis sin^2 + cos^2 = 1.\n");
printf("\nMake 2 arrays, sin^2 and cos^2, then sum to verify.\n");
printf("If sin^2 + cos^s = 1, then summing all should result in the\n");
printf("Length of the Arrays\n");

printf("Filling Arrays...\n");

/* lets first fill the arrays */
a_ifill(ARRSIZE,sin_array,0.0,PI2);
a_ifill(ARRSIZE,cos_array,0.0,PI2);


printf("Getting Sines and Cosines\n");

/* now get the sin & cos */
a_fun(ARRSIZE,sin_array,sin_array,sin);		/* sin defined in math.h */
a_fun(ARRSIZE,cos_array,cos_array,cos);		/* cos defined in math.h */


printf("Squaring\n");

/* now square */
aa_mul(ARRSIZE,sin_array,sin_array,sin_array);
aa_mul(ARRSIZE,cos_array,cos_array,cos_array);

printf("Adding\n");

/* now sum the two arrays */
aa_add(ARRSIZE,sum_array,sin_array,cos_array);


printf("Display some Values\n");

/* now display the some of them */
printf("Results: [0] = %f, [1] = %f, [%d] = %f, [%d] = %f\n",	\
	sum_array[0],sum_array[1],ARRSIZE/2,sum_array[ARRSIZE/2],	\
	ARRSIZE-1,sum_array[ARRSIZE-1]);


printf("Now Summing all sin^2 + cos^2\n");

/* now add them all up */
a_sum(ARRSIZE,sum,sum_array);


printf("Results:\n");

/* now results are? */
printf("Total: %f, Size %f, Total/Size %f, Total-Size %f\n",	\
	sum,(double)ARRSIZE,sum/ARRSIZE,sum-ARRSIZE);


/* QED */
printf("\nQED\n");

exit(0);
}
