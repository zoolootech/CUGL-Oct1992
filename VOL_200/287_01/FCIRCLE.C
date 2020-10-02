#include <stdio.h>
#include <gds.h>

/*===========================================================*
 *                                                           *
 *  This file contains functions :-                          *
 *      FillCircle(centerx,centery,radius)                   *
 *          Draw a solid circle. Usage is same as Circle     *
 *                                                           *
 *      FillEllipse(centerx,centery,a,b)                     *
 *          Draw a solid ellipse. Usage is same as Ellipse   *
 *                                                           *
 *===========================================================*/

FillCircle(centerx,centery,radius)
int centerx,centery,radius;
{
    register int x,y;
    int incr1,incr2,d,tmpstyle;

    if (radius <= 0) return;
    if ((ORGX+centerx+radius < WINX1) || (ORGX+centerx-radius > WINX2) ||
        (ORGY+centery+radius < WINY1) || (ORGY+centery-radius > WINY2)) return;
    tmpstyle=STYLE;
    STYLE=0xffff;
    incr1=6;
    incr2=10 - (radius << 2);
    for (d=3-(radius<<1),x=0,y=radius; x<y; x++) {
        HorzLine(centerx-y,centery+x,(y<<1)+1,1);
        if (x) HorzLine(centerx-y,centery-x,(y<<1)+1,1);
        if (d<0) {
            d += incr1;
            incr2 += 4;
        } else {
            HorzLine(centerx-x,centery+y,(x<<1)+1,1);
            HorzLine(centerx-x,centery-y,(x<<1)+1,1);
            d += incr2;
            incr2 += 8;
            y--;
            }
        incr1 += 4;
        }
    if (x==y) {
        HorzLine(centerx-x,centery+y,(x<<1)+1,1);
        HorzLine(centerx-x,centery-y,(x<<1)+1,1);
        }
    STYLE=tmpstyle;
}

FillEllipse(ctrx,ctry,a,b)
int ctrx,ctry,a,b;
{
    long incr1,incr2,a2,b2,d,step1,step2,test,a_b,step3;
    int tmpstyle;
    register int x,y;

    if ((a<=0) || (b <= 0)) return;
    if ((ctrx+a < WINX1) || (ctrx-a > WINX2) ||
        (ctry+b < WINY1) || (ctry-b > WINY2)) return;
    tmpstyle=STYLE;
    STYLE=0xffff;
    x=0;
    y=b;
    a_b = (a2=(long) a*a) + (b2= (long) b*b);
    step3=(step1=a2<<2)+(step2=b2<<2);
    d=((b2-(test=a2*b)) << 1) + a2;
    incr1=step2+(b2<<1);
    incr2=incr1+step1-(test<<1);
    incr2-=test << 1;
    while (test>0) {
        if (d<0) {
            d+=incr1;
            incr2+=step2;
            test-=b2;
        } else {
            d+=incr2;
            incr2+=step3;
            test-=a_b;
            HorzLine(ctrx-x,ctry+y,(x<<1)+1,1);
            HorzLine(ctrx-x,ctry-y,(x<<1)+1,1);
            y--;
            }
        incr1+=step2;
        x++;
        }
    if (test==0) {
        HorzLine(ctrx-x,ctry+y,(x<<1)+1,1);
        HorzLine(ctrx-x,ctry-y,(x<<1)+1,1);
        }
    y=0;
    x=a;
    d=((a2-(test=b2*a)) << 1) + b2;
    incr1=step1+(a2<<1);
    incr2=incr1+step2-(test<<1);
    incr2-=test<<1;
    while (test>0) {
        HorzLine(ctrx-x,ctry+y,(x<<1)+1,1);
        if (y) HorzLine(ctrx-x,ctry-y,(x<<1)+1,1);
        if (d<0) {
            d+=incr1;
            incr2+=step1;
            test-=a2;
        } else {
            d+=incr2;
            incr2+=step3;
            test-=a_b;
            x--;
            }
        incr1+=step1;
        y++;
        }
    STYLE=tmpstyle;
}

