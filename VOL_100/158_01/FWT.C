/* 
HEADER: 	CUG
TITLE:		FWT.C - Fast Walsh Transform
VERSION:	1.00
DATE:		05/27/85
DESCRIPTION:	"A Fast Walsh Transform implementation based on
		Cooley's successive-doubling method. See the
		September '77 issue of BYTE for a description of
		this alternative to the Fourier transform."
KEYWORDS:	fwt, fft, filter, walsh, fourier, transform
SYSTEM:		Any
FILENAME:	FWT.C
WARNINGS:	"Data must be presented in multiples of two."
CRC:		xxxx
SEE-ALSO:	FFT.C
AUTHORS:	Ian Ashdown - byHeart Software
COMPILERS:	Any C compiler
REFERENCES:	AUTHORS: R.F. Gonzalez & P. Wintz;
		TITLE:	 Digital Image Processing;
		AUTHORS: B. Jacobi;
		TITLE:	 Walsh Functions: A Digital Fourier
			 Series
			 pp. 190 - 198, BYTE, September 1977;
		AUTHORS: J. Cooley, P. Lewis & P. Welch;{
		TITLE:	 The Fast Fourier Transform and Its
			 Applications
			 IEEE Trans. Educ.
			 pp. 27 - 34, Vol. E-12, No. 1, 1969;
ENDREF
*/

/*-------------------------------------------------------------*/

#include "math.h"

typedef double REAL;

/* FWT() - Fast Walsh Transform accepts a one-dimensional
 *	   "double" array of one row of N+1 elements, where the
 *	   zeroth column element is not used and N is equal to
 *	   an integer power of two, and a variable n, set equal
 *	   to N above. The discrete Walsh transform is computed
 *	   in place and returned in the same array.
 */

void fwt(n,point)
int n;
REAL point[];
{
  register int a,
	       b,
	       e;
  int c = 1,
      d;
  REAL temp;
  void r_bitrev();

  /* Reorder real data vector by bit reversal rule */

  r_bitrev(n,point);

  /* Perform successive doubling calculations */

  while(c < n)
  {
    d = c;
    c *= 2;
    for(b = 1; b <= d; b++)
      for(a = b; a <= n ; a += c)
      {
	e = a + d;
	temp = point[e];
	point[e] = point[a] - temp;
	point[a] = point[a] + temp;
      }
  }

  /* Normalize transformed data vector */

  for(a = 1; a <= n; a++)
    point[a] = point[a]/n;
}

/* IFWT() - Inverse Fast Walsh Transform accepts a one-dimensional
 *	    "double" array of one row of N+1 elements, where the
 *	    zeroth column element is not used and N is equal to
 *	    an integer power of two, and a variable n, set equal
 *	    to N above. The discrete Walsh transform is computed
 *	    in place and returned in the same array. (The
 *	    algorithm is identical to the Fast Walsh Transform,
 *	    except that the normalization of the transformed data
 *	    vector is not performed.)
 */

void ifwt(n,point)
int n;
REAL point[];
{
  register int a,
	       b,
	       e;
  int c = 1,
      d;
  REAL temp;
  void r_bitrev();

  /* Reorder real data vector by bit reversal rule */

  r_bitrev(n,point);

  /* Perform successive doubling calculations */

  while(c < n)
  {
    d = c;
    c *= 2;
    for(b = 1; b <= d; b++)
      for(a = b; a <= n ; a += c)
      {
	e = a + d;
	temp = point[e];
	point[e] = point[a] - temp;
	point[a] = point[a] + temp;
      }
  }
}

/* R_BITREV() - Reorder real data vector by bit reversal rule */

void r_bitrev(n,point)
int n;
REAL point[];
{
  register int i,
	       j,
	       k;
  REAL temp;

  j = n/2 + 1;
  for(i = 2; i < n; i++)
  {
    if(i < j)
    {
      temp = point[i];
      point[i] = point[j];
      point[j] = temp;
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

/* End of FWT.C */
ansformed data
 *	    vector is not performed.)
 */

void ifwt(n,point)