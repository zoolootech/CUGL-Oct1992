//  VECPHCNT.CPP
//			routines for polar vector phase adjustment.
//			applies to complex vectors.

#include <stdlib.h>
#include "wtwg.h"
#include "dblib.h"
#include "vector.h"


void PhaseCenter ( complex &z , float r)
	{
	float r_end = r+ 2*PI;
	float phase = imag (z);
	
	while ( phase >= r_end ) phase -= 2*PI ;
	while ( phase <  r     )  phase += 2*PI ;
	z = complex ( real(z), phase );	
	return;
	}




/* CVector::PhaseCenter ( float NewRange )
 *	routine to adjust the phase part of a polar vector
 *      to the range NewRange to NewRange+2*PI
 *
 *	default NewRange is -PI
 *
 */
void CVector::PhaseCenter ( float  r )
	{
	if ( ! ispolar ) return;

	int vn = y.n;
	float *vy = y.v;
	complex pz;
	
	while ( --vn >= 0 )
		{
		pz = complex ( 0, *vy );
		::PhaseCenter ( pz, r );		// note scope resolution to global
		*(vy++)= imag ( pz );
		}

	return;	/* CVector::PhaseCenter () */
	}


// ----------------- end of VECPHCNT.CPP -------------------------