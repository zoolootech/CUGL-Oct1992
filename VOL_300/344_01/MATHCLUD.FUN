/*
HEADER:		;
TITLE:		Math functions support routines;
VERSION:	1.0;

DESCRIPTION:	This is an include file containing various routines called
		by the functions EXP,SINX and LNX;

KEYWORDS:	Math funtions, approximations;
SYSTEM:		MSDOS;
FILENAME:	MATHCLUD.FUN;
WARNINGS:	None;

SEE-ALSO:	EXP, SINX, LNX, XPON;
AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C, also works with MS-C, Quick C, and Power C;
*/
double Intpwr(double x, int y)  /* x^y y is an integer */
{
     int idx;
     double xpdt=1;
     for(idx=0;idx<y;idx++)
       xpdt = xpdt*x;
     return(xpdt);
}
double Ifac(int n)   /* calculate n factorial */
{
     int i;
     double fac=1;
     for(i=1;i<=n;i++)
       fac = fac*i;
     return(fac);
}
double Abs_val(double xval)  /* absolute value of a number */
{
      return(((xval>=0) ? xval : -xval));
}