/* Display face on screen

   Copyright (c) 1988 by Gus O'Donnell

   Revision history:

   Version 1.00         February 29, 1988       As released.

   Version 1.01         March 20, 1988          Created libraries for all
                                                memory models

*/
#include <3d.h>
#include <float.h>
#include <graphics.h>
#include <math.h>
#include <stdio.h>

void    disp_face (VECTOR lsource, int color, FACE *this_face, MATRIX xfrm_mat)

/* Display a face on the screen using the current transform matrix.

The shade of the face is determined by the angle between the light source,
lsource, and the normal to the face.  The displayed shade is proportional
to the cosine of this angle.  A temporary face is constructed from the
untransformed face and the transform matrix.  The normal is calculated.
If the z component of the normal is positive, the face is not displayed
(the assumption is that the face is on the far side of the object).

This function will display a polygon with a maximum of 10 corners
*/

{
    VECTOR tnorm;               /* Normal to the transformed face */
    CORNER *chandle;            /* Pointer for traversing the corner
                                   list */
    VERTEX tvec;                /* Temporary storage for vec_mul result */
    double angle;               /* Cosine of angle between tnorm and
                                   lsource */
    int vertices [22];          /* Corner list */
    int ncorners;               /* Number of corners in the face */
    char pat [8];               /* Shaded fill pattern */
    int bcount,ccount,incr;     /* Counters to create fill pattern */
    int err;

    err = normal (this_face,tnorm);
    if (err != 0) return;              /* Return if error */
    if ((tnorm [2]) <= 0.0) return;	   /* Don't display if facing away */
    ncorners = 0;
    chandle = this_face -> first -> next;
    while (((chandle -> next) != NULL) && (ncorners < 10))
    {
        vec_mul (chandle -> this,xfrm_mat,&tvec);
        vertices [ncorners * 2] = tvec.coord [0];
        vertices [ncorners * 2 + 1] = tvec.coord [1];
        chandle = chandle -> next;
        ncorners++;
    }
    vertices [ncorners * 2] = vertices [0];
    vertices [ncorners * 2 +1] = vertices [1];
    angle = 100 * dot_prod (tnorm,lsource);
    incr = 0;
    for (ccount = 0; ccount < 8; ccount++)
    {
        pat [ccount] = 0;
        for (bcount = 0; bcount < 8; bcount++)
        {
            pat [ccount] = pat [ccount] * 2;
            incr = incr + angle;
            if (incr >= 100)
            {
                pat [ccount]++;
                incr = incr - 100;
            }
        }
    }
    setfillpattern (pat,color);
    setfillstyle (12,color);
    fillpoly(ncorners,vertices);
}
