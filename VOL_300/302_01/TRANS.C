/* Add translation to transformation matrix

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

void    trans (double tx, double ty, double tz, MATRIX this_mat)

/* Add translation to the transformation matrix.

Vertices may be translated in any direction, given by [tx ty tz].  The
result of the transformation is

          [x y z] -> [(x + tx) (y + ty) (z + tz)]

The matrix created for the translation transformation is

           |  1.0  0.0  0.0  0.0  |
           |  0.0  1.0  0.0  0.0  |
           |  0.0  0.0  1.0  0.0  |
           |  tx   ty   yz   1.0  |

The current transformation matrix this_mat is then multiplied by the trans-
lation transformation matrix, thus concatenating the translation operation
with the current transformation.
*/
{
    MATRIX t_mat;

    identity (t_mat);
    t_mat [3] [0] = tx;
    t_mat [3] [1] = ty;
    t_mat [3] [2] = tz;
    mat_mul (this_mat,t_mat,this_mat);
}
