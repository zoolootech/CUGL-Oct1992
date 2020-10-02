/* Delete face from object

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

int     del_face (OBJECT *this_obj, FACE *this_face)

/* Delete a face from an object.  The list of faces is traversed
until the face is located, then it is unlinked from the list.
The function returns a 0 if the operation completes successfully.
The function returns a 1 if the face isn't found. */

{
	FACE *fhandle;      /* Pointer to traverse the face list */
    CORNER *chandle;    /* Temporary pointer to free corners */
    int not_done;

	fhandle = this_obj -> faces;
    not_done = 1;
	while ((fhandle -> next -> next != NULL) && not_done)
	{
		if (fhandle -> next == this_face)
		{

            /* Unlink the face */

            fhandle -> next = fhandle -> next -> next;

            /* Free the corners */

            do {
                chandle = this_face -> first;
                this_face -> first = this_face -> first -> next;
                free (chandle);
            } while (this_face -> first != NULL);
            free (this_face);
            not_done = 0;
        }
        fhandle = fhandle -> next;
    }
    return (not_done);
}