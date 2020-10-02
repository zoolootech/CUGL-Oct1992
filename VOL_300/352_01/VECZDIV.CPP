//////// COMPLEX VECTORS
//
//		primary subroutines to support complex vector math :
//		assignment, addition, and mutliplication.
//
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"

	CVector& CVector::operator/=(float  f)
		// Divide CVector this by  float f. 
		// NOTE class Vector prevents divide by zero.
		{
		x /= f;
		if ( !  ispolar )  y /= f;
		else  this->rectify ();		// if f was neg, make sure mag.s now > 0
		return *this;
		}



	CVector& CVector::operator/=(complex z)
		// Divide CVector this by complex z. NOTE prevents divide by zero.
		{
		if ( ispolar ) werror ( 'V', "DIVIDING POLAR VECTORS" );
		
		x /= real (z);
		y /= imag (z);
		
		return *this;
		}


