#include <stdio.h>
#include <gds.h>

extern int ARCSTTX, ARCSTTY, ARCENDX, ARCENDY, ARCSTTR, ARCENDR;

struct cpoint {
    int point,state,region;
};

extern int arcreg[], regbit[];
 
#define on 1
#define off 0

/*==============================================================*
 *  Arc2 use a special way to calculate the starting and ending *
 *    coordiantes of the arc. It is quite difficult for me to   *
 *    explain with my limited English vocabulary. But generally *
 *    speaking, the method used can be summarized as follows:   *
 *    a. A circle is divided into 8 symmetrical portion. When   *
 *       the circle is drawn, only one of the 8 portion is      *
 *       calculated. Others portions are drawn using the same   *
 *       result but appropiate adjustment is given.             *
 *    b. So when we calculate the starting and ending points,   *
 *       we do not calculate the actual coordinates instead     *
 *       we calcalate the correspond coordinates in the portion *
 *       that we actually calculate and draw.                   *
 *    c. Actually it is not correct to say `coordinates' above  *
 *       because I only find the x coordiate only.              *
 *    d. Since a circle is symmetrical about its center, I      *
 *       can rotate it without affecting the output. So         *
 *       I always rotate (logically) the circle such that       *
 *       the starting point is always within 45 degree          *
 *       in clockwise direction from the vertical upward line   *
 *    e. After calculation, I rotate it back to originally      *
 *       position.                                              *
 *    f. There are some special cases handling which make       *
 *       the program even more difficult to understand.         *
 *       Therefore, unless you are absolutely necessary         *
 *       to change this program, better left it alone.          *
 *                                                              *
 *==============================================================*/

Arc2(centerx,centery,radius,deg1,deg2)
int centerx,centery,radius,deg1,deg2;
{
    int region2, arcp, rotreg;
    struct cpoint pnt1, pnt2;

    if (deg2<=0) {        /* deg2 must be greater than 0 */
setnull:
        LASTX=centerx;
        LASTY=centery;
        ARCSTTX=ARCSTTY=ARCENDX=ARCENDY=0;
        return;
    }

    /* if deg2 >= 360, use Arc1 */
    if (deg2>=360) Arc1(centerx,centery,radius,0xff);

    centerx += ORGX;   /* get absolute location */
    centery += ORGY;

    /* if the whole circle is outside is outside the window, return */
    if ((centerx+radius < WINX1) || (centerx-radius > WINX2) ||
        (centery+radius < WINY1) || (centery-radius > WINY2)) goto setnull;
    setcptr(1);  /* always use '1' in arc drawing */

    /* deg1=deg1 mod 360, result always is positive */
    arcp=deg1/360;
    deg1-=arcp*360;
    if (deg1<0) deg1+=360;

    rotreg=deg1/45;     /* 45 degree a region, get region number */
    deg1 %= 45;         /* get angle within the region */

    deg2 += deg1;       /* find the ending degree */
    region2=deg2/45;    /* calculate the region number */
    deg2 %= 45;         /* and angle within the region */

    arcp= 0xff ^ arcreg[region2];       /* find region needed to be filled */
    arcp=arcp<<rotreg;  /* rotate lower 8 bit or arcp */
    arcp=(arcp & 0xff) | ((arcp & 0xff00) >> 8);
    region2=(region2+rotreg) & 0x07; /* get actual region number after 
                                         unrotate */
    ARCSTTR=rotreg;  /* for finding the actual coordinate of */
    ARCENDR=region2; /* starting and ending point */

    /* calculate the corresponding x coordinate for 
       starting and ending point and stored in struct pnt1 and pnt2 */
    setpnt(&pnt1,&pnt1,rotreg,on,off,radius,radius,45-deg1,deg1);
    setpnt(&pnt2,&pnt2,region2,off,on,radius,radius,45-deg2,deg2);
    if (pnt1.point==0) {
        ARCSTTX=0;
        ARCSTTY=radius;
    }
    if (pnt2.point==0) {
        ARCENDX=0;
        ARCENDY=radius;
    }
    /* special cases handling !! */
    adjpnt(&arcp,&pnt1,&pnt2,(deg1<deg2),rotreg);
    ARCSPEC=arcp;
    circ(centerx,centery,radius,STYLE,&pnt1,&pnt2);
    /* another special case handling !! */
    if (pnt1.point >= 0) {
        ARCSTTX=LASTX;
        ARCSTTY=LASTY;
    }
    if (pnt2.point >= 0) {
        ARCENDX=LASTX;
        ARCENDY=LASTY;
    }
    LASTX=centerx;
    LASTY=centery;
}

