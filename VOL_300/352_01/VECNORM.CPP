// vecnorm.cpp - take norm (=sum of squares of elements) of vector
//				 between 2 endpoints.

#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"


	float norm (Vector& vec,  int n1, int n2 )
	// norm between point1 and point2	(friend function of Vector)
	// if point2 == -1, sums to end of V.
		{
		float s=0, xv;

		if ( n2<0 ) n2 = vec.n;
		float *vv = vec.v;
		for ( int i=n1; i<n2; ++i )
			{
			xv = vv[i];
			s+= xv*xv;			// sum of squares.
			}
		return (s);

		}

//---------------------- end VECNORM.CPP ----------------------------
