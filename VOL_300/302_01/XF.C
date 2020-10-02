/* Transform object

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

int     xform (OBJECT this_obj, MATRIX transform)

/*  Transform an object.  This function transforms an object from one
coordinate system to another as specified by the transform matrix.
Each vertex in the vertex list is multiplied by the transformation matrix.
*/

{
    VERTEX *vhandle;	/* Pointer to traverse the vertex list */

    vhandle = this_obj.vertices -> next;
    while (vhandle -> next != NULL)
    {
        vec_mul (vhandle, transform, vhandle);
        vhandle = vhandle -> next;
    }
    return(0);
}