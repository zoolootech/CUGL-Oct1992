/* Vector multiplication

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

typedef double HOMO [CMAX];

void    vec_mul (VERTEX *this_vec, MATRIX this_mat, VERTEX *prod)

/* Multiply a vertex in homogeneous coordinates by a 4x4 matrix.

Points in 3-space are converted to homogeneous coordinates prior to
transformation.  This consists simply of adding a fourth coordinate
w = 1.0, called the scale factor:

                    [x y z] -> [wx wy wz w]

Mathematically, the value of w is not important; 1.0 is chosen for
convenience.  To convert back to world coordinates, divide each coordinate
by w and drop the last coordinate.

The value of w does not change as a result of scaling, translation, or
rotation, but it does change as a result of perspective transformation.

Given a vector v in homogeneous coordinates and a matrix m, each coordinate
of the product p[j] is given by the dot product of v and column j of m,
that is:

                     p [j] = v [0] * m [0] [j]
                           + v [1] * m [1] [j]
                           + v [2] * m [2] [j]
                           + v [3] * m [3] [j]

*/

{
    HOMO tvec,pvec;                /* tvec is temporary storage for the
                                      vertex to be multiplied, pvec is the
                                      product. */
    int cc,cl;

    for (cc = 0; cc < DIM; cc++)                    /* Copy this_vec to
                                                       temp storage */
    tvec [cc] = this_vec -> coord [cc];
    tvec [CMAX - 1] = 1.0;                          /* Convert to homo-
                                                       geneous coordinates */

    for (cl = 0; cl < CMAX; cl++)                   /* Outside loop to
                                                       calculate each coor-
                                                       dinate */
    {
        pvec [cl] = 0.0;
        for (cc = 0; cc < CMAX; cc++)               /* Inside loop to cal-
                                                       culate dot product */
        pvec [cl] = pvec [cl]
                       + tvec [cc] * this_mat [cc] [cl];
    }
    for (cc = 0; cc < DIM; cc++)                    /* Convert back to world
                                                       coordinates */
        prod -> coord [cc] = pvec [cc]/pvec [CMAX - 1];
}
