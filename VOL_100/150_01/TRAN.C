/*
       HEADER:  150.??;
        TITLE:  Transcendental Function Library;
  DESCRIPTION:  "C subroutines to compute square root, log (to base e), and
                 exponential functions.  Iterates until 1E-7 difference.";
     KEYWORDS:  Transcendental Functions, Exponential, Square Root, Log;
       SYSTEM:  any with suitable C compiler;
     FILENAME:  TRAN.C;
     WARNINGS:  "1) User documentation not included. 
                 2) Requires a C compiler with double precision real numbers.";
     SEE-ALSO:  HP.C;
    COMPILERS:  any C compiler with double prec. reals;
   REFERENCES:  AUTHORS: unknown; TITLE: "TRAN.C";
                CITATION: "PC-SIG Disk 50 or 142" 
       ENDREF;
*/
/* Transcendental function library */
#define E 2.718281828

/* Square root of x */
double root(x)
double x;
{
double a, x1, x2=1;
x1 = x;
if (x > 0) {
while (x2 > .00000001) {
	a = x;
	x = x1 / a + a;
	x = x / 2;
	if (x >= a) x2=x-a;
	else x2=a-x;
	}
}
else x=0;
return (x);
}

/* Logarithm of x */
double log(x)
double x;
{
double a=0, x1, x2=0, x3=1;
int t;
	if (x>0) {
	x1 = x;
	for (t=0; x1<.5; t--)
		x1 *= E;
	for (t=t; x1>1.5; t++)
		x1 /= E;
	a = t;
	x1 = x1-1;
	x  = -x1;
	for (t=1; x3>.0000001; t++) {
		x2 = x1/t;
		a = a + x2;
		x1 = x1 * x;
		x3 = (x2>=0) ? x2 : -x2;
	}
	}
return (a);
}

/* Exponential of x */
double exp(x)
double x;
{
double a, x1, x2, x3, x4, x7=1;
int x5, x6, t;
	if (x5=(x<0)) x=-x;
	t  = x;
	x4 = 1;
	x = x-t;
	for (x6 = 1; x6 <= t; x6++)
		x4 *= E;
	x1=x;
	x=1;
	x2=x1;
	x3=0;
	for (x6=2; x7>.0000001; x6++) {
		x=x+x2;
		x2=x2*x1/x6;
		x7 = (x2 >= 0) ? x2 : -x2;
	}
	x = x*x4;
	return (x5 ? 1/x : x);
}
