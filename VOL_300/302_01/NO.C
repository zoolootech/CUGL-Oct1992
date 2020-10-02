/* Create new object

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
#include <values.h>

int     new_obj (OBJECT *this_obj)

/* Create a new obj.  The initial data structure looks like this:


               OBJECT o o------>FACE o X
                      |              |
                      V           +--+
                   VERTEX o       |
                          |       V
                      +---+     FACE X
                      |
                   VERTEX X


Where 'X' is the NULL pointer.  The value returned is 0 if the operation
is successful, 1 otherwise. */

{
    int count;
	FACE *fhandle1,*fhandle2;       /* Handles for head and tail nodes of
									   FACE list */
	VERTEX *vhandle1,*vhandle2;     /* Handles for head and tail nodes of
                                       VERTEX list */

    /* Allocate memory.  Return a 1 if unsuccessful. */

    if (!(fhandle1 = (FACE *)malloc(sizeof(FACE)))) return(1);
    if (!(fhandle2 = (FACE *)malloc(sizeof(FACE)))) return(1);
    if (!(vhandle1 = (VERTEX *)malloc(sizeof(VERTEX)))) return(1);
    if (!(vhandle2 = (VERTEX *)malloc(sizeof(VERTEX)))) return(1);
    this_obj -> faces = fhandle1;
    fhandle1 -> next = fhandle2;
    fhandle1 -> first = NULL;
    fhandle2 -> first = NULL;
    fhandle2 -> next = NULL;
    this_obj -> vertices = vhandle1;
    vhandle1 -> next = vhandle2;
    vhandle2 -> next = NULL;
    for (count = 0;count < DIM;count++)  /* Put pseudodata in end nodes */
    {
        vhandle1 -> coord [count] = -MAXDOUBLE;
        vhandle2 -> coord [count] =  MAXDOUBLE;
    }
    return(0);
}