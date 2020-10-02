/* Dump face to screen

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

void	dump_face (FACE this_face)

/* Dump a face to the screen. */

{
    CORNER *chandle;	/* Pointer to traverse the corner list */

	chandle = this_face.first -> next;
	while ((chandle -> next) != NULL)
	{
		dump_vec (*(chandle -> this));
		chandle = chandle -> next;
	}
	printf ("next face = %08X\n",this_face.next);
}
