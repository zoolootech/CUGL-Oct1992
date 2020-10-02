// vector.cpp - trial vector program to iron out C++  8/8/90

#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include "wtwg.h"

#include "dblib.h"

#include "vector.h"



	Vector::Vector(int k)			// constructor
		{
		int nb = k*sizeof(float);
		if ( k>0 )
			{
			n=k;
			v=(float*)wmalloc(nb, "Vector");
			memset ( v, 0 ,nb );
			}
		else
			{
			v=NULL;			// allow zero length vectors
			n=0;
			}
		}

	Vector::Vector( Vector& vec)		// copy constructor
		{
		n = vec.n;
		int nb =(n*sizeof(float));
		v =(float*)wmalloc(nb, "Vector");
		memcpy ( v, vec.v, nb );
		}

	Vector::~Vector() { if ( n>0 ) free (v); }

	void Vector::realloc ( int nitems )
		// change size of a vector.
		{
		int old_nb = n * sizeof (float);
		float *old_v = v;
		
		n = nitems;
		if ( n>0 )
			{
			register int new_nb = nitems * sizeof (float);
			v = (float *)wmalloc ( new_nb, "Vector");
			memset ( v, 0, new_nb );
			memcpy ( v, old_v, min ( new_nb, old_nb ) );
			}
		else
			{
			v =NULL;
			}
		if ( old_nb > 0 ) free (old_v);
		}

	Vector& Vector::operator=(Vector& vec)		// assign one V to another
		{
		int  vecn = vec.n;
		
		if ( n == 0 )  realloc (vecn);		// if target is zero len...
	
		int vn = n;
		
		int nt  = min ( vn, vecn );
		
		float *vv = v;
		memcpy ( vv, vec.v, nt*sizeof(float) );
		
		// if target longer than source, fill in remainder with zeros.
		while ( nt++ < vn )		
			{
			vv[nt] = 0;;
			}
		
		return *this;
		}


	Vector& Vector::operator=(float x)
		// assign float x to vector
		{
		int vn =n;
		float *vv =v;
		while ( --vn >= 0 )
			{
			vv[vn] = x;
			}
		return *this;
		}


	Vector& Vector::operator+=(Vector&  x)
		// add Vector x to Vector this.
		{
		int  k = min ( x.n, n );	// use smaller Vector for range.
		float *tv = v, *xv = x.v;
		while ( --k >= 0 )
			{
			tv[k] += xv[k];
			}
		return *this;
		}


	Vector& Vector::operator+=(float  x)
        // add float x to Vector this.
		{
		int k =n;
		float *tv = v;
		while ( --k >= 0 )
			{
			tv[k] += x;
			}
		return *this;
		}


	Vector& Vector::operator*=(Vector&  x)
		// multiply Vector this by Vector x.
		{
		int  k = min ( x.n, n );	// use smaller Vector for range.
		float *tv = v,  *xv =x.v;
		while  ( --k >= 0 )
			{
			tv[k] *= xv[k];
			}
		return *this;
		}


	Vector& Vector::operator*=(float  x)
		// Multiply Vector this by float x
		{
		int k =n;
		float *tv =v;
		while ( --k >= 0 )
			{
			tv[k] *= x;
			}
		return *this;
		}

	Vector& Vector::operator-=(Vector&  x)
		// subtract Vector x from Vector this.
		{
		int  k = min ( x.n, n );	// use smaller Vector for range.
		float *tv =v, *xv =x.v;
		while ( --k >= 0 )
			{
			tv[k] -= xv[k];
			}
		return *this;
		}


	Vector& Vector::operator-=(float  x)
		// Subtract float x from Vector this
		{
		int k =n;
		float *tv =v;
		while ( --k >= 0 )
			{
			tv[k] -= x;
			}
		return *this;
		}

	Vector& Vector::operator/=(float  x)
		// Divide Vector x by float x. NOTE prevents divide by zero.
		{
		int k =n;
		float *tv =v;
		if ( x > -MINFLOAT*10  && x< MINFLOAT*10 )
			{
			while ( --k >= 0 )
				{
				tv[k] = MAXFLOAT/10;
				}
			}
		else
			{
			while ( --k >= 0 )
				{
				tv[k] /= x;
				}
			}
		return *this;
		}


//--------------------  end of VECTOR.CPP -------------------- 

