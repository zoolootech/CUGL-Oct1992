// VECCORLT.CPP
//	this routine computes the correlation function between 2 real Vectors
//
//	where:
//	   Vector x, y  = Vector of real (float) data
//	   float *r	= r-value
//	   float *t = t-value
//
//	RETURNS: void. sets *r, *t to r & t values for correlation.
//
//
//	NOTES:
// 	This routine computes correlation using linear programming methods
//	for one-dimensional datasets. No provision is made for singular data
//
//	uses
// r = SUM{ x-xbar)*(y-ybar) }/ sqrt( SUM{ (x-xbar)**2 }*SUM{ (y-ybar)**2 } )
//
//	replacing:
//	SUM{ (x-xbar)**2 }       = SUM{x**2} - ( SUM{x}**2 /n )
//	SUM{ (x-xbar)*(y-ybar) } = SUM{x*y}  - ( SUM{x}*SUM{y}/n )
//
// t = r * sqrt( (n-2)/(1-r**2) )
//
// df = n-2
//

#include <stdlib.h>
#include <math.h>

#include "wtwg.h"
#include "dblib.h"
#include "vector.h"

void correlate ( Vector& x, Vector& y, float *r, float *t )
	{
	float sumx, sumy, sumsqx, sumsqy, sumxy;
	float xval, yval;
	float nf;
	float rval;
	float denom;		// denominator - prevent zero divide.

	int ndata = min ( x.n, y.n );

	float  *yv = y.v,  *xv = x.v; 		// pointers to data.

	sumx = sumy = sumsqx = sumsqy = sumxy =0;
	
	*r = *t = 0;
	if ( ndata <= 2 ) return; 
	

	for (int i =0; i<ndata; ++i )
		{
		xval = xv[i];
		yval = yv[i];

		sumx += xval;
		sumy += yval;

		sumsqx += xval*xval;
		sumsqy += yval*yval;

		sumxy  += xval*yval;
		}


	if ( sumsqx < SMALL_VAL || sumsqy < SMALL_VAL )
		{
		/* one of the datasets consistantly equals its mean
		 */
		return;
		}

	nf = ((float) ndata);

	denom = (sumsqx - (sumx*sumx/nf)) * (sumsqy - (sumy*sumy/nf));
	
	if ( denom > SMALL_VAL )
		{
		rval = (sumxy - sumx*sumy/nf) /  sqrt (denom);
		}
	else
		{
		rval = LARGE_VAL;	// prevent zero divide.	(should never happen)
		}
		
	*r = rval;

	denom = (  1.0 -(rval*rval) );
	if ( denom > SMALL_VAL )
		{
		*t = rval * sqrt( (nf- 2.0) / ( denom ) ) ;
		}
	else
		{
		*t = LARGE_VAL;
		}



	return;	/* VECTOR_correlate */
	}
