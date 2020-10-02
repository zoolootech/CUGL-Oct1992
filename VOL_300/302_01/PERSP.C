/* Add perspective to transformation matrix

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

void    persp (double s, double d, double f, MATRIX this_mat)

/* Add perspective to the transformation matrix.

Unlike other transformations, the perspective transformation alters the
value of the scale factor w.  Converting to world coordinates involves
dividing by a factor other than unity.  This has the effect of dividing
the values of the coordinates by the depth, causing objects further away
to appear smaller.

The factors required for the perspective transformation are

                     s = Viewing screen size
                     d = Viewing screen depth
                     f = Viewing pyramid depth

The eye lies at the apex of a pyramid; the object is transformed to
coordinates lying inside this pyramid.  The base of the pyramid is
at depth f, and the viewing screen is defined by the cross section of
the pyramid at depth d.

The matrix created for the perspective transformation is

                   |  1.0  0.0   0.0             0.0  |
                   |  0.0  1.0   0.0             0.0  |
                   |  0.0  0.0   s/[d(1 - d/f)]  s/d  |
                   |  0.0  0.0  -s/(1 - d/f)     0.0  |

The current transformation matrix this_mat is then multiplied by the perspective
transformation matrix, thus concatenating the perspective operation with the
current transformation.
*/

{
    MATRIX p_mat;

    identity (p_mat);
    p_mat [2] [2] = s/(d * (1 - (d/f)));
    p_mat [2] [3] = s/d;
    p_mat [3] [2] = -s/(1 - (d/f));
    p_mat [3] [3] = 0;
    mat_mul (this_mat,p_mat,this_mat);
}