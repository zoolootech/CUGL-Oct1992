/* Matrix multiplication

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

void    assign_mat(MATRIX mat1, MATRIX mat2)

/* Copy mat2 to mat1. */

{
    int rcount,ccount;

    for (rcount = 0; rcount < RMAX; rcount++)
        for (ccount = 0; ccount < CMAX; ccount++)
            mat1 [rcount] [ccount] = mat2 [rcount] [ccount];
}

void    mat_mul (MATRIX mat1, MATRIX mat2, MATRIX prod)

/* Multiply two matrices.  An element of the product matrix prod [i] [j]
is the dot product of row i of mat1 and column j of mat2, that is:

           prod [i] [j] = mat1 [i] [0] * mat2 [0] [j]
                        + mat1 [i] [1] * mat2 [1] [j]
                        + mat1 [i] [2] * mat2 [2] [j]
                        + mat1 [i] [3] * mat2 [3] [j]

The matrices must be conformal, i.e., the number of columns of mat1
must equal the number of rows of mat2.  Since all transformation
matrices are 4x4, they are always conformal.  Notice that the operation
is not commutative--in general, mat1 * mat2 != mat2 * mat1.  This has
important implications for 3D transforms, for example, a rotation about
[0,0,0], followed by a translation is not the same as a translation
followed by a rotation.  */

{
    int i,j,p;  /* i indexes the row of the product matrix,
                   j indexes the column,
                   p indexes the product term while
                   calculating the dot product. */
    MATRIX result;

    for (i = 0; i < RMAX; i++)
        for (j = 0; j < CMAX; j++)
        {
            result [i] [j] = 0.0;
            for (p = 0; p < RMAX; p++)
                result [i] [j] = result [i] [j]
                               + mat1 [i] [p] * mat2 [p] [j];
        }
    assign_mat (prod,result);
}
