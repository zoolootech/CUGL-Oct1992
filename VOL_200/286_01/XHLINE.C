#include <stdio.h>
#include <gds.h>

int XHLine(x,y,boundary,leftx,rightx)
int x,y,boundary,*leftx,*rightx;
{
    int ret;

    x+=ORGX;
    y+=ORGY;
    if (outside(x,y)) return(-1);
    ret=hlnlmt(x,y,(boundary ? 0xffff : 0),STYLE,leftx,rightx);
    if (!ret) {
        *leftx+=ORGX;
        *rightx+=ORGX;
    }
    return(ret);
}

