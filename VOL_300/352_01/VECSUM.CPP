// vecsum.cpp - sum elements of vector between 2 endpoints 

#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"


	float sum (Vector& vec,  int n1, int n2 )
	// sum between point1 and point2 (friend function of Vector)
	// if point2 == -1, sums to end of vec
		{
		float s=0;

		if ( n2<0 ) n2 = vec.n;
		float *vv =vec.v;
		for ( int i=n1; i<n2; ++i )
			{
			s+= vv[i];
			}
		return (s);
		}
//----------------------------- end VECSUM.CPP --------------------------