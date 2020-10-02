/* Array Handling Package
(C) Copyright 1985,1987,1988 James P. Cruse - All Rights Reserved

a_testi.c  

A Part of the User-Supported Array Handling package. Please see
the file ARRAY.h for discussion concerning the registration and 
usage of the package.

This program does a Quick test of some of the Array Package routines.

Tests using Integer operations.

Show that the sum of the first N terms of an arithmetic sequence 
	sn = a + (n-1)*d
is
	Sn = (n/2)*(2*a + (n-1)*d)

*/	

#include <stdio.h>

#include "array.h"

#define 	ARRSIZE		50  	/* Number of Elements in Array */


/* globals for arithmetic series */
#define		A		2
#define		D		5

/* global arrays */
int	sn_array[ARRSIZE];

main()
{
int 	i;
int	calc_sum;
int 	sum;

/* Test whether or not your compile can handle curly braces without */
/* having a conditional prior to them */
/* If your compiler generates an error, try replacing ARR_IF_NEEDED */
/* with 1 in array.h */

ARR_IF1
{		
sum = ARRSIZE;		/* do something */
} ARR_IF2


printf("Test the sum of an arithmetic series is Sn = (n/2)*(2*a + (n-1)*d)\n");
printf("\nFill Array with Series, then sum to verify.\n");
printf("If true, then summing should result in a simply calculated value\n");

printf("Filling Arrays...\n");

printf("Calculating Arithmentic Series\n");
a_i_scale(ARRSIZE,sn_array,D,A);	/* C already starts @ 0, no n-1 needed*/


printf("Display some Values\n");

/* now display the some of them */
printf("Results: [0] = %d, [1] = %d, [%d] = %d, [%d] = %d\n",	\
	sn_array[0],sn_array[1],ARRSIZE/2,sn_array[ARRSIZE/2],	\
	ARRSIZE-1,sn_array[ARRSIZE-1]);


printf("Now Summing Array\n");

/* now add them all up */
a_sum(ARRSIZE,sum,sn_array);


printf("Calculating Answer\n");

calc_sum = (ARRSIZE*(2*A + (ARRSIZE - 1) * D))/2;	/* /2 last: roundoff */


printf("Results:\n");

/* now results are? */
printf("Total: %d, Calculated %d, Total-Calculated %d\n",	\
	sum,calc_sum,sum-calc_sum);


/* QED */
printf("\nDone\n");

exit(0);
}
