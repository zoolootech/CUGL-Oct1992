#include <stdio.h>
#include <gds.h>

Box(x1,y1,width,height,hw,vw)
int x1,y1,width,height,hw,vw;
{
    if ((hw << 1) >= height || (vw << 1) >= width)
        HorzLine(x1,y1,width,height);
    HorzLine(x1,y1,width,hw);
    y1+=hw; 
    height -= hw << 1;
    HorzLine(x1,y1+height,width,hw);
    VertLine(x1,y1,height,vw);
    VertLine(x1+width-vw,y1,height,vw);
}

Rectangle(x1,y1,width,height)
int x1,y1,width,height;
{
    HorzLine(x1,y1++,width,1);
    height -= 2;
    HorzLine(x1,y1+height,width,1);
    VertLine(x1,y1,height,1);
    VertLine(x1+width-1,y1,height,1);
}

