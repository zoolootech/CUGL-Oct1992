#include <stdio.h>
#include <gds.h>

#define ERROR (-1)
#define OK 0
#define VOIDBLOCK 1

/*==============================================================*
 *  This file contains the following routine:                   *
 *      blockclip       clip a block against the window         *
 *                      this function is used by HorzLine,      *
 *                      VertLine, BlockCopy                     *
 *                                                              *
 *==============================================================*/

blockclip(x,y,length,height)
int *x, *y, *length, *height;
{
    /* return if the whole block is above or on the right of the window */
    if (*y > WINY2 || *x > WINX2) {
        return(VOIDBLOCK);
    }
    /* any part of the line on the top of the window  */
    if (*y < WINY1)
        /* yes */
        if ((*height -= (WINY1 - *y)) <= 0) {
            return(VOIDBLOCK); 
        } else
          *y=WINY1;

    /* any part of the line on the left of window ? */
    if (*x < WINX1)
        /* yes */
        if ((*length -= (WINX1 - *x)) <= 0) {
            return(VOIDBLOCK);
        } else
          *x=WINX1;

    /* any part of the line is below the window ? */
    if (*y+*height-1 > WINY2)
        /* yes */
        *height=WINY2-*y+1;

    /* any part of the line on the right of the window ? */
    if (*x+*length-1 > WINX2)
        /* yes */
        *length=WINX2-*x+1;

    return(OK);
}

