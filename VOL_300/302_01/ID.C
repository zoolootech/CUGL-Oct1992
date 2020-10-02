/* Create identity matrix

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

void    identity (MATRIX this_mat)

/* Initialize the transformation matrix to the identity matrix,
i.e., all ones on the diagonal.  A matrix is initially created using
the following code:

                   MATRIX example;
                   identity (example);

The result is:

              |  1.0  0.0  0.0  0.0  |
              |  0.0  1.0  0.0  0.0  |
              |  0.0  0.0  1.0  0.0  |
              |  0.0  0.0  0.0  1.0  |

A vertex multiplied by the identity matrix is unchanged.
*/

{
    int ri,ci;/* Row and column counters */

    for (ri = 0; ri < RMAX; ri++)
        for (ci = 0; ci < CMAX; ci++)
            if (ri == ci)                 /* Row index == column index, so
                                             set element to 1.0 */
                this_mat [ri] [ci] = 1.0;
            else
                this_mat [ri] [ci] = 0.0; /* Otherwise, set off diagonals
                                             to 0.0. */
}
