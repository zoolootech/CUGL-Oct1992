#include <stdio.h>
#include <gds.h>

/*==============================================================*
 *                                                              *
 *  This file contain 2 routines for Arc2 and Earc2 use only    *
 *                                                              *
 *==============================================================*/

/* p2 is in degree between 0 and 90, returns p1*sin(p2) in integer */
#define gdssin(p1,p2) (int)((long)p1*SINVAL[p2]+16384>>15)

extern int ARCSTTX, ARCSTTY, ARCENDX, ARCENDY, ARCSTTR, ARCENDR;

struct cpoint {
    int point,state,region;
};

int arcreg[] = {
      0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00
};
int regbit[] = {
      0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};
 
extern unsigned int SINVAL[];

setpnt(pnt1,pnt2,reg,odd,even,major,minor,deg1,deg2)
register struct cpoint *pnt1, *pnt2;
int odd, even, major, minor, deg1, deg2;
{
    struct cpoint tmppnt;

    tmppnt.region=regbit[reg];
    if (reg & 0x01) {
        tmppnt.point=gdssin(minor,deg1);
        if ((reg == 3) || (reg == 7)) {
            pnt2->point=major;
            pnt2->region=0x8000;      /* indicate this struct is not used */
            pnt1->state=odd;
            pnt1->point=tmppnt.point;
            pnt1->region=tmppnt.region;
        } else {
            pnt1->point=major;
            pnt1->region=0x8000;      /* indicate this struct is not used */
            pnt2->state=odd;
            pnt2->point=tmppnt.point;
            pnt2->region=tmppnt.region;
        }
    } else {
        tmppnt.point=gdssin(major,deg2);
        if ((reg == 0) || (reg == 4)) {
            pnt2->point=minor;
            pnt2->region=0x8000;
            pnt1->state=even;
            pnt1->point=tmppnt.point;
            pnt1->region=tmppnt.region;
        } else {
            pnt1->point=minor;
            pnt1->region=0x8000;
            pnt2->state=even;
            pnt2->point=tmppnt.point;
            pnt2->region=tmppnt.region;
        }
    }
} 

/* special cases handling for Arc2 and Earc2 */
adjpnt(arcp,pnt1,pnt2,shortarc,rotreg)
int *arcp, shortarc, rotreg;
register struct cpoint *pnt1, *pnt2;
{
    *arcp |= pnt1->region | pnt2->region;
    if ((pnt1->region == pnt2->region) && (pnt1->region & 0xff)) {
      /* if starting and ending point in the same region
         and they are used, then requires special check */
      if (shortarc)
          *arcp ^= pnt1->region;
      if (pnt1->point==pnt2->point) {
label1:
        if (shortarc) {
            if (rotreg & 0x01)
                pnt1->point++;
            else
                pnt2->point++;
        } else {
            if (!(rotreg & 0x01)) {
                pnt1->point++;
            }
        }
      } else {
        if (pnt1->state) pnt1->point++;
        if (pnt2->state) pnt2->point++;
        if (pnt1->point == pnt2->point) goto label1;
      }
    } else {
        if (!pnt1->state) *arcp ^= pnt1->region; else pnt1->point++;
        if (!pnt2->state) *arcp ^= pnt2->region; else pnt2->point++;
    }
    if (!pnt1->point--) {
        *arcp |= pnt1->region;
        if (pnt1->state) *arcp ^= pnt1->region;
    }
    if (!pnt2->point--) {
        *arcp |= pnt2->region;
        if (pnt2->state) *arcp ^= pnt2->region;
    }
}

