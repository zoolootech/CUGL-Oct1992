//////// COMPLEX VECTORS
//
//		sum of elements in a complex vector.
//		 
//
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"

	complex sum (CVector& vec,  int n1, int n2 )
	// sum between point1 and point2 (friend function of CVector)
	// if point2 == -1, sums to end of vec
		{
		float sreal= sum(vec.x, n1,n2);
		float simag= sum(vec.y, n1,n2);
		return ( complex(sreal,simag) );
		}
//--------------------- end VECZSUM.CPP ---------------------
