//////// COMPLEX VECTORS
//
//		primary subroutines to support complex vector math :
//		assignment, addition, and mutliplication.
//
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"



	CVector& CVector::operator=(CVector& vec)
		// assign  CVector to Cvector
		{
		ispolar = vec.ispolar;
		x = vec.x;
		y = vec.y;
		return *this;
		}

	CVector& CVector::operator=(Vector& vec)
		// assign real V. to CV
		{
		x = vec;
		y =0;
		ispolar = 0;	
		return *this;
		}

	CVector& CVector::operator=(float f)
		// assign float f to CVvector
		{
		x =f;
		y =0;
		ispolar =0;
		return *this;
		}


	CVector& CVector::operator=(complex z)
		// assign comlex z to CVvector
		{
		x = real (z);
		y = imag (z);
		ispolar = 0;
		return *this;
		}


	CVector& CVector::operator+=(CVector&  vec)
		// add CVector vec to CVector this.
		{
		if ( ispolar || vec.ispolar )
			{
			werror ( 'V', "ADDING POLAR VECTORS" );
			}
		
		x += vec.x;
		y += vec.y;
		
		return *this;
		}

	CVector& CVector::operator+=(Vector&  vec)
		// add real Vector vec to CVector this.
		{
		if ( ispolar )
			{
			werror ( 'V', "ADDING POLAR VECTORS" );
			}
		x += vec;
		return *this;
		}


	CVector& CVector::operator+=(float  f)
		// add float f to Vector this.
		{
		if ( ispolar ) werror ( 'V', "ADDING POLAR VECTORS" );
		
		x += f;
		
		return *this;
		}

	CVector& CVector::operator+=(complex z)
		// add complex z to Vector this.
		{
		if ( ispolar ) werror ( 'V', "ADDING POLAR VECTORS" );
		
		x += real (z);
		y += imag (z);
		
		return *this;
		}


	CVector& CVector::operator*=(CVector&  z)
		// multiply CVector this by CVector z.
		{
		if ( ispolar )
			{
			if ( z.ispolar )
				{
				x*= z.x;		// mult mags, add phases
				y+= z.y;
				}
			else	werror ('V', "VECTOR MULTIPLY mixing rect and polar");
			}
		else
			{
			if ( ! z.ispolar )
				{
				x *= z.x;
				y *= z.y;
				}
			else	werror ('V', "VECTOR MULTIPLY mixing rect and polar");
			}
		return *this;
		}


	CVector& CVector::operator*=(float  f)
		// Multiply CVector this by float f
		// note works for either polar or rect.
		{
		
		x *= f;
		if ( ! ispolar )  y *= f;
		else  this->rectify();		// make sure mag.s are all positive.
		return *this;
		}


	CVector& CVector::operator*=(complex z)
		// Multiply CVector this by scalar complex z
		{
		if ( ispolar ) 
			{
			float  zmag = norm (z);
			x *= ( ( zmag > SMALL_VAL ) ? sqrt(zmag) : 2*zmag );
			y += arg (z);
			}
		else
			{
			x *= real (z);		// rect. form of mutilply.
			y *= imag (z);		
			}	
		return *this;
		}
//----------------- end VECZ.CPP-------------------------	
