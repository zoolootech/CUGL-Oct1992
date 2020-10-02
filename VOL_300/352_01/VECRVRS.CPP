

/* VECTOR_reverse ()
 *	reverse the time direction of a complex VECTOR
 *	used by cross correlation routine
 */

#include <stdlib.h>
#include "wtwg.h"
#include "dblib.h"
#include "Vector.h" 



Vector& Vector::reverse ( void )
	{
	int vn = n;  float *vv = v;
	float hold;
	float  *vvend = vv+ vn;
	
	while ( --vn >= 0 )
		{
		/* switch real part */
		hold = *vv;
		*(vv++) = *vvend;
		*vvend = hold;
		--vvend;
		}
	return (*this);		/* Vector::reverse() */
	}
// ----------------- end VECRVRS.CPP --------------