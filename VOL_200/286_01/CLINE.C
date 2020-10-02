#include <stdio.h>
#include <gds.h>

/*===========================================================*
 *                                                           *
 *  This file contain only 1 public routine:                 *
 *                                                           *
 *      Line(x1,y2,x2,y2)                                    *
 *              draw a line from (x1,y1) to (x,y2)           *
 *              inclusively                                  *
 *                                                           *
 *===========================================================*/

/* returns a 4 bit code giving information of a point relative */
/* to the window. The 4 bits represent left, right, above      */
/* and below of the window                                     */
#define getcode(x,y) (x<WINX1 ?1:(x>WINX2 ?2:0))|(y<WINY1 ?4:(y>WINY2 ?8:0))

/*-----------------------------------------------------------*
 *  The algorithm used in clipping and drawing a line        *
 *  is adapted from the book "Fundamentals of interactive    *
 *  Computer Graphics" by Foley[1982]                        *
 *                                                           *
 *  I would not try to explain the algorithm here. Interested*
 *  user should refer the book                               *
 *                                                           *
 *-----------------------------------------------------------*/

Line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
    int d,incr1,incr2,xdir,ydir,dir;
    register int dx, dy;

    x1+=ORGX;  /* get absolute location */
    y1+=ORGY; 
    x2+=ORGX; 
    y2+=ORGY;

/* clipping of the line, refer to Foley[1982] for explanation */
    for (;;) {
        if ((incr1=getcode(x1,y1)) & (incr2=getcode(x2,y2))) return;
        if (!(incr1 | incr2)) break;
        if (incr1)
            clip(incr1,&x1,&y1,x2,y2);
        else
            clip(incr2,&x2,&y2,x1,y1);
    }

    xdir=ydir=0;
    if ((dx=x2-x1) < 0) { 
        dx = -dx; 
        xdir=1; 
    }
    if ((dy=y2-y1) < 0) { 
        dy = -dy; 
        ydir=1; 
    }
    if (dx == 0) {         /* a vertical line ? */
        if (ydir) y1=y2;
        vertl(x1,y1,++dy,1);
        return;
    }
    if (dy == 0) {         /* a horizontal line ? */
        if (xdir) x1=x2;
        horzl(x1,y1,++dx);
        return;
    }
    dir = (xdir == ydir) ? 1 : -1;
    if (dy <= dx) {
        if (xdir) {
            x1=x2; 
            y1=y2;
        }
        d = (incr1 = dy << 1) - dx;
        incr2 = (dy - dx) << 1;
         /* . . . . . . . . . . . . . . . . . . . . . . . .*/
         /* When to use lline1 or lline2 is determined by  */
         /* experiment result. The objective is to maximize*/
         /* the speed in drawing line                      */
         /* . . . . . . . . . . . . . . . . . . . . . . . .*/
        if (dy > (dx >> 1)) 
            /* . . . . . . . . . . . . . . . . . . . . . . . .*/
            /* absolute value of the slope of the line is     */
            /* greater than 1/2. Use lline1 to draw the line  */
            /* . . . . . . . . . . . . . . . . . . . . . . . .*/
            lline1(x1,y1,dir,dx,d,incr1,incr2);
        else
            /* . . . . . . . . . . . . . . . . . . . . . . . .*/
            /* absolute value of the slope of the line        */
            /* is between 0 and 1/2 (excluding 0). use lline2 */
            /* to draw the line                               */
            /* . . . . . . . . . . . . . . . . . . . . . . . .*/
            lline2(x1,y1,dir,dx,d,incr1,incr2);
    } 
    else {
        if (ydir) {
            x1=x2; 
            y1=y2;
        }
        d = (incr1 = dx << 1) - dy;
        incr2 = (dx - dy) << 1;
         /* . . . . . . . . . . . . . . . . . . . . . . . .*/
         /* When to use lline3 or lline4 is determined by  */
         /* experiment result. The objective is to maximize*/
         /* the speed in drawing line                      */
         /* . . . . . . . . . . . . . . . . . . . . . . . .*/
        if (dx >= (dy >> 2))
            /* . . . . . . . . . . . . . . . . . . . . . . . .*/
            /* absolute value of the slope of the line is     */
            /* greater than 4. Use lline3 to draw the line    */
            /* . . . . . . . . . . . . . . . . . . . . . . . .*/
            lline3(x1,y1,dir,dy,d,incr1,incr2);
        else
            /* . . . . . . . . . . . . . . . . . . . . . . . .*/
            /* absolute value of the slope of the line        */
            /* is between infinitive and 4. use lline4        */
            /* to draw the line                               */
            /* . . . . . . . . . . . . . . . . . . . . . . . .*/
            lline4(x1,y1,dir,dy,d,incr1,incr2);
    }
}

/*-----------------------------------------------------------*
 *  chop off the part of the line which is outside the window*
 *-----------------------------------------------------------*/
static clip(code,x1,y1,x2,y2)
int code,*x1,*y1,x2,y2;
{
    register int dx,dy;
    int temp;

    dx=x2 - *x1;
    dy=y2 - *y1;

    if (code & 0x0c) {
        if (dy) {
            temp = code & 0x08 ? WINY2 : WINY1;
            *x1 += (dx * (temp - *y1) + (dy >> 1)) / dy;
        }
        *y1 = temp;
        return;
    }
    if (dx) {
        temp = code & 0x02 ? WINX2 : WINX1;
        *y1 += (dy * (temp - *x1) + (dx >> 1)) / dx;
    }
    *x1 = temp;
}

