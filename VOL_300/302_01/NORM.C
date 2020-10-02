/* Calculate normal to face

   Copyright (c) 1988 by Gus O'Donnell

   Revision history:

   Version 1.00         February 29, 1988       As released.

   Version 1.01         March 20, 1988          Created libraries for all
                                                memory models

*/
#include <3d.h>
#include <float.h>
#include <stdio.h>
#include <math.h>

int     normal (FACE *this_face, VECTOR norm)

/* Calculate the normal to the face.  The normal is calculated as the
cross product of two sides of the face.  The face must have at least
three corners.  The first three corners must not be colinear.  Since
each corner added to the face using add_corner is checked for colinearity,
this check is not performed here.  The function also assumes that the
face is a convex polygon; if the first two sides of the face are at an
angle greater than 180 degrees (i.e., the face is a concave polygon), the
normal will have the wrong sign.  Finally, the function assumes that the
corners are listed in clockwise order, viewing the face from the outside
of the solid.

Given two vectors

                 q = [Ai + Bj + Ck],
                 r = [Di + Ej + Fk]

where i, j, and k are orthogonal unit vectors, the cross product p = q X r
is given by:

                 p = [(B*F) - (C*E)]i
                   + [(C*D) - (A*F)]j
                   + [(A*E) - (B*D)]k

If the first three corners of the face are a, b, and c, the components of
the vectors forming the first two sides q and r are

                 A = a [0] - b [0]
                 B = a [1] - b [1]
                 C = a [2] - b [2]
                 D = c [0] - b [1]
                 E = c [1] - b [2]
                 F = c [2] - b [2]

Note that the components are calculated such that corner b corresponds
to the tail of the two vectors.  This yields a normal vector p = q X r
with a direction away from the interior of the solid.  Substituting
the expressions for A through F into the expression for the cross product
gives:

                 p = [(a [1] - b [1])*(c [2] - b [2])
                   -  (a [2] - b [2])*(c [1] - b [1])]i
                   + [(a [2] - b [2])*(c [0] - b [0])
                   -  (a [0] - b [0])*(c [2] - b [2])]j
                   + [(a [0] - b [0])*(c [1] - b [1])
                   -  (a [1] - b [1])*(c [0] - b [0])]k

This is the result returned in norm.  The function returns an integer
status corresponding to:

                     0 - Operation successful.
                     1 - Face has less than three corners.

*/
{
    VECTOR p [3];       /* Temporary storage for the first three corners. */

    CORNER *chandle;
    int index,count;

    chandle = this_face -> first -> next;
    for (count = 2; count >= 0; count--)    /* Copy the first three corners
                                               of the face to temporary
                                               storage. */
    {
        if (chandle -> next == NULL) return (1); /* Face has less than three
                                                    corners. */
        for (index = 0; index < DIM; index++)
            p [count] [index] = chandle -> this -> coord [index];

        chandle = chandle -> next;
    }

    /* In the following expression,

                        p[0] = a
                        p[1] = b
                        p[2] = c

    */
    norm [0] = ((p[2][1] - p[1][1]) * (p[0][2] - p[1][2]))
             - ((p[2][2] - p[1][2]) * (p[0][1] - p[1][1]));
    norm [1] = ((p[2][2] - p[1][2]) * (p[0][0] - p[1][0]))
             - ((p[2][0] - p[1][0]) * (p[0][2] - p[1][2]));
    norm [2] = ((p[2][0] - p[1][0]) * (p[0][1] - p[1][1]))
             - ((p[2][1] - p[1][1]) * (p[0][0] - p[1][0]));

    return (0);
}