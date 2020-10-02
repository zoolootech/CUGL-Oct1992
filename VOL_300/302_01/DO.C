/* Display object on screen

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

void    disp_object (VECTOR lsource, int color, OBJECT *this_obj, MATRIX xfrm_mat)

/* Display an object on the screen using the current transform matrix.

The shades of the faces are determined by the angle between the light source,
lsource, and the normal to the face.  The displayed shade is proportional
to the cosine of this angle. */

{
    FACE *fhandle;                      /* Pointer for traversing the face
                                           list */

    fhandle = this_obj -> faces -> next;
    while ((fhandle -> next) != NULL)
    {
        disp_face (lsource, color, fhandle, xfrm_mat);
        fhandle = fhandle -> next;
    }
}
