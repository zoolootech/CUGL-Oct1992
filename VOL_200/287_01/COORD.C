#include <stdio.h>
#include <gds.h>

SetOrg(x,y)
int x,y;
{
    ORGX=x;
    ORGY=y;
}

RelOrg(x,y)
int x,y;
{
    ORGX += x;
    ORGY += y;
}

SetWin(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
    int temp;

    if (x1 > x2) {
        temp=x1; 
        x1=x2; 
        x2=temp;
    }
    if (y1 > y2) {
        temp=y1; 
        y1=y2; 
        y2=temp;
    }
    x1 += ORGX; 
    x2 += ORGX;
    y1 += ORGY; 
    y2 += ORGY;
    if (x1 > XLIMIT)
        WINX2=WINX1=XLIMIT;
    else {
        if (x1 < 0)
            WINX1=0;
        else
            WINX1=x1;
        if (x2 > XLIMIT)
            WINX2=XLIMIT;
        else
            WINX2=x2;
    }
    if (y1 > YLIMIT)
        WINY2=WINY1=YLIMIT;
    else {
        if (y1 < 0)
            WINY1=0;
        else
            WINY1=y1;
        if (y2 > YLIMIT)
            WINY2=YLIMIT;
        else
            WINY2=y2;
    }
}

ResetWin()
{
    WINX1=WINY1=0;
    WINX2=XLIMIT;
    WINY2=YLIMIT;
}

