//////// COMPLEX VECTORS
//
// VECZNORM.CPP
//		norm  of a complex vector = sum of norms of complex elements
//		also equals sum of norms of real and imag vector parts.
//
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"

float norm (CVector& vec,  int n1, int n2 )
	// norm between point1 and point2	(friend function of CVector)
	// if point2 == -1, sums to end of V.
		{
		return ( norm(vec.x,n1,n2) + norm(vec.y,n1,n2) );
		}
//---------------------- end VECZNORM.CPP ----------------------------
