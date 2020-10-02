/* Find maximum z coordinate in face

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
#include <values.h>

double  max_z (FACE this_face)

/* Find the maximum z coordinate in the face. */

{
    double result;
    CORNER *chandle;    /* Pointer for traversing the corner list. */

    chandle = this_face.first -> next;
    result = -MAXDOUBLE;
    while (chandle -> next != NULL)
    {
        if (chandle -> this -> coord [2] > result)
            result = chandle -> this -> coord [2];
        chandle = chandle -> next;
    }
    return(result);
}