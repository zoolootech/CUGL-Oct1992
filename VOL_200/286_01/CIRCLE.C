#include <stdio.h>
#include <gds.h>

struct cpoint {
    int point, state, region;
};

/*===========================================================*
 *                                                           *
 *  This file contains 2 external usable routines to draw    *
 *  circle and arc.                                          *
 *      Circle(centerx,centery,radius)                       *
 *          It draws a logical circle and the 'circle' will  *
 *          look like a circle only on a screen with square  *
 *          pixels (width and height is the same.            *
 *                                                           *
 *      Arc1(centerx,centery,radius,spec)                    *
 *          Draw n/8 of a circle. n can be 0 to 8. Read      *
 *          user's manual on the use of the function.        *
 *                                                           *
 *===========================================================*/

/*-----------------------------------------------------------*
 *  The algorithm used in drawing circle is come from the    *
 *  the book "Fundamentals of interactive computer Graphics" *
 *  by Foley[1982]. Read the book about the algorithm.       *
 *                                                           *
 *  The points drawn by this method will have less than or   *
 *  equal to half of a pixel from the corresponding point    *
 *  on a perfect circle of the same size.                    *
 *-----------------------------------------------------------*/

Circle(centerx,centery,radius)
int centerx,centery,radius;
{
    register int tmp1,tmp2;
    struct cpoint dummy1,dummy2;

    if (radius <= 0) return;
    centerx += ORGX;  /* get absolute position */
    centery += ORGY;

    /* return if the circle must be outside the window */
    if ((centerx+radius < WINX1) || (centerx-radius > WINX2) ||
        (centery+radius < WINY1) || (centery-radius > WINY2)) return;

    tmp1=centerx+radius; tmp2=centery+radius;
    if (outside(tmp1,tmp2)) {
        setcptr(1);  /* use '1' if part of the circle may be outside
                        the window */
    } else {
        tmp1=centerx-radius; tmp2=centery-radius;
        if (outside(tmp1,tmp2))
            setcptr(1);  /* use '1' if part of the circle may be outside
                        the window */
        else  /* the whole circle is within the window */
            setcptr(0);
    }
    ARCSPEC=0xff;    /* draw the whole circle */
    dummy1.point=dummy2.point=radius;
    /* always use solid line */
    circ(centerx,centery,radius,0xffff,&dummy1,&dummy2);
}

Arc1(centerx,centery,radius,spec)
int centerx,centery,radius,spec;
{
    struct cpoint dummy;

    if (radius <= 0) return;
    centerx += ORGX;   /* get absolute location */
    centery += ORGY;

    /* if the whole circle is outside is outside the window, return */
    if ((centerx+radius < WINX1) || (centerx-radius > WINX2) ||
        (centery+radius < WINY1) || (centery-radius > WINY2)) return;
    setcptr(1);  /* always use '1' in arc drawing */
    ARCSPEC=spec;
    dummy.point=32767;
    dummy.region=0x8000;
    /* use the STYLE setting */
    circ(centerx,centery,radius,STYLE,&dummy,&dummy);
}

