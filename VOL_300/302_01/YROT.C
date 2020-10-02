/* Add rotation about y axis to transformation matrix

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

void    yrot (double theta, MATRIX this_mat)

/* Add rotation to the transformation matrix.

Vertices may be rotated about any axis.  Note that y rotation is about
the vector j = [0 1 0].  If a rotation about a point local to the object
is desired, the point (and the entire object) must first be translated
to [0 0 0], the rotation performed, and the translation reversed.  This
is accomplished by concatenating these three transformations.

The end result of the rotation is

   [x y z] -> [(x cos theta - z sin theta) y (z cos theta + x sin theta)]

The rotation is clockwise as viewed along the axis of rotation from
the positive direction.  The angle is expressed in radians.

The matrix created for the scaling transformation is

           |   cos theta  0.0  sin theta  0.0  |
           |   0.0        1.0  0.0        0.0  |
           |  -sin theta  0.0  cos theta  0.0  |
           |   0.0        0.0  0.0        1.0  |

The current transformation matrix this_mat is then multiplied by the rotation
transformation matrix, thus concatenating the rotation operation with the
current transformation.
*/

{
    MATRIX yr_mat;

    identity (yr_mat);
    yr_mat [0] [0] = cos(theta);
    yr_mat [2] [0] = -sin(theta);
    yr_mat [0] [2] = sin(theta);
    yr_mat [2] [2] = cos(theta);
    mat_mul (this_mat,yr_mat,this_mat);
}
