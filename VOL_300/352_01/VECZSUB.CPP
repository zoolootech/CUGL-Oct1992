//////// COMPLEX VECTORS
//
//		primary subroutines to support complex vector math :
//		subtraction.
//

#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"


	CVector& CVector::operator-=(CVector&  z)
		// subtract CVector z from CVector this.
		{
		if ( ispolar || z.ispolar )	werror ( 'V', "ADDING POLAR VECTORS" );
		
		x -= z.x;
		y -= z.y;

		return *this;
		}


	CVector& CVector::operator-=(complex z)
		// Subtract complex z from CVector this
		{
		if ( ispolar ) werror ( 'V', "ADDING POLAR VECTORS" );
		
		x -= real (z);
		y -= imag (z);

		return *this;
		}


	CVector& CVector::operator-=(Vector&  vec)
		// subtract real Vector vec from CVector this.
		{
		if ( ispolar )	werror ( 'V', "ADDING POLAR VECTORS" );
		
		x -= vec;
		
		return *this;
		}


	CVector& CVector::operator-=(float  f)
		// Subtract float f from CVector this
		{
		if ( ispolar )	werror ( 'V', "ADDING POLAR VECTORS" );
		
		x -= f;

		return *this;
		}
//---------------------end VECZSUB.CPP -----------------------------
