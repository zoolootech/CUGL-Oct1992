/* Dump matrix to screen

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

void    dump_mat(MATRIX this_mat)

/* Dump the matrix to the screen.  Provided as a debugging tool. */

{
    int rc;

    for (rc = 0; rc < RMAX; rc++)
    printf (" %+12.5e, %+12.5e, %+12.5e, %+12.5e\n", this_mat [rc] [0],
                                                     this_mat [rc] [1],
                                                     this_mat [rc] [2],
                                                     this_mat [rc] [3]);
    printf ("\n");
}
