/* Create new face

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

int     new_face (FACE *this_face)

/* Create a new face.  The initial data structure looks like this:


               FACE o X
                    |
                    |
                    V
                 CORNER o X
                        |
                    +---+
                    |
                    V
                 CORNER X X

Where 'X' is the NULL pointer.

The function returns an integer status as follows:

             0 - operation successful
             1 - cannot allocate memory
*/

{
	CORNER *chandle1,*chandle2;     /* Handles for head and tail nodes of
                                       CORNER list */

    /* Allocate memory.  Return a 1 if unsuccessful. */

    if (!(chandle1 = (CORNER *)malloc(sizeof(CORNER)))) return(1);
    if (!(chandle2 = (CORNER *)malloc(sizeof(CORNER)))) return(1);
    this_face -> first = chandle1;
    this_face -> next = NULL;
    chandle1 -> this = NULL;
    chandle1 -> next = chandle2;
    chandle2 -> this = NULL;
    chandle2 -> next = NULL;
    return(0);
}
