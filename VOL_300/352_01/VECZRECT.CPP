//////// COMPLEX VECTORS
//
//	VECZRECT.CPP - rectify a complex vector in polar form.
//					this involves checking magnitudes for negative values
//					which might occur after some scalar to complex operations.
//
//
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"

void CVector::rectify(void)
		{
		if ( !ispolar ) return;
		int vn = x.n;
		float *vm = x.v, *va = y.v;		// mag and angle arrays.
		float temp;
		while ( --vn >= 0 )
			{
			if ( (temp =vm[vn]) < 0 ) 
				{
				vm[vn] = -temp;		// make mag. positive.
				va[vn] += PI;		// add PI to phase.	
				}	
			}
		}
//----------------------end of VECZRECT.CPP -------------------------

