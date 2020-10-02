/* Add corner to face

   Copyright (c) 1988 by Gus O'Donnell

   Revision history:

   Version 1.00         February 29, 1988       As released.

   Version 1.01         March 20, 1988          Created libraries for all
                                                memory models

*/
#include <3d.h>
#include <alloc.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

int     add_corner (double x, double y, double z, FACE *this_face)

/* Add a corner to a face.  The initial data structure looks like this:


               FACE X o----->CORNER o X
                                    |
                                +---+
                                |
                                V
                             CORNER X X


Where 'X' is the NULL pointer.  The revised structure looks like this:


               FACE X o----->CORNER o X
                                    |
                                +---+
                                |
                                V
                             CORNER X o----->VERTEX X
                                    |
                                +---+
                                |
                                V
                             CORNER X X


Note that the corner is added to the beginning of the list.  The corners
are assumed to be added in "standard" order, that is, counterclockwise
as the face is viewed from the outside of the object.

The new corner is checked for colinearity with the first two corners.
If it is colinear, it replaces the current first corner.  The test
proceeds as follows:

Calculate the parametric representation of the line formed by the
first two corners:

                        X = A + t(B - A)                   (1)

where X is any point on the line, A and B are two points on the line
(the first two corners of the face), and t is the parameter.  Thus:

                        X[0] = A[0] + t(B[0] - A[0]),
                        X[1] = A[1] + t(B[1] - A[1]),
                        X[2] = A[2] + t(B[2] - A[2]),      (2)

Substitute the new corner's coordinates into the parametric equations
and calculate t for any dimension i such that A[i] != B[i]:

                    t = (C[i] - A[i])/(B[i] - A[i])        (3)

If all equations (2) are satisfied with this value of t, the points are
colinear.

The value returned is the status:

                   0 - success.
                   1 - memory allocation failed.
                   2 - corner is colinear, replaced first
                       corner with new corner.

*/

{
    int count;
    CORNER *chandle;    /* Handle for new corner */
    VERTEX *vhandle;    /* Handle for new vertex */
    VECTOR A,B,X;       /* Temporary storage for colinearity check */
    double t;           /* Line parameter */

    /* Test for colinearity */

    chandle = this_face -> first -> next;
    if ((chandle -> next) != NULL)                      /* At least 1 corner */
    {
        for (count = 0; count < DIM; count++)
            A [count] = chandle -> this -> coord [count];  /* Get 1st corner */
        chandle = chandle -> next;
        if ((chandle -> next) != NULL)                 /* Face has 2 corners */
        {
            t = 0.0;
            X[0] = x;
            X[1] = y;
            X[2] = z;
            for (count = 0; count < DIM; count++)
            {
                B [count] = chandle -> this -> coord [count];  /* 2nd corner */
                if (A[count] != B[count])
                    t = (X[count] - A[count])/(B[count] - A[count]);
                    count = DIM;                              /* Out of loop */
            }
            if   ((X[0] == A[0] + t*(B[0] - A[0]))
               && (X[1] == A[1] + t*(B[1] - A[1]))
               && (X[2] == A[2] + t*(B[2] - A[2])))     /* Point is colinear */
            {
                chandle = this_face -> first -> next;
                for (count = 0; count < DIM; count++)
                                                   /* Assign new coordinates */
                                                   /* to existing 1st corner */
                    chandle -> this -> coord [count] = X[count];
                return (2);                             /* and return status */
            }
        }
    }

    /* If point is not colinear, get memory.  Return a 1 if unsuccessful. */

    if (!(chandle = (CORNER *)malloc(sizeof(CORNER)))) return(1);
    if (!(vhandle = (VERTEX *)malloc(sizeof(VERTEX)))) return(1);
    chandle -> next = this_face -> first -> next;
    this_face -> first -> next = chandle;
    chandle -> this = vhandle;
    vhandle -> coord [0] = x;
    vhandle -> coord [1] = y;
    vhandle -> coord [2] = z;
    vhandle -> next = NULL;
    return(0);
}