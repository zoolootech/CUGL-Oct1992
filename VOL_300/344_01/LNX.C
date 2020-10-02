/*
HEADER:		;
TITLE:		Series approximation of ln(x);
VERSION:	1.0;

DESCRIPTION:	Function to perform a Taylor series approximation of the
		function ln(x).  Large arguments are easily handled and
		convergence is rapid;

KEYWORDS:	log(x), math functions, approximations;
SYSTEM:		MSDOS;
FILENAME:	LNX;
WARNINGS:       Domain errors;

SEE-ALSO:	LNX, SINX, EXP, MATHCLUD.FUN, XPON;
AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C, will also compile under QC, MS-C, power C;
*/
/*  Natural logarithm of x */
#include "mathclud.fun"

double lnx(double x)
{
     double sum=0,sum1=0,logtoln=2.30258509299404568402;
     int i=1,basecount;
     if(x<=0)
     {
     printf("Lnx is undefined at %lf\n",x);
     return(-1e710);
     }
/* Caculate ln(x) as ln(x1) + ln(x2) where x = x1*x2 and x1 is a power of */
/* 10 and ln(x1) = x1*ln(10) = x1*2.302... and x2 lies between 1 and 10   */
     if(x<1)
     {              /* Do if x is a fractional number smaller than 1 */
       for(basecount=0;x<1;basecount--)
	 x = x*10.0;
       if(x>3)
       {
	 x = x/10.0;
	 basecount++;
       }
     }
     else
     {               /* Do if x is a number greater than or equal to 1 */
       for(basecount=0;x>1;basecount++)
	 x = x/10.0;
       if(x<=0.3)
       {
	 x = x*10.0;
	 basecount--;
       }
     }
     logtoln = logtoln*basecount;
     x = (x-1)/(x+1);  /* Series expansion of lnx for x>0 */
     for(;;)
     {
       sum = sum + Intpwr(x,i)/i;
       i += 2;
       if(Abs_val(sum - sum1)<=1e-15)
	 break;
       sum1 = sum;
     }
     sum = 2*sum + logtoln;
     return (sum);
}