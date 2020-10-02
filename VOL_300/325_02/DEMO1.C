#include    <stdio.h>
#include    <string.h>
#include    <conio.h>
#include    <math.h>
#include    <stdlib.h>
#include    <grph.h>
#include    <ops2d.h>
#include    <font.h>
#include    <pattern.h>

extern	    Byte    ModeNo;
extern	    int     strtln;
struct	VP  vp1;		    /* Viewport */

extern void Delay(long);

static void near figure(int,int,int,int);

extern	void	pttrn1(void);

void	curves(void)
{

    SetClipOff();			    /* clipping off, to clear screen */
    SetFillPattern(1,0,0,Fp[19]);
    ClearScreen();
    PxlClr=7;
    Rectangl(0,0,XRes,YRes);		    /* Rectangle for whole screen */

    BFntStClr(14);			    /* Text display color */
    strtln = 1; 			    /* Start line of text display */
    BFntStLctn((XRes>>1)-BFntHght*16,strtln);	  /* Start position of Text  */
    strtln += BFntHght; 			  /* for next line of text */
    BFntDsply("Drawing Curves by Setting Pixels");
    BFntStLctn((XRes>>1)-16*12,strtln); 	  /* set next postion */
    BFntDsply("Four Different ViewPorts");

    vp1.x1 = 2; 			/* viewport upper-left	x */
    vp1.y1 = strtln+BFntHght+2; 	/*	    upper-left	y */
    vp1.x2 = ((XRes>>1)+vp1.x1+2);	/*	    lower-right x */
    vp1.y2 = ((YRes>>1)+vp1.y1+2);	/*	    lower-right y */
    SetClipOff();			/* to draw rectangle */

    Rectangl(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    FillRectangle(vp1.x1+1,vp1.y1+1,vp1.x2-1,vp1.y2-1);
    SetViewPort(vp1);
    SetClipOn();			/* Drawings will be clipped */
    MapWorld((float)(vp1.x1),(float)(vp1.y1),\
	     (float)(vp1.x2),(float)(vp1.y2));	 /* World C0-ordinates = */
						 /* viewport boundaries  */

    PxlClr = 15;	/* draw the curve in this color */
    figure(0,vp1.x1+((vp1.x2-vp1.x1)>>1),vp1.y1+((vp1.y2-vp1.y1)>>1),80);

    vp1.x1 = vp1.x2+2;			/* change only upper-left x and */
					/* lower right x for next viewport */
    vp1.x2 = XRes-2;

    SetClipOff();
    PxlClr = 7;
    Rectangl(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    FllClr = 2;
    FillRectangle(vp1.x1+1,vp1.y1+1,vp1.x2-1,vp1.y2-1);
    SetViewPort(vp1);
    SetClipOn();
    MapWorld((float)(vp1.x1),(float)(vp1.y1),\
	     (float)(vp1.x2),(float)(vp1.y2));

    PxlClr = 12;
    figure(1,vp1.x1+((vp1.x2-vp1.x1)>>1),vp1.y1+((vp1.y2-vp1.y1)>>1),100);

    vp1.x1 = 2;
    vp1.y1 = (YRes>>1)+strtln+BFntHght+2;
    vp1.x2 = ((XRes>>1)+vp1.x1+2);
    vp1.y2 = (YRes-2);
    SetClipOff();
    PxlClr = 7;
    Rectangl(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    FllClr = 3;
    FillRectangle(vp1.x1+1,vp1.y1+1,vp1.x2-1,vp1.y2-1);
    SetViewPort(vp1);
    SetClipOn();
    MapWorld((float)(vp1.x1),(float)(vp1.y1),\
	     (float)(vp1.x2),(float)(vp1.y2));

    PxlClr = 13;
    figure(2,vp1.x1+((vp1.x2-vp1.x1)>>1),vp1.y1+((vp1.y2-vp1.y1)>>1),80);

    vp1.x1 = vp1.x2+2;

    vp1.x2 = (XRes-2);

    SetClipOff();
    PxlClr = 7;
    Rectangl(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    FllClr = 4;
    FillRectangle(vp1.x1+1,vp1.y1+1,vp1.x2-1,vp1.y2-1);
    SetViewPort(vp1);
    SetClipOn();
    MapWorld((float)(vp1.x1),(float)(vp1.y1),\
	     (float)(vp1.x2),(float)(vp1.y2));

    PxlClr = 11;
    figure(3,vp1.x1+((vp1.x2-vp1.x1)>>1),vp1.y1+((vp1.y2-vp1.y1)>>1),100);

    getch();

}

void  near  figure(int tp,int xc,int yc,int a)	   /* 4 type of curves */
{

    float   t,dt,r;
    float   x,y;

    t = 0.0f;
    dt = 1.0f / (float)a;

    /* calculate points on the curve */

    while (t < 6.28f)
	{
	    switch (tp)
	    {
		case 0 : r = (float)a * (1.0f + cos(t));break;
		case 1 : r = (float)a * cos(3.0f*t);break;
		case 2 : r = (float)a * cos(4.0f*t);break;
		case 3 : r = (float)a * cos(t * 2.0f);break;
	    }

	    x = (float)xc + r*cos(t);
	    y = (float)yc + r*sin(t);

	    SetPixel(WtoDX(x),WtoDY(y));	/* Pixel setting routine  */

	    t = t + dt;
	}
}

void	ellps(void)			/* ellipse using bresenham algorithm */
{

    int     xc,yc;
    int     a,b,c;
    int     i;

    SetDraw(0);
    SetClipOff();
    SetFillPattern(5,0,0,Fp[19]);
    ClearScreen();

    PxlClr=15;
    Rectangl(0,0,XRes,YRes);

    strtln=1;
    BFntStClr(9);
    BFntStLctn((XRes>>1)-8*BFntWdth,strtln);
    BFntDsply("Drawing Ellipses");

    strtln += BFntHght;
    LineD(1,strtln,(XRes-1),strtln);

    vp1.x1=1;
    vp1.y1=strtln+1;
    vp1.x2=XRes-1;
    vp1.y2=YRes-1;

    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);

    SetViewPort(vp1);
    SetClipOn();

    xc = (XRes>>1)+1;			/* initial center of ellipse */
    yc = (YRes>>1)+1;
    a = (XRes>>1);
    b = (YRes>>1);			/* initial major and minor axes*/
    c = XRes>>1;			/* loop control to change size */

    SetDraw(3);

    PxlClr= 14;
    do
	{
	    for (i=c;i>0;i-=3)		/* shift, re-size, and draw */
		{

		    EllipseD(xc-i,yc-i,a-i,b-i);

		    EllipseD(xc-i,yc-i,a-i,b-i);
		}

	    for (i=0;i<c;i+=3)
		{

		    EllipseD(xc-i,yc+i,a-i,b-i);

		    EllipseD(xc-i,yc+i,a-i,b-i);
		}

	    for (i=c;i>0;i-=3)
		{

		    EllipseD(xc+i,yc-i,a-i,b-i);

		    EllipseD(xc+i,yc-i,a-i,b-i);
		}

	    for (i=0;i<c;i+=3)
		{

		    EllipseD(xc+i,yc+i,a-i,b-i);

		    EllipseD(xc+i,yc+i,a-i,b-i);
		}
	}
    while (!kbhit());
    getch();

}
void Delay(long n)
{
    long    i;

    for (i=0;i<=n;i++) ;
}

void arcs(void) 			    /* draw arc of various params */
{

    float   xc,yc,a,b;
    float   t1,t2;
    int     i,j;
    int     ch;
    float   arcx[40],arcy[40];
    Word    n;

    SetDraw(0);
    SetClipOff();
    SetFillPattern(8,0,0,Fp[19]);
    ClearScreen();

    PxlClr=15;
    Rectangl(0,0,XRes,YRes);

    strtln=1;
    BFntStClr(10);
    BFntStLctn((XRes>>1)-6*BFntWdth,strtln);
    BFntDsply("Drawing Arcs");

    strtln +=BFntHght;
    BFntStLctn((XRes>>1)-18*BFntWdth,strtln);
    BFntDsply("Press 'y' to end, any other key to continue");

    strtln += BFntHght;
    LineD(1,strtln,(XRes-1),strtln);

    vp1.x1=1;
    vp1.y1=strtln+1;
    vp1.x2=XRes-1;
    vp1.y2=YRes-1;

    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);

    xc = (float)(XRes>>1);
    yc = (float)(YRes>>1);

    SetViewPort(vp1);
    MapWorld(-xc,-yc,xc,yc);

    SetClipOn();
    SetDraw(3);

    do
	{			    /* axes, start and end angles */
	    a  = (float)abs(rand() % (XRes>>1));
	    b  = (float)abs(rand() % (YRes>>1));
	    t1 = (float)fabs((double)(rand() % 360));
	    t2 = (float)fabs((double)(rand() % 360));

	    i = rand();
	    j = rand() % 29;

	    PxlClr = (Byte)(rand() % 256);

	    n = Arc(0.0f,0.0f,a,b,t1,t2,arcx,arcy,40);	   /* get the arc */

	    PolyLine(n,arcx,arcy);

	    if (i % 2)
		{
		    Line(arcx[0],arcy[0],arcx[n-1],arcy[n-1]);
		}
	    else if (i % 3)
		{
		    Line (0.0f,0.0f,arcx[0],arcy[0]);
		    Line (0.0f,0.0f,arcx[n-1],arcy[n-1]);

		}
	    else  if (i%4)
		{
		    SetFillPattern(PxlClr+1,0,0,Fp[j]);
		    Line(arcx[0],arcy[0],arcx[n-1],arcy[n-1]);
		    FillPoly(n,arcx,arcy);
		}
	    else
		{
		}

	    ch=getch();
				    /* draw it again in back-ground color */

	    if (i % 2)
		{
		    Line(arcx[0],arcy[0],arcx[n-1],arcy[n-1]);
		}
	    else if (i % 3)
		{
		    Line (0.0f,0.0f,arcx[0],arcy[0]);
		    Line (0.0f,0.0f,arcx[n-1],arcy[n-1]);

		}
	    else if (i%4)
		{
		    SetFillPattern(PxlClr+1,0,0,Fp[j]);
		    Line(arcx[0],arcy[0],arcx[n-1],arcy[n-1]);
		    FillPoly(n,arcx,arcy);
		}
	    else
		{
		}
	    PolyLine(n,arcx,arcy);

	}
    while ((ch != 'y') && (ch != 'Y'));

}

void pttrn1()			       /* fill ellipses with patterns */

{
    float   a,b;
    Byte    BClr,zc;
    int     Pttrn;

    SetDraw(0);
    SetFillPattern(8,0,0,Fp[19]);
    SetClipOff();
    ClearScreen();

    PxlClr=4;
    Rectangl(0,0,XRes,YRes);

    strtln=1;
    BFntStClr(11);
    BFntStLctn(1,strtln);
    BFntDsply("Area Fills with Holes");

    strtln += (int)BFntHght;
    LineD(1,strtln,(XRes-1),strtln);

    vp1.x1=1;
    vp1.y1=strtln+1;
    vp1.x2=XRes-1;
    vp1.y2=YRes-1;

    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);

    SetViewPort(vp1);
    SetClipOn();
    SetDraw(3);

    MapWorld(-500.0f,-500.0f,500.0f,500.0f);

    do
	{				/* big ellipse axes */

	    a  = (float)(abs(rand()) % XRes ) + 150;
	    b  = (float)(abs(rand()) % YRes) + 250;

	    Pttrn = (rand() %  29);	/* random pattern and colors */

	    if (_ZMode== 1)
		PxlClr = (Byte)(rand() % 256);	    /* 256 color mode */
	    else
		PxlClr = (Byte)(rand() % 16);	    /* 16 color mode */
	    if ((!PxlClr) || (PxlClr == FllClr))
		PxlClr = FllClr-1;

	    zc = PxlClr + 1;
	    BClr = PxlClr^8;		    /* border color = drawning color*/
					    /* XOR background color */
	    Ellipse(0.00f,0.00f,a,b);	    /* big ellipse */
					    /* inside rectangle */
	    Rectangl(WtoDX(-100.0f),WtoDY(-100.0f),WtoDX(100.0f),WtoDY(100.0f));

	    Ellipse(-120.00f,120.00f,20.0f,20.0f);
	    Ellipse(120.00f,-120.00f,20.0f,20.0f);    /* small ellipses */
	    Ellipse(-120.00f,-120.00f,20.0f,20.0f);
	    Ellipse(120.00f,120.00f,20.0f,20.0f);

	    SetFillPattern(zc,0,0,Fp[Pttrn]);

	    FillArea(WtoDX(0.00f),WtoDY(150.00f),BClr);   /* fill */

	    SetFillPattern(zc,0,0,Fp[Pttrn]);
							   /* erase the fill */
	    FillArea(WtoDX(0.00f),WtoDY(150.00f),BClr);    /* by redrawing   */
							   /* every thing in */
	    Ellipse(120.00f,120.00f,20.0f,20.0f);	   /* same color in  */
	    Ellipse(-120.00f,-120.00f,20.0f,20.0f);	   /* XOR mode */
	    Ellipse(120.00f,-120.00f,20.0f,20.0f);
	    Ellipse(-120.00f,120.00f,20.0f,20.0f);
	    Rectangl(WtoDX(-100.0f),WtoDY(-100.0f),WtoDX(100.0f),WtoDY(100.0f));

	    Ellipse(0.00f,0.00f,a,b);

	}
    while(!kbhit());
    getch();
}

void dpolyfill(void)	     /* polygon fill in device co-ordinates */
{

    int     Pttrn;
    Byte    PttClr;
    static int PSx[80];
    static int PSy[80];
    int     i;
    Word    n;

    SetDraw(0);
    SetClipOff();
    SetFillPattern(3,0,0,Fp[19]);
    ClearScreen();

    PxlClr=2;
    Rectangl(0,0,XRes,YRes);

    strtln=1;
    BFntStClr(12);
    BFntStLctn((XRes>>1)-18*BFntWdth,strtln);
    BFntDsply("Polygon Filling in Device Co-ordinates");

    strtln += BFntHght;
    LineD(1,strtln,(XRes-1),strtln);

    vp1.x1=1;
    vp1.y1=strtln+1;
    vp1.x2=XRes-1;
    vp1.y2=YRes-1;

    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);

    SetViewPort(vp1);
    SetClipOn();
    SetDraw(3);

    do
	{
	    i  = rand() % 39;
	    if (i < 4)
		i += 4;

	    for (n=0;n<i;n++)
		{
		    PSx[n] = (rand() % (XRes));
		    PSy[n] = (rand() % (YRes));

		}
	    PSx[i]=PSx[0];
	    PSy[i]=PSy[0];

	    Pttrn = rand() % 29;
	    if (_ZMode==1)			/* 256 color mode */
		{
		    PxlClr =(Byte)(rand() % 256);
		    PttClr = PxlClr + 10;
		}
	    else
		{				/* 16 color mode */
		    PxlClr = (Byte)(rand() % 16);
		    PttClr = PxlClr + 2;
		}

	    SetFillPattern(PttClr,0,0,Fp[Pttrn]);
	    FillPolyD(i,PSx,PSy);
	    PolyLineD((i+1),PSx,PSy);

	    SetFillPattern(PttClr,0,0,Fp[Pttrn]);
	    FillPolyD(i,PSx,PSy);
	    PolyLineD((i+1),PSx,PSy);

	}
    while (!(kbhit()));
    getch();
}

void pttrn2(void)			/* pattern reference point */

{					/* polygon edge points */
    static float x2[] = {10.0f,60.0f,100.0f,120.0f,140.0f,200.0f,250.0f,\
			 300.0f,230.0f,180.0f,130.0f,100.0f,80.0f,10.0f};
    static float y2[] = {100.0f,9.0f,70.0f,70.0f,50.0f,80.0f,40.0f,120.0f,\
			 200.0f,160.0f,190.0f,140.0f,170.0f,100.0f};

    Byte    i;
    Byte    j,k;

    SetDraw(0);
    SetClipOff();
    SetFillPattern(8,0,0,Fp[19]);
    ClearScreen();

    PxlClr=13;
    Rectangl(0,0,XRes,YRes);

    strtln=1;
    BFntStClr(14);
    BFntStLctn((XRes>>1)-14*BFntWdth,strtln);
    BFntDsply("Fill Patterns Can be aligned");

    strtln += BFntHght;
    BFntStLctn((XRes>>1)-11*BFntWdth,strtln);
    BFntDsply("with Reference to a point");

    BFntStLctn((XRes>>1)-11*BFntWdth,(YRes-1-BFntHght));
    BFntDsply("Press a key to continue");
    LineD(1,(YRes-2-BFntHght),(XRes-1),(YRes-2-BFntHght));

    strtln += BFntHght;
    LineD(1,strtln,(XRes-1),strtln);

    vp1.x1=1;
    vp1.y1=strtln+1;
    vp1.x2=XRes-1;
    vp1.y2=YRes-3-BFntHght;

    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);

    SetViewPort(vp1);
    SetClipOn();
    if (ModeNo==2)
	MapWorld((float)vp1.x1,(float)vp1.y1,(float)vp1.x2,(float)vp1.y2);
    else
	MapWorld(0.00f,0.00f,320.0f,200.0f);

    PxlClr=12;				/* draw a whole */
    PolyLine(14,x2,y2);
    Ellipse(190.0f,120.0f,10.0f,10.0f);

    PxlClr = 15;			/* arrow line color */

    for (i=0;i<16;i++)
	{
	    SetDraw(0); 		/* replace */
	    if (i<8)
		{
		    j = i;
		    k = 0;
		}
	    else
		{
		    j=0;
		    k=i-8;
		}

	    SetFillPattern(9,j,k,Fp[23]);

	    if (!(i))
		FillArea(WtoDX(100.0f),WtoDY(100.0f),12);
	    else
		FillAgain();

	    Line(100.0f,100.0f,100.0f,110.0f);
	    Line(100.0f,100.0f,110.0f,100.0f);
	    Line(100.0f,100.0f,150.0f,150.0f);

	    getch();

	    SetDraw(3);
	    SetFillPattern(9,j,k,Fp[23]);

	    FillAgain();
	}
    getch();
    SetFillPattern(8,0,0,Fp[19]);
}				 /* text strings */

    static  char    s1[] = {"Bit-Mapped Fonts"};
    static  char    s2[] = {"Font Type 8x16 (This is Normal Size)"};
    static  char    s3[] = {"Size 5x5"};
    static  char    s4[] = {"Size 2x4"};
    static  char    s5[] = {"Size 4x2"};

    static  char    s6[] = {"Font Type 8x8 (This is Normal Size)"};
    static  char    s7[] = {"Size 3x3"};
    static  char    s8[] = {"Zero Orientation, Right Direction"};
    static  char    s9[] = {"Default Orientation, Left Direction"};
    static  char    s10[] = {"90 Degrees, Up"};
    static  char    s11[] = {"90 Degrees, Down"};
    static  char    s12[] = {"180 Degrees, Right"};
    static  char    s13[] = {"180 Degrees, Left"};
    static  char    s14[] = {"270 Degrees, Up"};
    static  char    s15[] = {"270 Degrees, Down"};
    static  char    s16[] = {"360 Degrees, Right"};
    static  char    s17[] = {"360 Degrees, Left"};

    static  char    s18[] = {"Stroked-Fonts"};
    static  char    s19[] = {"Font type 8x8 (This is Normal Size)"};
    static  char    s20[] = {"Stroked Fonts can be"};
    static  char    s21[] = {"Any Size, Can be in Any"};
    static  char    s22[] = {"of Four Directions, and at"};
    static  char    s23[] = {"Any Angle"};

static void near dsply1(void);
static void near dsply2(void);
static void near dsply3(void);
static	void	near	StVwPrt(void);

void fnts(void)
{

    StVwPrt();
    dsply1();			    /* bit-mapped fonts 8*16 */

    StVwPrt();			    /*			8x8  */
    dsply2();

    StVwPrt();			    /* vector fonts	     */
    dsply3();

}

static void near StVwPrt(void)
{

    SetDraw(0);
    SetClipOff();
    SetFillPattern(8,0,0,Fp[19]);
    ClearScreen();

    vp1.x1 = 1;
    vp1.y1 = 1;
    vp1.x2 = XRes - 1;
    vp1.y2 = YRes - 1;
    SetClipOff();
    PxlClr = 7;
    Rectangl(0,0,XRes,YRes);

    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    SetViewPort(vp1);
    MapWorld((float)(vp1.x1),(float)(vp1.y1),(float)(vp1.x2),(float)(vp1.y2));
    SetClipOn();
}

static void near dsply1()
{
    int     i,j;

    BFntInit("v8x16.fnt",16);		/* initialize and load font */

    i = 2;
    BFntFClr = 9;
    BFntStSz(2,2);
    j = (XRes>>1) - (((strlen(s1)) >> 1) * 16);
    BFntStLctn(j,i);
    BFntDsply(s1);

    i += 42;

    Line(Wrldxy[0],Wrldxy[3]-(float)i,Wrldxy[2],Wrldxy[3]-(float)i);

    i += 4;
    BFntFClr ++;
    BFntStSz(1,1);
    j = (XRes>>1) - (((strlen(s2)) >> 1) * 8);
    BFntStLctn(j,i);
    BFntDsply(s2);

    i += 24;
    BFntFClr ++;
    BFntStSz(5,5);
    j = (XRes>>1) - (((strlen(s3)) >> 1) * 40);
    BFntStLctn(j,i);
    BFntDsply(s3);

    i += 88;
    BFntFClr ++;
    BFntStSz(2,4);
    j = (XRes>>1) - (((strlen(s4)) >> 1) * 16);
    BFntStLctn(j,i);
    BFntDsply(s4);

    i += 72;
    BFntFClr ++;
    BFntStSz(4,2);
    j = (XRes>>1) - (((strlen(s5)) >> 1) * 32);
    BFntStLctn(j,i);
    BFntDsply(s5);

    getch();
}

static void near dsply2()
{
    int     i,j,k;

    BFntInit("v8x8.fnt",8);		/* init and load 8x8 font */
					/* which can be rotated */
    i = 2;
    BFntFClr = 9;
    BFntStSz(2,2);
    j = (XRes>>1) - (((strlen(s1)) >> 1) * 16);
    BFntStLctn(j,i);
    BFntDsply(s1);

    i += 34;

    Line(Wrldxy[0],Wrldxy[3]-(float)i,Wrldxy[2],Wrldxy[3]-(float)i);

    i += 4;
    BFntFClr ++;
    BFntStSz(1,1);
    j = (XRes>>1) - (((strlen(s6)) >> 1) * 8);
    BFntStLctn(j,i);
    BFntDsply(s6);

    if (XRes == 319)
	k = 1;
    else
	k = 2;

    i += 16;
    BFntFClr ++;
    BFntStSz(3,3);
    j = (XRes>>1) - (((strlen(s7)) >> 1) * 24);
    BFntStLctn(j,i);
    BFntDsply(s7);

    i += 32;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (XRes>>1) - (((strlen(s8)) >> 1) * (k<<3));
    BFntStLctn(j,i);
    BFntDsply(s8);

    i += 16;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (XRes>>1) + (((strlen(s9)) >> 1) * (k<<3));
    BFntStLctn(j,i);
    SFntStDrctn(left);
    BFntDsply(s9);

    i = 6;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (YRes>>1) + (((strlen(s10)) >> 1) * (k<<3));
    BFntStLctn(i,j);
    SFntStDrctn(up);
    BFntStOrntn(90);		    /* rotate by 90 degrees */
    BFntDsply(s10);

    i = 22;
    BFntFClr ++;		    /* change direction from up to down */
    BFntStSz(k,k);
    j = (YRes>>1) - (((strlen(s11)) >> 1) * (k<<3));
    BFntStLctn(i,j);
    SFntStDrctn(down);

    BFntDsply(s11);

    if (_ZMode == 1)
	BFntFClr = 32;

    i = YRes - 42;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (XRes>>1) - (((strlen(s12)) >> 1) * (k<<3));
    BFntStLctn(j,i);
    SFntStDrctn(right);
    BFntStOrntn(90);		    /* rotate further by 90 degrees */
    BFntDsply(s12);		    /* so effectively 180 degrees */

    i -= 22;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (XRes>>1) + (((strlen(s13)) >> 1) * (k<<3));
    BFntStLctn(j,i);
    SFntStDrctn(left);
    /*BFntStOrntn(90);*/
    BFntDsply(s13);

    i = XRes - 22;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (YRes>>1) + (((strlen(s14)) >> 1) * (k<<3));
    BFntStLctn(i,j);
    SFntStDrctn(up);
    BFntStOrntn(90);		    /* rotate by more 90 degrees to make */
    BFntDsply(s14);		    /* 270 degrees */

    i -= 22;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (YRes>>1) - (((strlen(s15)) >> 1) * (k<<3));
    BFntStLctn(i,j);
    SFntStDrctn(down);
    /*BFntStOrntn(90);*/
    BFntDsply(s15);


    i = YRes>>1;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (XRes>>1) - (((strlen(s16)) >> 1) * (k<<3));
    BFntStLctn(j,i);
    SFntStDrctn(right);
    BFntStOrntn(90);		    /* now total of 360 degrees */
    BFntDsply(s16);

    i += 22;
    BFntFClr ++;
    BFntStSz(k,k);
    j = (XRes>>1) + (((strlen(s17)) >> 1) * (k<<3));
    BFntStLctn(j,i);
    SFntStDrctn(left);
    /*BFntStOrntn(90);*/
    BFntDsply(s17);

    getch();

}

static void near dsply3()
{
    int     i,j,k;
    Byte    Clr;

    SFntInit("sdflt.fnt");	    /* initialise and load stroked-font */
    Clr = 57;

    i = YRes - 22;
    SFntStClr(Clr);
    SFntStSz(2.0f,2.0f);
    j = (XRes>>1) - (((strlen(s18)) >> 1) * 16);
    SFntStLctn((float)j,(float)i);
    SFntStDrctn(right);
    SFntDsply(s18);

    i -= 6;

    Line(Wrldxy[0],(float)i,Wrldxy[2],(float)i);

    i -= 12;
    Clr++;
    SFntStClr(Clr);
    SFntStSz(1.0f,1.0f);
    j = (XRes>>1) - (((strlen(s19)) >> 1) * 8);
    SFntStLctn((float)j,(float)i);
    SFntStDrctn(right);
    SFntDsply(s19);

    if (XRes == 319)
	k = 1;
    else
	k = 2;

    i = (YRes>>1)-100;
    Clr++;
    SFntStClr(Clr);
    SFntStSz((float)k,(float)k);
    j = (XRes>>3) - (((strlen(s20)) >> 1));
    SFntStLctn((float)j,(float)i);
    SFntStOrntn(45.0f);
    SFntDsply(s20);

    i = (YRes>>1) + 100;
    Clr++;
    SFntStClr(Clr);
    SFntStSz((float)k,(float)k);
    j = (XRes>>1) + (((strlen(s21)) >> 1) );
    SFntStLctn((float)j,(float)i);
    SFntStOrntn(-120.0f);
    SFntDsply(s21);

    i -= 275;
    Clr++;
    SFntStClr(Clr);
    SFntStSz((float)k,(float)k);
    j = (XRes>>1) + (((strlen(s22)) >> 1) * (k<<1));
    SFntStLctn((float)j,(float)i);
    SFntStOrntn(-200.0f);
    SFntStDrctn(down);
    SFntDsply(s22);

    i += 49;
    Clr++;
    SFntStClr(Clr);
    SFntStSz((float)k,(float)k);
    j = (XRes>>1) + (((strlen(s23)) >> 1) * (k<<4));
    SFntStLctn((float)j,(float)i);
    SFntStOrntn(240.0f);
    SFntStDrctn(up);
    SFntDsply(s23);

    getch();
}
