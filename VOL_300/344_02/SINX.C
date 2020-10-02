/*
HEADER:		;
TITLE:          Sin(x) series approximation;
VERSION:	1.0;

DESCRIPTION:    Function to demonstrate how a Taylor series expansion of
		sin(x) may be implemented.  Convergence is fast and
		arguments are accepted in degrees.  Large arguments accepted.;

KEYWORDS:       Sin(x), math functions, approximations;
SYSTEM:		MSDOS 1.x, 2.x, 3.x;
FILENAME:	SINX;
WARNINGS:       None ;

SEE-ALSO:	LNX, EXP, IFAC, ABS_VAL,MATHCLUD.FUN;
AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C 2.0;
*/

#include "mathclud.fun"

double sinx(double sinarg)
{
     double sum=0.0,s1=100.0,xval,radtodeg=3.14159265358979/180;
     int k=0,oddeven=1;
     sinarg = ((sinarg/360)-(int)(sinarg/360))*360.0; /*  convert to  */
     sinarg = sinarg*radtodeg;           /*  radians and scale input  */
     for(;;)
     {
       sum = sum + oddeven*Intpwr(sinarg,2*k+1)/Ifac(2*k+1);
       oddeven = oddeven*(-1);
       k++;                            		    /* sum series */
       if(Abs_val(s1-sum)<=1e-10)
	 break;
       s1 = sum;
     }
     return (sum);
}