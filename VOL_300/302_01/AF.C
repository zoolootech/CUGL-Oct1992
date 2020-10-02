/* Add face to object

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

int     add_face (OBJECT *this_obj, FACE *this_face)

/* Add a face to an object.  The initial data structure looks like this:


               OBJECT o o------------------------------+
                      |                                |
                      V                                V
                     FACE o X                        VERTEX o
                          |                                 |
                      +---+                            +----+
                      |                                |
                      V                                V
                     FACE X X                        VERTEX X


Where 'X' is the NULL pointer.  The revised structure looks like this:


               OBJECT o o------------------------------+
                      |                                |
                      V                                V
                     FACE o X   +-->CORNER o X       VERTEX o
                          |     |          |                |
                      +---+     |     +----+           +----+
                      |         |     |                |
                      V         |     V                V
                     FACE o o---+   CORNER o o------>VERTEX o
                          |                |                |
                      +---+           +----+           +----+
                      |               |                |
                      V               V                V
                     FACE X X       CORNER o o------>VERTEX o
                                           |                |
                                      +----+           +----+
                                      |                |
                                      V                V
                                    CORNER o o------>VERTEX o
                                           |                |
                                      +----+           +----+
                                      |                |
                                      V                V
                                    CORNER X X       VERTEX X


The corners of the face are merged into the vertex list of the object,
sorted by maximum z, then maximum y, then maximum x.  If a duplicate
vertex is encountered, the corner is set to point at the existing vertex,
and the duplicate is discarded.

Return status as follows:

                 0 - success,
                 1 - face has less than 3 corners.
*/

{
    typedef enum {insert,replace,next} ACTION;

    int count;
    CORNER *chandle;
    VERTEX *vhandle,*v1,*v2;
    ACTION action;

    /* Check number of corners */

    chandle = this_face -> first -> next;
    for (count = 1;count <= 3;count++)
    {
        if (chandle -> next == NULL) return (1);
        chandle = chandle -> next;
    }

    /* Link in the face at the beginning of the list */

    this_face -> next = this_obj -> faces -> next;
    this_obj -> faces -> next = this_face;

    chandle = this_face -> first;
    while (chandle -> next -> next != NULL)
    {
        vhandle = this_obj -> vertices;
        action = next;
        while (action == next)   /* Search the vertex list.  Pseudodata in */
        {                        /* tail node prevents running off end */

        /* The action taken depends on the following conditions:

               z1 ? z2   y1 ? y2   x1 ? x2    Action
             +---------+---------+---------+----------+
             |    <    |    X    |    X    |  Insert  |
             |    =    |    <    |    X    |  Insert  |
             |    =    |    =    |    <    |  Insert  |
             |    =    |    =    |    =    |  Replace |
             |    =    |    =    |    >    |  Next    |
             |    =    |    >    |    X    |  Next    |
             |    >    |    X    |    X    |  Next    |
             +---------+---------+---------+----------+

        Where [x1 y1 z1] is the vertex pointed to by chandle -> next -> this,
          and [x2 y2 z2] is the vertex pointed to by vhandle -> next.
        */

            v1 = chandle -> next -> this;
            v2 = vhandle -> next;
            if (v1 -> coord [2] < v2 -> coord [2])
                action = insert;
            else
            {
                if (v1 -> coord [2] == v2 -> coord [2])
                {
                    if (v1 -> coord [1] < v2 -> coord [1])
                        action = insert;
                    else
                    {
                        if (v1 -> coord [1] == v2 -> coord [1])
                        {
                            if (v1 -> coord [0] < v2 -> coord [0])
                                action = insert;
                            else
                            {
                                if (v1 -> coord [0] == v2 -> coord [0])
                                    action = replace;
                                else
                                    action = next;
                            }
                        }
                    }
                }
            }
            switch (action)
            {
                case insert:
                    v1 -> next = v2;
                    vhandle -> next = v1;
                    break;
                case replace:
                    chandle -> next -> this = v2;
                    free (v1);
                    break;
                case next:
                    vhandle = vhandle -> next;
            }
        }
        chandle = chandle -> next;
    }
    return(0);
}