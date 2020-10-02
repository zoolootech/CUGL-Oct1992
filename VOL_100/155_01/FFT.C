/*	
HEADER:
TITLE:		Fast Fourier Transform;
DATE:		05/18/1985;
DESCRIPTION:	"Performs fast fourier transform using method described
		by E. O. Brigham.  For details of the method, refer
		to Brigham's book. THE FAST FOURIER TRANSFORM";	
KEYWORDS: 	Fourier, transform;
FILENAME:	FFT.C;
WARNINGS:	
  "This program is self-contained, all that is needed is a manner of getting
  the data into the array real_data (& imag_data, if applicable).  The 
  transformed data will reside in these two arrays upon return with the 
  original data being destroyed."
AUTHORS:	Jim Pisano;
COMPILERS:	DeSmet;
REFERENCES:	AUTHORS:	E. O. Brigham;
		TITLE:		"THE FAST FOURIER TRANSFORM";
		CITATION:	"";
	ENDREF
*/

/*	file name fft.c
*	program name fft() ... Fast Fourier Transform
*
*	Perform fast fourier transform using method described by E. O. Brigham.
*  For details of the method, refer to Brigham's book 
*
*	Translated to C from FORTRAN by
*
*		Jim Pisano
*		P.O. Box 3134
*		University Station
*		Charlottesville, VA 22903
*
*  This program is in the public domain & may be used by anyone for commercial
*  or non-commercial purposes.
*
*  real_data ... ptr. to real part of data to be transformed
*  imag_data ... ptr. to imag  "   "   "   "  "      "
*  inv ..... Switch to flag normal or inverse transform
*  n_pts ... Number of real data points
*  nu ...... logarithm in base 2 of n_pts e.g. nu = 5 if n_pts = 32. 
*
*  This program is self-contained, all that is needed is a manner of getting
*  the data into the array real_data (& imag_data, if applicable).  The 
*  transformed data will reside in these two arrays upon return with the 
*  original data being destroyed.
*/

#include	<stdio.h>
#include	<math.h>		    /* declare math functions to use */
#define		PI		3.1419527

fft( real_data, imag_data, n_pts, nu, inv )
double *real_data, *imag_data;
int n_pts, nu, inv;
{
	int n2, j, j1, l, i, ib, k, k1, k2;
	int sgn;
	double tr, ti, arg, nu1;	/* intermediate values in calcs. */
	double c, s;	       /* cosine & sine components of Fourier trans. */

	n2 = n_pts / 2;
	nu1 = nu - 1.0;
	k = 0;
/* 
* sign change for inverse transform 
*/
	sgn = inv ? -1 : 1;		
/*
* Calculate the componets of the Fourier series of the function
*/
	for( l = 0; l != nu; l++ )
	{
		do
		{
			for( i = 0; i != n2; i++ )
			{	
				j = k / ( pow( 2.0, nu1 ) );
				ib = bit_swap( j, nu );
				arg = 2.0 * PI * ib / n_pts;
				c = cos( arg );
				s = sgn * sin( arg );
				k1 = k;
				k2 = k1 + n2;
				tr = *(real_data+k2) * c + *(imag_data+k2) * s;
				ti = *(imag_data+k2) * c - *(real_data+k2) * s;
				*(real_data+k2) = *(real_data+k1) - tr;
				*(imag_data+k2) = *(imag_data+k1) - ti;
				*(real_data+k1) = *(real_data+k1) + tr;
				*(imag_data+k1) = *(imag_data+k1) + ti;
				k++;
			}
			k +=  n2;
		} while( k < n_pts - 1);
		k = 0;
		nu1 -= 1.0;
		n2 /= 2;
	}	
	for( k = 0; k != n_pts; k++ )
	{
		ib = bit_swap( k, nu );
		if( ib > k)
		{
			swap( (real_data+k), (real_data+ib) );
			swap( (imag_data+k), (imag_data+ib) );
		}
	}
/* 
* If calculating the inverse transform, must divide the data by the number of
* data points.
*/
	if( inv )
		for( k = 0; k != n_pts; k++)
		{
			*(real_data+k) /= n_pts;
			*(imag_data+k) /= n_pts;
		} 
}
/* 
* Bit swaping routine in which the bit pattern of the integer i is reordered.
* See Brigham's book for details
*/
bit_swap( i, nu )
int i, nu;
{
	int ib, i1, i2;

	ib = 0;

	for( i1 = 0; i1 != nu; i1++ )
	{
		i2  = i / 2;
		ib = ib * 2 + i - 2 * i2;
		i   = i2;
	}
	return( ib );
}
/*
* Simple exchange routine where *x1 & *x2 are swapped
*/
swap( x1, x2 )	
double *x1, *x2;
{
	double *temp_x;
 
	*temp_x = *x1;
	*x1 = *x2;
	*x2 = *temp_x;
}