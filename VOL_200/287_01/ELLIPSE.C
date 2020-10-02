#include <stdio.h>
#include <gds.h>

extern ARCSTTX, ARCSTTY, ARCENDX, ARCENDY, ARCSTTR, ARCENDR;

struct cpoint {
    int point, state, region;
};

/*===========================================================*
 *                                                           *
 *  This file contains 3 external usable routines to draw    *
 *  ellipse and arc. (Ell is intended to call by Earc2 only) *
 *      Ellipse(centerx,centery,a,b)                         *
 *          It draws a ellipse with center at                *
 *          (centerx,centery) and length of major and minor  *
 *          axes equal to a and b respectively. If a and b   *
 *          are equal, it becomes the radius of a logical    *
 *          circle with center at (centerx,centery). But     *
 *          sometimes when an ellipse appear on an asymmet-  *
 *          ical display, it may look like a circle more     *
 *          than the circle draw by the Circle command.      *
 *                                                           *
 *      EARC1(centerx,centery,a,b,spec)                      *
 *          Draw n/8 of an ellipse. n can be 0 to 8. Read    *
 *          user's manual on the use of the function.        *
 *                                                           *
 *===========================================================*/

Ellipse(ctrx,ctry,a,b)
int ctrx,ctry,a,b;
{
    register int tmp1,tmp2;
    struct cpoint dummy;

    if ((a<=0) || (b<=0)) return;
    ctrx+=ORGX;   /* get absolute position */
    ctry+=ORGY;

    /* return if the whole ellipse is outside the window */
    if ((ctrx+a < WINX1) || (ctrx-a > WINX2) ||
        (ctry+b < WINY1) || (ctry-b > WINY2)) return;
    tmp1=ctrx+a; tmp2=ctry+b;
    if (outside(tmp1,tmp2)) {
        seteptr(1);    /* use '1' if part of the ellipse may be outside
                          the window */
    } else {
        tmp1=ctrx-a; tmp2=ctry-b;
        if (outside(tmp1,tmp2))
            seteptr(1);   /* use '1' if part of the ellipse may be outside
                             the window */
        else 
            seteptr(0);
    }
    /* draw the whole circle and use solid line */
    dummy.point=32767;
    dummy.region=0x8000;
    Ell(ctrx,ctry,a,b,0xff,0xffff,&dummy,&dummy,&dummy,&dummy); 
}

Earc1(ctrx,ctry,a,b,spec)
int ctrx,ctry,a,b,spec;
{
    struct cpoint dummy;

    if ((a<=0) || (b<=0)) return;
    ctrx+=ORGX;   /* get absolute position */
    ctry+=ORGY;

    /* return if the whole ellipse is outside the window */
    if ((ctrx+a < WINX1) || (ctrx-a > WINX2) ||
        (ctry+b < WINY1) || (ctry-b > WINY2)) return;
    seteptr(1);
    dummy.point=32767;
    dummy.region=0x8000;
    Ell(ctrx,ctry,a,b,spec,STYLE,&dummy,&dummy,&dummy,&dummy);
}    


/*-----------------------------------------------------------*
 *  The method used in drawing ellipse is similar to drawing *
 *  circle. However, it is not described in Foley[1982] and  *
 *  it is dervied by myself. The result is shown in the      *
 *  system manual.                                           *
 *-----------------------------------------------------------*/
Ell(ctrx,ctry,a,b,spec,sty,pnt1,pnt2,pnt3,pnt4)
int ctrx,ctry,a,b,spec,sty;
struct cpoint *pnt1, *pnt2, *pnt3, *pnt4;
{
    extern int cirsty;
    long incr1,incr2,a2,b2,d,step1,step2,test,a_b,step3;
    register int x,y;
    int temp, oldx, oldy;

    ARCSPEC=spec & 0x55;
    cirsty=sty;
    x=0;
    y=b;
    a_b = (a2= (long) a * a) + (b2= (long) b * b);
    step3 = (step1 = a2 << 2) + (step2 = b2 << 2);
    d= ((b2 - (test=a2 * b)) << 1) + a2;
    incr1= step2 + (b2 << 1);
    incr2= incr1 + step1 - (test << 1);
    incr2 -= test << 1;
    wrtepta(x++,y,ctrx,ctry); 
    ARCSPEC=spec;
    goto normal1;
    /* Begining of the first main loop */
    while (test > 0) {
        if (!pnt1->point--) {
            spec |= pnt1->region;
            if (pnt1->state) {
                spec ^= pnt1->region;
                ARCSTTX=oldx;
                ARCSTTY=oldy;
            } else {
                ARCSTTX=x;
                ARCSTTY=y;
            }
            ARCSPEC=spec;
        }
        if (!pnt2->point--) {
            spec |= pnt2->region;
            if (pnt2->state) {
                spec ^= pnt2->region;
                ARCENDX=oldx;
                ARCENDY=oldy;
            } else {
                ARCENDX=x;
                ARCENDY=y;
            }
           ARCSPEC=spec;
        }
        wrtept(x++,y,ctrx,ctry);
normal1:
        oldx=x;
        oldy=y;
        if (d<0) {
            d+=incr1;
            incr2+=step2;
            test-=b2;
        }
        else {
            d+=incr2;
            incr2+=step3;
            y--;
            test-=a_b;
        }
        incr1+=step2;
    }
    if (test == 0) {
        ARCSPEC=spec & 0xaa;
        wrtepta(x,y,ctrx,ctry);
        LASTX2=x;
        LASTY2=y;
    } else {
        LASTX2=oldx;
        LASTY2=oldy;
    }
    spec=((spec << 1) & 0x88) | ((spec >> 1) & 0x11);
    ARCSPEC=spec & 0xaa;
    cirsty=sty;
    y=0;
    x=a;
    d= ((a2 - (test=b2 * a)) << 1) + b2;
    incr1= step1 + (a2 << 1);
    incr2= incr1 + step2 - (test << 1);
    incr2 -= test << 1;
    wrtepta(x,y++,ctrx,ctry); 
    ARCSPEC=spec;
    goto normal2;
    /* Begining of the second main loop */
    while (test > 0) {
        if (!pnt3->point--) {
            temp=pnt3->region;
            temp=((temp<<1) & 0x88) | ((temp >> 1) & 0x11);
            spec |= temp;
            if (pnt3->state) {
                spec ^= temp;
                ARCSTTX=oldy;
                ARCSTTY=oldx;
            } else {
                ARCSTTX=y;
                ARCSTTY=x;
            }
            ARCSPEC=spec;
        }
        if (!pnt4->point--) {
            temp=pnt4->region;
            temp=((temp<<1) & 0x88) | ((temp >> 1) & 0x11);
            spec |= temp;
            if (pnt4->state) {
                spec ^= temp;
                ARCENDX=oldy;
                ARCENDY=oldx;
            } else {
                ARCENDX=y;
                ARCENDY=x;
            }
            ARCSPEC=spec;
        }
        wrtept(x,y++,ctrx,ctry);
normal2:
        oldx=x;
        oldy=y;
        if (d<0) {
            d+=incr1;
            incr2+=step1;
            test-=a2;
        }
        else {
            d+=incr2;
            incr2+=step3;
            x--;
            test-=a_b;
        }
        incr1+=step1;
    }
    if (test==0) {
        ARCSPEC=spec & 0x55;
        wrtepta(x,y,ctrx,ctry);
        LASTX=y;
        LASTY=x;
    } else {
        LASTX=oldy;
        LASTY=oldx;
    }
}

