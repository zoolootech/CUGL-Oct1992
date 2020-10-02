/* Add scaling to transformation matrix

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

void    scale (double sx, double sy, double sz, MATRIX this_mat)

/* Add scaling to the transformation matrix.

Each coordinate may be scaled independently.  The result of the transformation
is

               [x y z] -> [(sx * x) (sy * y) (sz * z)]

The matrix created for the scaling transformation is

           |  sx   0.0  0.0  0.0  |
           |  0.0  sy   0.0  0.0  |
           |  0.0  0.0  sz   0.0  |
           |  0.0  0.0  0.0  1.0  |

The current transformation matrix this_mat is then multiplied by the scaling
transformation matrix, thus concatenating the scaling operation with the
current transformation.
*/

{
    MATRIX s_mat;

    identity (s_mat);
    s_mat [0] [0] = sx;
    s_mat [1] [1] = sy;
    s_mat [2] [2] = sz;
    mat_mul (this_mat,s_mat,this_mat);
}
