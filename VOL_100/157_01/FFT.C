/* 
HEADER: 	CUG
TITLE:		FFT.C - Fast Fourier Transform
VERSION:	1.00
DATE:		05/27/85
DESCRIPTION:	"A Fast Fourier Transform implementation based on
		Cooley's successive-doubling method."
KEYWORDS:	fft, filter, fourier, transform
SYSTEM:		Any
FILENAME:	FFT.C
WARNINGS:	"Complex numbers are represented by their real
		and imaginary components in a 2-D array. Data
		must be presented in multiples of two."
CRC:		xxxx
SEE-ALSO:	FWT.C
AUTHORS:	Ian Ashdown - byHeart Software
COMPILERS:	Any C compiler
REFERENCES:	AUTHORS: R.F. Gonzalez & P. Wintz;
		TITLE:	 Digital Image Processing;
		AUTHORS: J. Cooley, P. Lewis & P. Welch;
		TITLE:	 The Fast Fourier Transform and Its
			 Applications
			 IEEE Trans. Educ.
			 pp. 27 - 34, Vol. E-12, No. 1, 1969;
		AUTHORS: A. Aho, J. Hopcroft & J. Ullman;
		TITLE:	 The Design and Analysis of Computer
			 Algorithms
			 Chapter 7, The Fast Fourier Transform
			 and Its Applications;
ENDREF
*/

/*-------------------------------------------------------------*/

#include "math.h"

typedef struct complex	/* Complex number structure */
{
  double real,
	 imag;
} COMPLEX;

#define PI  3.1415926536

/* FFT() - Fast Fourier Transform accepts a two dimensional
 *	   "double" array of two rows of N+1 elements, where the
 *	   zeroth column elements are not used and N is equal to
 *	   an integer power of two, and a variable n, set equal
 *	   to N above. The discrete Fourier transform is computed
 *	   in place and returned in the same array.
 */

void fft(n,point)
int n;
COMPLEX point[];
{
  register int a,
	       b;
  int d = 1,
      e,
      f;
  COMPLEX u,
	  w,
	  temp;
  void c_bitrev();

  /* Reorder complex data vector by bit reversal rule */

  c_bitrev(n,point);

  /* Perform successive doubling calculations */

  while(d < n)
  {
    e = d;
    d *= 2;
    u.real = 1.0;
    u.imag = 0.0;
    w.real = cos(PI/e);
    w.imag = -sin(PI/e);
    for(b = 1; b <= e; b++)
    {
      for(a = b; a <= n; a += d)
      {
	f = a + e;
	temp.real = point[f].real * u.real -
	    point[f].imag * u.imag;
	temp.imag = point[f].real * u.imag +
	    point[f].imag * u.real;
	point[f].real = point[a].real - temp.real;
	point[f].imag = point[a].imag - temp.imag;
	point[a].real = point[a].real + temp.real;
	point[a].imag = point[a].imag + temp.imag;
      }
      temp.real = u.real;
      u.real = u.real * w.real - u.imag * w.imag;
      u.imag = temp.real * w.imag + u.imag * w.real;
    }
  }

  /* Normalize transformed data vector */

  for(a = 1; a <= n; a++)
  {
    point[a].real = point[a].real/n;
    point[a].imag = point[a].imag/n;
  }
}

/* IFFT() - Inverse Fast Fourier Transform accepts a two
 *	    dimensional "double" array of two rows of N+1
 *	    elements, where the zeroth column elements are not
 *	    used and N is equal to an integer power of two, and a
 *	    variable n, set equal to N above. The inverse
 *	    discrete Fourier transform is computed in place and
 *	    returned in the same array. 
 */

void ifft(n,point)
int n;
COMPLEX point[];
{
  register int a,
	       b;
  int d = 1,
      e,
      f;
  COMPLEX u,
	  w,
	  temp;
  void c_bitrev();

  /* Reorder complex data vector by bit reversal rule */

  c_bitrev(n,point);

  /* Perform successive doubling calculations */

  while(d < n)
  {
    e = d;
    d *= 2;
    u.real = 1.0;
    u.imag = 0.0;
    w.real = cos(PI/e);
    w.imag = sin(PI/e);
    for(b = 1; b <= e; b++)
    {
      for(a = b; a <= n ; a += d)
      {
	f = a + e;
	temp.real = point[f].real * u.real -
	    point[f].imag * u.imag;
	temp.imag = point[f].real * u.imag +
	    point[f].imag * u.real;
	point[f].real = point[a].real - temp.real;
	point[f].imag = point[a].imag - temp.imag;
	point[a].real = point[a].real + temp.real;
	point[a].imag = point[a].imag + temp.imag;
      }
      temp.real = u.real;
      u.real = u.real * w.real - u.imag * w.imag;
      u.imag = temp.real * w.imag + u.imag * w.real;
    }
  }
}

/* C_BITREV() - Reorder complex data vector by bit reversal
 *		rule.
 */ 

void c_bitrev(n,point)
int n;
COMPLEX point[];
{
  register int i,
	       j,
	       k;
  COMPLEX temp;

  j = n/2 + 1;
  for(i = 2; i < n; i++)
  {
    if(i < j)
    {
      temp.real = point[i].real;
      temp.imag = point[i].imag;
      point[i].real = point[j].real;
      point[i].imag = point[j].imag;
      point[j].real = temp.real;
      point[j].imag = temp.imag;
    }
    k = n/2;
    while(k < j)
    {
      j = j - k;
      k /= 2;
    }
    j = j + k;
  }
}

/* End of FFT.C */
.imag;
	temp.imag = po