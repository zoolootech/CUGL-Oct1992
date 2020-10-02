/* Dump vertex to screen

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

void    dump_vec (VERTEX this_vec)

/* Dump a vertex to the screen. */

{
    printf (" %+12.5e, %+12.5e, %+12.5e, next = %06X\n",this_vec.coord [0],
                                                        this_vec.coord [1],
                                                        this_vec.coord [2],
                                                        this_vec.next);
}
