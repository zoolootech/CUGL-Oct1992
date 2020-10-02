/* VECBASE.CPP
 *	fill a Vector with uniform data values
 *	to use as a time base for graphing
 *	or for computing phase2time()
 */
#include <stdlib.h>
#include "wtwg.h"
#include "dblib.h"
#include "vector.h"


void Vector::base ( float start, float final )
	{
	float *vv =v; int vn =n;
	float val;
	float incr;
	int i;
	
	if ( vn == 0 ) werror ( 'V', "Vector::base() on zero length Vector");
	incr = (final - start)/ vn;
	

	for ( i=0, val=start; i< vn; ++i, val+=incr )
		{
		vv[i] = val;
		}
	return;	/* Vector::base() */
	}
//-------------------- end VECBASE.CPP ---------------------- 
