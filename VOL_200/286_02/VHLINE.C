#include <stdio.h>
#include <gds.h>

#define VOIDBLOCK 1

/*==============================================================*
 *  This file contains the following routines:                  *
 *                                                              *
 *      Dot(x,y) :-                                             *
 *              Writing a single dot to a frame                 *
 *                                                              *
 *      HorzLine(x,y,length,width)                              *
 *              Drawing a horizontal line starting at (x,y)     *
 *              with length and width in the positive direction *
 *              of x and y respectively.                        *
 *                                                              *
 *      VertLine(x,y,length,width)                              *
 *              Drawing a vertical line starting at (x,y)       *
 *              with length and width in the positive direction *
 *              of y and x respectively                         *
 *                                                              *
 *==============================================================*/


Dot(x,y)
int x,y;
{
    x += ORGX; 
    y += ORGY;
    if (inside(x,y)) plot(x,y); /* plot if the dot is within the window */
}

HorzLine(x,y,length,width)
int x,y,length,width;
{
    if ((length<=0) || (width <= 0)) return; /* width must be positive */
    x += ORGX;              /* get absolute position */
    y += ORGY;

    if (blockclip(&x, &y, &length, &width)==VOIDBLOCK) return;
    do {
        horzl(x,y++,length); /* draw a line with width 1 each time */
    } 
    while (--width);
}

VertLine(x,y,length,width)
int x,y,length,width;
{
    if ((length<=0) || (width <= 0)) return; /* width must be positive */
    x += ORGX;              /* get absolute position */
    y += ORGY;

    if (blockclip(&x, &y, &width, &length)==VOIDBLOCK) return;
    vertl(x,y,length,width);
}

