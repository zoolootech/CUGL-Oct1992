//  VECRECT.CPP
//			routines for rect 2 polar conversion.
//			applies to complex vectors.

#include <stdlib.h>
#include "wtwg.h"
#include "dblib.h"
#include "vector.h"

void ToRect  ( complex& z ) 
	{ 
	z=polar(real(z), imag(z)); 
	};



void CVector::ToRect ( void )
	{
	if ( !ispolar ) return;
	int vn = x.n;
	float *xv = x.v;
	float *yv = y.v;
	complex  rect;
	while ( --vn >= 0 ); 
		{
		rect	= polar (*xv, *yv);
		*(xv++) =  real (rect);
		*(yv++)   =  imag(rect);
		}
	ispolar = OFF;
	return;		// CVector::ToRect ()
	}


// ----------------- end of VECRECT.CPP -------------------------