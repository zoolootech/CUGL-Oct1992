/*--------------------------------------------------------------------------
**		file: 		MATH.C
**		project:	hoc3, "Higher Order Calculator"
*/

#include <math.h>
#include <errno.h>
#include "hocdecl.h"

extern int errno ;
double errcheck() ;

double Log( double x)
{
   return errcheck( log(x), "log") ;
}

double Log10( double x)
{
   return errcheck( log10(x), "log10") ;
}

double Exp( double x)
{
   return errcheck( exp(x), "exp") ;
}

double Pow( double x, double y)
{
   return errcheck( pow(x, y), "exponentiation") ;
}

double integer( double x)
{
   return (double)(long) x ;
}

double Sqrt( double x)
{
   return errcheck( sqrt(x), "sqrt") ;
}

double errcheck( double d, char *s)
{
	if (errno == EDOM) {
		errno = 0 ;
		execerror( s, "argument out of domain") ;
	}
	else if (errno == ERANGE) {
		errno = 0 ;
		execerror(s, "result out of range") ;
	}

	return d ;
}

