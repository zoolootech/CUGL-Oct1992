/* Calculate dot product

   Copyright (c) 1988 by Gus O'Donnell

   Revision history:

   Version 1.00         February 29, 1988       As released.

   Version 1.01         March 20, 1988          Created libraries for all
                                                memory models

*/
#include <3d.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

void    normalize (VECTOR this_vec)

/* Normalize a vector to unit magnitude.  Calculate the magnitude as:

               magnitude = sqrt ( x^2 + y^2 + z^2 )

Normalize the vector by dividing each coordinate by the magnitude.

FOR loops are used to maintain generality--any dimension vector may
be normalized by changing DIM. */

{
    double mag;
    int count;

    mag = 0.0;
    for (count = 0; count < DIM; count++)
        mag = mag + (this_vec [count] * this_vec [count]);
    mag = sqrt(mag);
    for (count = 0; count < DIM; count++)
        this_vec [count] = this_vec [count]/mag;
}

double  dot_prod (VECTOR vec1, VECTOR vec2)

/* Calculate the cosine of the angle between two vectors.  First, normalize
the vectors, then calculate the dot product as:

             dot product = (x1 * x2) + (y1 * y2) + (z1 * z2)

*/

{
    double COSINE;
    int count;
    VECTOR tvec1,tvec2;   /* Temporary storage is used to avoid changing
                             the arguments passed to the function. */

    for (count = 0; count < DIM; count++)  /* Copy arguments to temporary */
    {                                      /* storage. */
        tvec1 [count] = vec1 [count];
        tvec2 [count] = vec2 [count];
    }
    normalize (tvec1);
    normalize (tvec2);
    COSINE = 0.0;

    for (count = 0; count < DIM; count++)
        COSINE = COSINE + tvec1 [count] * tvec2 [count];
    return (COSINE);
}

