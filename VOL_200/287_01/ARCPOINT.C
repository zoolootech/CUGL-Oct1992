#include <stdio.h>
#include <gds.h>

extern int ARCSTTX, ARCSTTY, ARCSTTR, ARCENDX, ARCENDY, ARCENDR;

ArcPoint(stx,sty,endx,endy)
int *stx, *sty, *endx, *endy;
{
    findarcpoint(stx,sty,ARCSTTX, ARCSTTY, ARCSTTR);
    findarcpoint(endx,endy,ARCENDX, ARCENDY, ARCENDR);
    *stx+=LASTX-ORGX;
    *endx+=LASTX-ORGX;
    *sty+=LASTY-ORGY;
    *endy+=LASTY-ORGY;
}

static findarcpoint(x,y,ax,ay,region)
int *x, *y, ax, ay, region;
{
    *x=ax;
    *y=-ay;
    switch(region) {
    case 1:
        *x=ay;
        *y=-ax;
        break;
    case 2:
        *x=ay;
        *y=ax;
        break;
    case 3:
        *y=ay;
        break;
    case 4:
        *x=-ax;
        *y=ay;
        break;
    case 5:
        *x=-ay;
        *y=ax;
        break;
    case 6:
        *x=-ay;
        *y=-ax;
         break;
   case 7:
        *x=-ax;
    }
}

