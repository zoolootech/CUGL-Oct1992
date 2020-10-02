#include    <stdlib.h>
#include    <grph.h>

void far space(int x1,int y1,int x2,int y2)
{

    int     i,x,y;
    Byte    LPxlClr,LFllClr;
    struct  VP	vp1,vp2;

    GetViewPort(&vp2);

    LPxlClr = PxlClr;
    LFllClr = FllClr;

    vp1.x1=x1;
    vp1.y1=y1;
    vp1.x2=x2;
    vp1.y2=y2;
    SetViewPort(vp1);

    x = vp1.x2>>1;
    y = vp1.y2>>1;
    PxlClr=8;
    EllipseD(x,y,10,12);
    FllClr=15;
    FillArea(x,y,PxlClr);

    PxlClr=15;
    for (i=0;i<45;i++)
	{
	    x = rand() % x2;
	    y = rand() % y2;
	    SetPixel(x,y);
	}

    for (i=0;i<30;i++)
	{
	    x = rand() % x2;
	    y = rand() % y2;
	    SetPixel(x,y);
	    SetPixel(x+1,y);
	}

    PxlClr = 13;
    for (i=0;i<20;i++)
	{
	    x = rand() % x2;
	    y = rand() % y2;
	    SetPixel(x,y);
	}

    PxlClr = LPxlClr;
    FllClr = LFllClr;
    SetViewPort(vp2);
}
