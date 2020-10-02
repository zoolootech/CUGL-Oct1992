//  VECPOLAR.CPP
//			routines for polar 2 rect conversion.
//			applies to complex vectors.

#include <stdlib.h>
#include "wtwg.h"
#include "dblib.h"
#include "vector.h"


void ToPolar ( complex& z )
	{
	double fmag   = norm ( z ); 
	z = complex ( ( (fmag>SMALL_VAL)? sqrt(fmag) : fmag * 2 ), arg(z) );
	}
	
	 
void 	CVector::ToPolar ( void )
	// 	convert a complex CVector to mag/phase form
	//	The range of the phase angle is -PI-->+PI, in radians
	{
	if ( ispolar ) return;
	int vn = x.n;
	float *xv = x.v;
	float *yv = y.v;
	complex  rect;
		
	while ( --vn >= 0 ); 
		{
		rect	= complex (*xv, *yv);
		::ToPolar(rect);		// note scope resolution    		
		*(xv++)   = real(rect);
		*(yv++)   = imag(rect);
		}
	ispolar = ON;
	
	return;	/* CVector::ToPolar() */
	}

//---------------------- end of VECPOLAR.CPP