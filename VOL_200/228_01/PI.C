/*
HEADER:         CUGXXX;
TITLE:          Pi calculator;
DATE:           3-20-86;
DESCRIPTION:    Computes pi to the specified number of decimal places;
KEYWORDS:       Pi;
FILENAME:       PI.C;
WARNINGS:       Execution time is nearly proportional to the square of
                the number of digits computed, 10000 digits take >51 hours;
AUTHORS:        R. E. Sawyer;
COMPILER:       Desmet C V2.4 on PC-DOS V2.1;
REFERENCES:     US-DISK 1308;
ENDREF
*/
/*   pi.c
*    a program to compute pi to an arbitrary number of decimal places
*
*    Method:  Numbers are represented as character arrays, with each
*	      character being one decimal digit of the number.	All
*	      numbers so represented are assumed to be non-negative
*	      and less than ten, e.g.
*		  3.14159...  <-->  a[] = {3, 1, 4, 1, 5, 9, ...}
*		  1/8	      <-->  a[] = {0, 1, 2, 5, 0, 0, ...}
*		  1/57	      <-->  a[] = {0, 0, 1, 7, 5, 4, ...}
*	      The value of pi is computed using the identity
*	      pi = 24 arctan(1/8) + 8 arctan(1/57) + 4 arctan(1/239),
*	      where arctan(x) = x - x*x*x/3 + x*x*x*x*x/5 - ...
*	      and the arithmetic is performed on the numbers as arrays.
*
*    Accuracy:	All digits are correct -- verified to 10000 digits;
*		the unsigned data type should handle over 50000 digits,
*		but for more than that, longs should be used instead.
*
*    Timing:  Execution time is nearly proportional to the square of
*	      the number of digits computed; some approximate times on
*	      an IBM-PC:
*			 200 digits	  1m 17s
*			 500		  7m 45s
*			1750	      1h 34m
*		       10000	     51h 08m
*
*    Language:	C (DeSmet 2.4, PC-DOS 2.1)
*
*    Author:  R. E. Sawyer, 3620 Spencer St. 30, Torrance, CA 90503
*
*    Date:  19 Jan 84
*/

#define MAXDIGITS    30000	   /*maximum number of digits allowed*/
#define EXTRA	     3		   /*extra digits computed internally*/
#define MAXSIZ	     MAXDIGITS + EXTRA
#define TRUE 1
#define FALSE 0

int numDigits;			   /*number of digits requested*/
int hiDigit;			   /*highest array index needed*/

main(argc, argv)
    int argc;
    char *argv[];
    {
    char pi[MAXSIZ],
	 tmp[MAXSIZ];
    prolog(argc, argv);
    arctan(pi, 8);
    mul(pi, 24);
    arctan(tmp, 57);
    mul(tmp, 8);
    add(pi, tmp);
    arctan(tmp, 239);
    mul(tmp, 4);
    add(pi, tmp);
    report(pi);
    }
/*-------------validate user request */
void prolog(ac, av)
    int ac;
    char *av[];
    {
    if ( ac != 2)
	{
	printf("\nUsage:  pi <numberOfDigits>\n");
	exit();
	}
    if ((numDigits = atoi(av[1])) > MAXDIGITS)
	{
	printf("\n<numberOfDigits> must not exceed %d\n", MAXDIGITS);
	exit();
	}
    hiDigit = numDigits + EXTRA - 1;
    }
/*-------------write (a[]) to stdout*/
void report(a)
    char a[];
    {
    int i;

    printf("\n%u.\n", a[0]);
    for (i = 1; i < numDigits; ++i)
	printf("%u", a[i]);
    printf("\n");
    }
/*-------------replace (a[]) by arctan(1/n) */
void arctan(a, n)
    char a[];
    unsigned n;
    {
    char t[MAXSIZ];
    unsigned k, nn;
    int isMinus;

    zero(t);
    t[0] = 1;
    div(t, n);
    zero(a);
    add(a, t);
    nn = n * n;
    k = 1;
    isMinus = TRUE;
    do
	{
	mul(t, k);
	k += 2;
	div(t, k);
	div(t, nn);
	if (isMinus)
	    sub(a, t);
	else
	    add(a, t);
	isMinus = !isMinus;
	}
	while (isNotZero(t));
    }
/*-------------set (a[]) equal to 0 */
void zero(a)
    char a[];
    {
    int i;

    for (i = 0; i <= hiDigit; ++i)
	a[i] = 0;
    }
/*-------------replace (a[]) by (a[])*n */
void mul(a, n)
    char a[];
    unsigned n;
    {
    int i;
    long b;

    for (i = hiDigit, b = 0; i >= 0; --i)
	{
	b = (long)(a[i]) * n  +  b / 10L;
	a[i] = b % 10L;
	}
    }
/*-------------replace (a[]) by (a[])/n */
void div(a, n)
    char a[];
    unsigned n;
    {
    int i;
    long b;
    for (i = 0, b = 0; i <= hiDigit; ++i)
	{
	b = 10L * (b % n) + (long)(a[i]);
	a[i] = b / n;
	}
    }
/*-------------replace (a[]) by (a[])+(b[]) */
void add(a, b)
    char a[], b[];
    {
    int i;

    for (i = hiDigit; i >= 0; --i)
	{
	if (a[i] + b[i] > 9)
	    {
	    a[i] -= 10 - b[i];
	    ++a[i-1];
	    }
	else
	    a[i] += b[i];
	}
    }
/*-------------replace (a[]) by (a[])-(b[]) */
void sub(a, b)
    char a[], b[];
    {
    int i;

    for (i = hiDigit; i >= 0; --i)
	{
	if (a[i] + 1 < b[i] + 1)
	    {
	    a[i] += 10 - b[i];
	    --a[i-1];
	    }
	else
	    a[i] -= b[i];
	}
    }
/*-------------test whether (a[]) is 0 */
int isNotZero(a)
    char a[];
    {
    int i;

    for (i = 0; i <= hiDigit; ++i)
	if (a[i] != 0)
	    return TRUE;
    return FALSE;
    }

