/* Add rotation about z axis to transformation matrix

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

void    zrot (double theta, MATRIX this_mat)

/* Add rotation to the transformation matrix.

Vertices may be rotated about any axis.  Note that z rotation is about
the vector k = [0 0 1].  If a rotation about a point local to the object
is desired, the point (and the entire object) must first be translated
to [0 0 0], the rotation performed, and the translation reversed.  This
is accomplished by concatenating these three transformations.

The end result of the rotation is

   [x y z] -> [(x cos theta + y sin theta) (y cos theta - x sin theta) z]

The rotation is clockwise as viewed along the axis of rotation from
the positive direction.  The angle is expressed in radians.

The matrix created for the scaling transformation is

           |  cos theta  -sin theta  0.0  0.0  |
           |  sin theta   cos theta  0.0  0.0  |
           |  0.0         0.0        1.0  0.0  |
           |  0.0         0.0        0.0  1.0  |

The current transformation matrix this_mat is then multiplied by the rotation
transformation matrix, thus concatenating the rotation operation with the
current transformation.
*/
{
    MATRIX zr_mat;

    identity (zr_mat);
    zr_mat [0] [0] = cos(theta);
    zr_mat [0] [1] = -sin(theta);
    zr_mat [1] [0] = sin(theta);
    zr_mat [1] [1] = cos(theta);
    mat_mul (this_mat,zr_mat,this_mat);
}
