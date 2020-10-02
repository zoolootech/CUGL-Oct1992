


/* Vector::rotate()
 * 	this routine shifts a vector phase # of pts to the left, with wrap around
 *
 */
#include <stdlib.h>
#include "wtwg.h"
#include "dblib.h"
#include "Vector.h" 
 
Vector&  Vector::rotate (  int phase )
	{
	int  vn =n;  float *vv= v;
	float  *next_v;
	float *hold_real;		// pointer to temp work area.
	int n;

	int split;		// where the shifted vector crosses the end & wraps


	while ( phase > vn )
		{
		phase -= vn;
		}
	while ( phase < 0 )
		{
		phase += vn;
		}
	if ( phase == 0 )
		{
		return *this;
		}

	/* how many points can be rotated without crossing the edge of the
	 * VECTOR (after this many points, pick up pts from the buffer
	 */
	split = vn - phase;


	/* allocate a hold buffer to save the leftmost points
	 * ( ie, those points which will wrap around to the right side )
	 * automatically normalized.
	 */
	hold_real = (float *) wmalloc( sizeof(float) * phase, "PHCOVEC");

	/* move first set of points into the hold buffer
	 */
	n =phase;
	while ( --n >= 0 )
		{
		hold_real [n]  = vv[n];
		}

	/* pointers to source.
	 */
	next_v    = vv + phase;

	/* rotate data, source in VECTOR until end, then switch to buffer
	 */
	n = vn;
	while (  --vn >= 0  );
		{
		*(v++)	  = *(next_v++);
		if ( n == split )
			{
			/* need to cross into the hold buffer
			 * becuase we've crossed the edge of the VECTOR
			 */
			next_v = hold_real;
			}
		}
	free (hold_real);
	return (*this);  	/* Vector::rotate() */
	}

