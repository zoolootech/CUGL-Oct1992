/*
HEADER:		;
TITLE:          Series approximation for the exponential function e^x;
VERSION:	1.0;

DESCRIPTION:	Calculates the value of exp(x) using a Taylor series
		expansion of e^x.  The function as written converges rapidly
		and can handle large values for x (i.e. up to 709.7).  Also,
		this routine demonstrates techniques for speeding up
		convergence and for handling large arguments in the series
		approximation;

KEYWORDS:       Exp(x), math functions, approximations;
SYSTEM:		MSDOS;
FILENAME:       EXP;
WARNINGS:       Bounds limits exceeded;

SEE-ALSO:	EXP, SINX, LNX, MATHCLUD.FUN;

AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C, will also compile under QuickC, MS-C and PowerC;
*/
/* This program demonstrates several numerical calculations including    */
/* calculation of exp(x), x^y (y integer,x>0), N factorial and absolute  */
/* value of x.  It should be noted that a function to calculate x^y for  */
/* x>0 and y fractional or integer may be developed from the routines to */
/* calculate exp(x) and lnx(x).                                          */
/*************************************************************************/

#include "mathclud.fun"
double Xpon(double);

double Exp(double earg)
{
     const double e=2.71828182845904523536;
     double ex,extmp;
     int exint,i,neg=0;
     if(earg>709.7)
     {
       printf("Bounds limit exceeded");
       return(-1);
     }
     if(earg<0)
     {
       earg = -earg;
       neg = 1;     /* set neg flag if argument is negative */
     }
 /* separate argument into an integral part and a fractional part.  Then  */
 /* calculate exp(x) as exp(x1)*exp(x2) where x = x1 + x2.                */
     exint = (int)earg; extmp = earg - exint;
     ex = Xpon(extmp);
     ex = ex*Intpwr(e,exint);
     if(neg)
       ex = 1/ex;
     return (ex);
}
double Xpon(double x) /* Calculate exp(x) where x is the fractional part */
{                     /* of the original argument.  This insures rapid   */
     double s1=0,s2=0;   /* convergence and prohibits exceeding bounds.  */
     int i=0;            /* for purposes of speed, x may be a number in  */
     if(!x)              /* the range 0 < x < 10.  Although the original */
       return(1);        /* argument may be as large as 709.7            */
     for(;;)
     {
       s1 = s1 + (Intpwr(x,i))/(Ifac(i));
       if(Abs_val(s1-s2)<1.0e-9)
	 break;
       s2 =s1; i++;
     }
     return(s1);
}