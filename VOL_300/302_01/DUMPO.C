/* Dump object to screen

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

void    dump_obj (OBJECT this_obj)

/* Dump an object to the screen. */

{
    FACE *fhandle;                      /* Pointer for traversing the face
                                           list */

    fhandle = this_obj.faces -> next;
    while ((fhandle -> next) != NULL)
    {
        dump_face (*fhandle);
        fhandle = fhandle -> next;
    }
    printf("\n");
}
