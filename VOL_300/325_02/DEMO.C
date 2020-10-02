#include    <math.h>
#include    <time.h>
#include    <stdio.h>
#include    <conio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <grph.h>
#include    <ops2d.h>
#include    <font.h>
#include    <sound.h>

/*	Variables   */

Byte  ModeNo,GMode;		    /* User and Graphic board specific Modes */
int   lines,maxlines;		    /* for text display */
int   strtln;			    /* text display line */
struct	VP  vp1;		    /* viewport corners */
extern	Byte	far Fp[29][8];	    /* 29 fill patterns */
int	 menuinput;		    /* input menu option */

/*	   function prototpes	  */

extern	void near initfont(void);   /* local font initialization  */
extern	void	intro(void);	    /*	Introduction   */
extern	void near setscreen1(void); /*	Setp Up Screen type 1 */
extern	void	curves(void);	    /*	Drawing Curves In Multiple Viewports */

extern	void	ellps(void);	    /* Ellipses      */
extern	void	arcs(void);	    /* Arcs	     */

extern	void	pttrn1(void);	    /* Flood filling with patterns */

extern	void dpolyfill(void);	    /* Polygon-Filling in Device Co-ordinates*/
extern	void	pttrn2(void);	    /* Setting fill pattern reference point */
extern	void	fnts(void);	    /* Bit-mapped and stroked fonts */
extern	void	btblt(void);	    /* Bit-Block moves and animation */
extern	void	Delay(long);	    /* just a loop routine for time delays */

extern	void	pan(void);	    /* panning	    */
extern	void	cpimg(void);	    /* Image Copy within video buffer	*/
extern	void	trnsfrm(void);	    /* 2D transformations   */
extern	void	space(int,int,int,int);     /*	Draw Moon and stars   */

extern	void	clrs(void);	    /* colurs */
extern	void	sound(void);	    /* play notes of various frequencies */
extern	void	menu(void);	    /* main menu routine  */

int	main(void)
{
    time_t	timenow;
    int 	dmd,k,j;

    timenow = time(NULL);
    srand((int)timenow);

    do
	{
	    printf("Enter Mode No between 0 and 6 as one of following");
	    dmd = Mode640x350x16;
	    printf("\n   0 = 640x350x16  Mode no = %x ",dmd);
	    dmd = Mode640x480x16;
	    printf("\n   1 = 640x480x16  Mode no = %x ",dmd);
	    dmd = Mode320x200x256;
	    printf("\n   2 = 320x200x256 Mode no = %x ",dmd);
	    dmd = Mode640x400x256;
	    printf("\n   3 = 640x400x256 Mode no = %x ",dmd);
	    dmd = Mode640x480x256;
	    printf("\n   4 = 640x480x256 Mode no = %x ",dmd);
	    dmd = Mode800x600x16;
	    printf("\n   5 = 800x600x16  Mode no = %x ",dmd);
	    dmd = Mode1024x768x16;
	    printf("\n   6 = 1024x768x16 Mode no = %x ",dmd);
	    printf("\n     ---->");

	    scanf("%d",&ModeNo);

	}
    while ((ModeNo<0) || (ModeNo>6));

    switch (ModeNo)
    {
	case 0 : j= Mode640x350x16;break;
	case 1 : j= Mode640x480x16;break;
	case 2 : j= Mode320x200x256;break;
	case 3 : j= Mode640x400x256;break;
	case 4 : j= Mode640x480x256;break;
	case 5 : j= Mode800x600x16;break;
	case 6 : j= Mode1024x768x16;
    }


    if ((k = OpenGraph(ModeNo,0)) != j)
	{
	    CloseGraph();
	    printf("Mode set error %d \n",k);
	    exit(1);
	}

    do
	{
	    menu();

	}
    while (menuinput != 14);

    CloseGraph();
    return(0);
}

void	menu(void)
{

    int     leftx;

    initfont(); 		    /* set font and sizes depending upon mode */

    SetClipOff();
    ClearScreen();

    FllClr=(Byte)(rand() % 16);     /* random back ground color */
    if (FllClr == 15)		    /* must not be same text color */
	FllClr--;

    vp1.x1=1;			    /* view-port corners */
    vp1.y1=1;
    vp1.x2=XMax-1;
    vp1.y2=YMax-1;

    SetFillPattern(FllClr,0,0,Fp[19]);			/* Solid Fill  */
    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    Rectangl(vp1.x1-1,vp1.y1-1,vp1.x2+1,vp1.y2+1);
    SetViewPort(vp1);		    /* activate viewport */

    strtln = 1;

    BFntStLctn(((XRes>>1)-15*BFntWdth),strtln);
    BFntDsply("VGA Graphics Demonstration Menu");

    strtln += BFntHght; 	    /* next line of text */

    PxlClr = FllClr+1;
    LineD(1,strtln,XRes-1,strtln);  /* just draw a line and then leave one */
    SetClipOn();
    BFntStClr(15);

    strtln += BFntHght;
    strtln += BFntHght; 	    /* more blank line */
    leftx = (XRes>>2)-25;

    BFntStLctn(leftx,strtln);
    BFntDsply(" a. Introduction ");

    strtln += BFntHght; 	    /* menu options display */
    BFntStLctn(leftx,strtln);
    BFntDsply(" b. Pixels");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" c. Ellipses");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" d. Arcs");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" e. Polygon Fill");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" f. Area Fill");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" g. Fill Patterns");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" h. Bit-mapped & Stroked Fonts");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" i. Bit-block moves");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" j. Image copy / Window scrolling");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" k. Transformations");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" l. Smooth Pixel panning");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" m. Colours");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" n. Sound Effects");

    strtln += BFntHght;
    BFntStLctn(leftx,strtln);
    BFntDsply(" o. Exit");

    do				    /* call appropriate routine */
	{
	    menuinput = getch();

	    if (menuinput<97)
		menuinput -= 65;
	    else
		menuinput -= 97;

	    switch(menuinput)
		{
		    case 14: break;

		    case 0 : intro();break;

		    case 1 : curves();break;

		    case 2 : ellps();break;

		    case 3 : arcs();break;

		    case 4 : dpolyfill();break;

		    case 5 : pttrn1();break;

		    case 6 : pttrn2();break;

		    case 7 : fnts();initfont();break;

		    case 8 : btblt();break;

		    case 9 : cpimg();break;

		    case 10: trnsfrm();break;

		    case 11: pan();break;

		    case 12: clrs();break;

		    case 13: sound();break;
		}
	}
    while ((menuinput<0) || (menuinput>14));
}

void	near	initfont()
{

    if (ModeNo==2)		      /* 320x200  mode, load 8x8 font*/
	BFntInit("v8x8.fnt",8);
    else
	BFntInit("v8x16.fnt",16);     /* others, load 8x16 font*/

    if (ModeNo>5)		      /* 1024x768 mode, font size = 2x1 */
	BFntStSz(2,1);
    else
	BFntStSz(1,1);		      /* others, font size = 1x1 */
}

void	intro(void)
{

    FILE    *intfl;
    int     sz,maxlines;
    char    flnm[13];
    char    far *s1;
    char    s2[81];

    initfont();

    memset(s2,(int) '\0',81);

    if (ModeNo==6)
	BFntStSz(2,1);
    else
	BFntStSz(1,1);

    maxlines = YMax - (BFntHght<<1);

    setscreen1();

    if (ModeNo==2)
	{
	    strcpy(flnm,"intrtxt2.dat\0");	/* intro data file name */
	    sz = 41;
	}
    else
	{
	    strcpy(flnm,"intrtxt1.dat\0");
	    sz = 80;
	}
    if ((s1 = (char far *)malloc(sz)) == NULL)
	{
	    BFntDsply("Memory Problem");
	    return;
	}

    if ((intfl = fopen(flnm,"rb")) == NULL)
	{
	    BFntDsply("Intro data File  ");
	    BFntDsply(flnm);
	    BFntDsply("  Not Found, Press a Key");
	    getch();
	    free(s1);
	    return;
	}

    while (!feof(intfl))		/* read a line and display */
	{
	    if (fread(s1,sz,1,intfl) == 1)
		{
		    if (strtln>=maxlines)	 /* when buffer end */
			{
			    while (YOrgn<=(YMax-YRes-7))
				{
				    getch();
				    SScroll(YRes,2);   /* scroll-up */
				 }		       /* by no of lines in */
			     getch();		       /* one screen */
			     SetOrigin(0,0);	       /* at end */
			     setscreen1();
			}
		    strncpy(s2,s1,sz-3);

		    strtln += BFntHght;
		    BFntStLctn(1,strtln);

		    BFntDsply(s2);
		}
	    else if (!feof(intfl))
		{
		    BFntDsply("read  error, press any key");
		    getch();
		    fclose(intfl);
		    free(s1);
		    return;
		}
	}

    while (strtln > (YOrgn+YRes))		 /* last frame */
       {
	   getch();
	   SScroll(YRes,2);
       }
    getch();

    SetOrigin(0,0);

    fclose(intfl);
    free(s1);

}

void near setscreen1(void)
{

    SetClipOff();
    vp1.x1=1;
    vp1.y1=1;
    vp1.x2=XMax-1;
    vp1.y2=YMax-1;

    FllClr = (Byte)(rand() % 6);

    SetFillPattern(FllClr,0,0,Fp[19]);			/* Solid Fill  */
    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    Rectangl(vp1.x1-1,vp1.y1-1,vp1.x2+1,vp1.y2+1);
    SetViewPort(vp1);

    PxlClr=7;
    LineD(1,BFntHght<<1,XRes-1,BFntHght<<1);
    SetClipOn();
    BFntStClr(15);

    strtln = 1;

    BFntStLctn(((XRes>>1)-10*BFntWdth),strtln);
    BFntDsply("VGA Graphics Routines");

    strtln += BFntHght;
    BFntStLctn(((XRes>>1)-17*BFntWdth),strtln);
    BFntDsply("Capabilities Demonstration Program");
}

void btblt(void)			/* bit-block moves */
{

    Word    j;
    void    far *p1;			/* memory blocks to read images */
    void    far *p2;
    void    far *p3;
    void    far *p4;
    float   x1,x2,y1,y2;
    int     a1,b1,a2,b2;

    SetDraw(0);
    SetClipOff();
    ClearScreen();

    strtln = 1;
    BFntStClr(15);
    BFntStLctn((XRes>>1)-14*BFntWdth,strtln);
    BFntDsply("Bit-Block Moves and Animation");
    strtln += BFntHght;
    LineD(1,strtln,(XRes-1),strtln);

    vp1.x1=0;
    vp1.y1=0;
    vp1.x2=319;
    if (_ZMode==0)		    /* view port lower right corner y */
	vp1.y2=479;
    else
	vp1.y2=199;
    SetViewPort(vp1);		    /* set view port */
    x2 = (float)((vp1.x2+1));	    /* world mapping corners */
    x1 = -x2;
    y1 = 200.0f;
    y2 = -y1;
    MapWorld(x1,y1,x2,y2);	    /* map world to device co-ordinates */


    a1 = WtoDX(-80.0f); 	    /* must be same as when image was created */
    b1 = WtoDY(-80.0f); 	    /* the image boundin rectangle */
    a2 = WtoDX(110.0f);
    b2 = WtoDY(105.0f);

    j = ImageSize(a1,b1,a2,b2);     /* no of bytes required to store */
				    /* image */
    p1 = (void far *)malloc(j);   /* allocate memory for 4 buffers */
    p2 = (void far *)malloc(j);
    p3 = (void far *)malloc(j);
    p4 = (void far *)malloc(j);

    if (_ZMode==0)		    /* load images from disk */
	{
	    LoadImage("b112",(void far *)p1,j);     /* use file depending */
	    LoadImage("b212",(void far *)p2,j);     /* upon mode used */
	    LoadImage("b312",(void far *)p3,j);
	    LoadImage("b412",(void far *)p4,j);     /* 16 color modes */
	}
    else
	{
	    LoadImage("b113",(void far *)p1,j);     /* 256 color modes */
	    LoadImage("b213",(void far *)p2,j);     /* because of different */
	    LoadImage("b313",(void far *)p3,j);     /* resolutions, same file */
	    LoadImage("b413",(void far *)p4,j);     /* makes a picture of */
	}					    /* different sizes */

    if (_ZMode==0)
	{					    /* start points */
	    a2 = -4;
	    b2 = (YRes+1)>>4;
	}
    else
	{
	    a2 = XRes;
	    b2 = (YRes+1)>>3;
	}

    a1=a2;
    b1=b2;

    j = 4;					    /* no of pixels to move */
    SetDraw(0);
    do
	{
	    PutImage(a1,b1,(void far *)p1);
	    Delay(20000L);
	    a1 -= j;

	    PutImage(a1,b1,(void far *)p2);
	    Delay(20000L);
	    a1 -= j;

	    PutImage(a1,b1,(void far *)p3);
	    Delay(20000L);
	    a1 -= j;

	    PutImage(a1,b1,(void far *)p4);
	    Delay(20000L);
	    a1 -= j;

	    PutImage(a1,b1,(void far *)p3);
	    Delay(20000L);
	    a1 -= j;

	    PutImage(a1,b1,(void far *)p2);
	    Delay(20000L);
	    a1 -= j;

	    if ((a1>=0) && (a1<32))
		{
		    SetDraw(3); 		/* erase last image using XOR */
		    a1 += j;			/* and displaying image at same loc. */
		    PutImage(a1,b1,(void far *)p2);
		    SetDraw(0); 		/* re-set to Replace drawing mode */
		    a1 = a2;
		    b1 = b2;
		}
	}
    while(!(kbhit()));
    getch();

    free(p1);
    free(p2);
    free(p3);
    free(p4);
}

void pan()			     /* scrolling */
{
	ClearScreen();

	space(0,0,XMax,YMax);	     /* draw the space background */
	SetClipOff();

	PxlClr=5;
	Rectangl(0,0,XMax,YMax);
	BFntStClr(14);
	BFntStLctn(4,(XRes>>1) - 128);
	BFntStSz(2,2);
	BFntDsply("Smooth Scrolling");


	SScroll(XMax-XRes,0);	    /* scroll left */

	SScroll(YMax-YRes,2);	    /* scroll up */

	getch();
	SScroll(XMax-XRes,1);	    /* scroll right */
	getch();
	SScroll(YMax-YRes,3);	    /* scroll down */
	getch();

	ClearScreen();
	SetXLen(100);		    /* re-dimension video buffer */
	space(0,0,XMax,YMax);	    /* so that it extends beyond right hand */
	PxlClr=4;		    /* screen edge */
	Rectangl(0,0,XMax,YMax);

	SScroll(XMax-XRes,0);	    /* scroll left */

	getch();

	SScroll(YMax-YRes,2);	    /* scroll up */

	getch();
	SScroll(XMax-XRes,1);	    /* scroll right */
	getch();
	SScroll(YMax-YRes,3);	    /* and down to original position */
	getch();
	if (XRes <600)		    /* set video dimensions to original val */
	    SetXLen(80);
}

void cpimg(void)			/* scrolling windows */

{

    int     x11,y11,x12,y12;
    int     x21,y21,x22,y22;
    int     x31,y31,x32,y32;
    int     vscrll;

    SetClipOff();
    ClearScreen();

    PxlClr =9;
    Rectangl(0,0,XRes,YRes);

    FllClr =  8;
    FillRectangle(1,1,XRes-1,YRes-1);

    strtln = 1;

    BFntStClr(15);

    BFntStLctn((XRes>>1)-BFntWdth*8,strtln);
    BFntDsply("Window Scrolling");

    strtln += BFntHght;

    LineD(1,strtln,XRes-1,strtln);

    vp1.x1 = 15;
    vp1.y1 = strtln+BFntHght;
    vp1.x2 = ((vp1.x1 + (XRes>>1)-19) & 0xfff8);
    vp1.y2 = vp1.y1 + ((YRes-vp1.y1)>>1)-6;

    PxlClr = 15;
    Rectangl(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    FllClr = 4;
    FillRectangle(vp1.x1+1,vp1.y1+1,vp1.x2-1,vp1.y2-1);

    x11 = vp1.x1;		    /* make another window using */
    y11 = vp1.y2+5;		    /* copyimage */
    x12 = vp1.x2;
    y12 = y11 + vp1.y2-vp1.y1;

    x21 = vp1.x2+15;		    /* make another window using */
    y21 = vp1.y1;		    /* copyimage */
    x22 = x21 + vp1.x2 - vp1.x1;
    y22 = vp1.y2;

    x31 = x21;			    /* make another window using */
    y31 = y11;			    /* copyimage */
    x32 = x22;
    y32 = y12;

    CopyImage(vp1.x1,vp1.y1,vp1.x2,vp1.y2,x11,y11);
    CopyImage(vp1.x1,vp1.y1,vp1.x2,vp1.y2,x21,y21);
    CopyImage(vp1.x1,vp1.y1,vp1.x2,vp1.y2,x31,y31);


    vp1.x1++; x31++; x11++;x21++;
    vp1.y1++; y31++; y11++;y21++;
    vp1.x2--; x32--; x12--;x22--;
    vp1.y2--; y32--; y12--;y22--;

    if (_ZMode == 0)
	vscrll=8;
    else
	vscrll = 4;

    do
	{
				   /* scroll down by 4 scan lines */
	    WindScrollDown(vp1.x1,vp1.y1,vp1.x2,vp1.y2,4);
	    PxlClr ++;
	    LineD(vp1.x1,vp1.y1,vp1.x2,vp1.y1);
				   /* scroll up by one scan line */
	    WindScrollUp(x31,y31,x32,y32,1);
	    PxlClr ++;
	    LineD(x31,y32,x32,y32);
	    WindScrollLeft(x11,y11,x12,y12,vscrll);  /* only multiples of 8 */
	    PxlClr ++;				    /* allowed for left right*/
	    LineD(x12,y11,x12,y12);		    /* scroll */
	    WindScrollRight(x21,y21,x22,y22,vscrll);
	    PxlClr ++;
	    LineD(x21,y21,x21,y22);

	}
    while (!(kbhit()));
    getch();


}

void trnsfrm(void)			  /* transformations */

{
    struct  VP	vp2,vp3,vp4;

    Word    n;
    float   svx[51],svy[51];
    float   nwx[51],nwy[51];
    float   m1[9],m2[9];

    SetClipOff();
    ClearScreen();

    PxlClr =9;
    Rectangl(0,0,XRes,YRes);

    FllClr =  8;
    FillRectangle(1,1,XRes-1,YRes-1);

    strtln = 1;

    BFntStClr(15);

    BFntStLctn((XRes>>1)-BFntWdth*8,strtln);
    BFntDsply("2D transformations");

    strtln += BFntHght;

    LineD(1,strtln,XRes-1,strtln);

    vp1.x1 = 15;
    vp1.y1 = strtln+BFntHght;
    vp1.x2 = ((vp1.x1 + (XRes>>1)-19) & 0xfff8);
    vp1.y2 = vp1.y1 + ((YRes-vp1.y1)>>1)-6;

    PxlClr = 15;
    Rectangl(vp1.x1,vp1.y1,vp1.x2,vp1.y2);
    FllClr = 5;
    FillRectangle(vp1.x1+1,vp1.y1+1,vp1.x2-1,vp1.y2-1);

    vp2.x1 = vp1.x1;		       /* make another window using */
    vp2.y1 = vp1.y2+5;		       /* copyimage */
    vp2.x2 = vp1.x2;
    vp2.y2 = vp2.y1 + vp1.y2-vp1.y1;

    vp3.x1 = vp1.x2+15; 	       /* make another window using */
    vp3.y1 = vp1.y1;		       /* copyimage */
    vp3.x2 = vp3.x1 + vp1.x2 - vp1.x1;
    vp3.y2 = vp1.y2;

    vp4.x1 = vp3.x1;			  /* make another window using */
    vp4.y1 = vp2.y1;			  /* copyimage */
    vp4.x2 = vp3.x2;
    vp4.y2 = vp2.y2;

    CopyImage(vp1.x1,vp1.y1,vp1.x2,vp1.y2,vp2.x1,vp2.y1);
    CopyImage(vp1.x1,vp1.y1,vp1.x2,vp1.y2,vp3.x1,vp3.y1);
    CopyImage(vp1.x1,vp1.y1,vp1.x2,vp1.y2,vp4.x1,vp4.y1);

    n = Arc(0.0f,0.0f,20.0f,20.0f,0.0f,355.00f,svx,svy,50);
    svx[50] = svx[0]; svy[50]=svy[0];

    SetClipOn();
    SetViewPort(vp1);
    MapWorld(-100.0f,-100.0f,100.0f,100.0f);

    PolyLine(51,svx,svy);

    BFntStLctn(vp1.x1+1,vp1.y1+1);
    BFntStClr(1);
    BFntDsply("Original circle");

    SetViewPort(vp2);
    MapWorld(-100.0f,-100.0f,100.0f,100.0f);
    for (n=0; n<51; n++)
	{
	    nwx[n] = svx[n];
	    nwy[n] = svy[n];
	}

    XShr2D(1.5f,m1);
    YShr2D(2.3f,m2);
    CmbnTrnsfrm2D(m1,m2);
    TrnsfrmPnts2D(51,nwx,nwy,m2);
    PolyLine(51,nwx,nwy);

    BFntStLctn(vp2.x1+1,vp2.y1+1);
    BFntDsply("Sheared");

    SetViewPort(vp3);
    MapWorld(-100.0f,-100.0f,100.0f,100.0f);
    for (n=0; n<51; n++)
	{
	    nwx[n] = svx[n];
	    nwy[n] = svy[n];
	}

    Rotate2D(60.0f,0.0f,0.0f,m1);
    CmbnTrnsfrm2D(m1,m2);
    TrnsfrmPnts2D(51,nwx,nwy,m2);
    PolyLine(51,nwx,nwy);

    BFntStLctn(vp3.x1+1,vp3.y1+1);
    BFntDsply("Sheared & Rotated");

    SetViewPort(vp4);
    MapWorld(-100.0f,-100.0f,100.0f,100.0f);
    for (n=0; n<51; n++)
	{
	    nwx[n] = svx[n];
	    nwy[n] = svy[n];
	}

    Scale2D(3.5f,2.5f,0.0f,0.0f,m2);
    TrnsfrmPnts2D(51,nwx,nwy,m2);
    PolyLine(51,nwx,nwy);

    BFntStLctn(vp4.x1+1,vp4.y1+1);
    BFntDsply("Scaled");

    getch();

}


void clrs()
{
    Byte i,j,k;
    int     xl,yl,yl1;
    struct  RGBstruct	RGB[64];	    /* RGB variable 3-byte structer */
    Byte    last,current;
    float   a1,b1,a2,b2;
    float   arcx[100],arcy[100];

    SetClipOff();
    ClearScreen();

    SetDraw(0);
    SetFillPattern(7,0,0,Fp[19]);

    PxlClr = 1;
    Rectangl(0,0,XRes,YRes);

    strtln = 1;

    BFntStClr(13);
    BFntStLctn((XRes>>1)-14*BFntWdth,strtln);
    BFntDsply("Colour Registers Manipulation");
    strtln += BFntHght;

    LineD(1,strtln,XRes-1,strtln);

    xl = ((XRes-1) >> 4);
    yl = ((YRes-strtln) >> 2);
    yl1 = (yl>>2);

    for (i=0;i<4;i++)			/* show all 256 colors */
	{
	    for (j=0;j<16;j++)
		{
		    FllClr=(Byte)(j+i*64);
		    FillRectangle(j*xl+1,i*yl+strtln,(j+1)*xl-1,i*yl+yl1-1+strtln);
		}

	    for (j=0;j<16;j++)
		{
		    FllClr=(Byte)(16+j+i*64);
		    FillRectangle(j*xl+1,i*yl+yl1+strtln,(j+1)*xl-1,i*yl+2*yl1-1+strtln);
		}

	    for (j=0;j<16;j++)
		{
		    FllClr=(Byte)(32+j+i*64);
		    FillRectangle(j*xl+1,i*yl+2*yl1+strtln,(j+1)*xl-1,i*yl+3*yl1-1+strtln);
		}

	    for (j=0;j<16;j++)
		{
		    FllClr=(Byte)(48+j+i*64);
		    FillRectangle(j*xl+1,i*yl+3*yl1+strtln,(j+1)*xl-1,i*yl+4*yl1-1+strtln);
		}
	    if (_ZMode == 0)
		getch();

	}

    for (i=0;i<4;i++)
	{
	    getch();			    /* grey-scale 4 blocks of */
	    GreyScale(i*64,64); 	    /* 64 registers */
	}
    getch();
    for (i=0;i<4;i++)
	{			 /* for 16 color modes, select dac group */
	    if	((ModeNo <2) || (ModeNo > 4))  /* as one of 4 groups each of */
		SelectDAC(i);		       /* 64 registers */
	    for (k=0;k<64;k++)
		{
		    switch(i)	       /* Red,Green,Blue and Grey shades */
			{
			    case 0 : RGB[k].Red = k;RGB[k].Green=0;RGB[k].Blue=0;break;
			    case 1 : RGB[k].Red = 0;RGB[k].Green=k;RGB[k].Blue=0;break;
			    case 2 : RGB[k].Red = 0;RGB[k].Green=0;RGB[k].Blue=k;break;
			    case 3 : RGB[k].Red = k;RGB[k].Green=k;RGB[k].Blue=k;break;
			}
		}

	/*	set RGB values in 64 registers */


	    SetRGBDAC(i*64,64,(struct RGBstruct far *)RGB);

	/* draw these 64 color rectangles */

	    for (j=0;j<16;j++)
		{
		    FllClr=(Byte)(j+i*64);
		    FillRectangle(j*xl+1,i*yl+strtln,(j+1)*xl-1,i*yl+yl1-1+strtln);
		}

	    for (j=0;j<16;j++)
		{
		    FllClr=(Byte)(16+j+i*64);
		    FillRectangle(j*xl+1,i*yl+yl1+strtln,(j+1)*xl-1,i*yl+2*yl1-1+strtln);
		}

	    for (j=0;j<16;j++)
		{
		    FllClr=(Byte)(32+j+i*64);
		    FillRectangle(j*xl+1,i*yl+2*yl1+strtln,(j+1)*xl-1,i*yl+3*yl1-1+strtln);
		}

	    for (j=0;j<16;j++)
		{
		    FllClr=(Byte)(48+j+i*64);
		    FillRectangle(j*xl+1,i*yl+3*yl1+strtln,(j+1)*xl-1,i*yl+4*yl1-1+strtln);
		}
	}
    getch();

    OpenGraph(ModeNo,1);	     /* to set default color register values */
				     /* do a mode set without clearing buffer*/
    SetClipOff();
    ClearScreen();

    PxlClr = 12;
    Rectangl(0,0,XRes,YRes);
    strtln = 1;

    BFntStClr(15);
    BFntStLctn((XRes>>1)-14*BFntWdth,strtln);
    BFntDsply("Colour Palette Manipulation");
    strtln += BFntHght;

    LineD(1,strtln,XRes-1,strtln);

    vp1.x1=1;
    vp1.y1=strtln+1;
    vp1.x2=XRes-1;
    vp1.y2=YRes-1;

    FllClr = 8;
    FillRectangle(vp1.x1,vp1.y1,vp1.x2,vp1.y2);

    SetViewPort(vp1);

    a1 = -500.0f;
    b1 = -500.0f;
    a2 =  500.0f;
    b2 =  500.0f;

    MapWorld(a1,b1,a2,b2);

    PxlClr = 15;

    Line(0.0f,b1,0.0f,b2);

    a1 = a2 / 8.0f;

    for (i=8;i>0;i--)		    /* draw 8 ellipses and fill in all */
	{			    /* 16 palette colors */
	    a2 = (float)(i) * a1;
	    Arc(0.0f,0.0f,a2,b2,0.0f,359.99f,arcx,arcy,100);
	    PolyLine(100,arcx,arcy);

	    FllClr = (Byte)8-i;

	    FillArea(WtoDX(-10.0f),WtoDY(0.0f),PxlClr);

	    FllClr = (Byte)16 - i;

	    FillArea(WtoDX(10.0f),WtoDY(0.0f),PxlClr);
	}
    getch();

    last = GetPalette(0);	    /* rotate palette values */
    do				    /* by putting last palette value into */
	{			    /* next palette */
	    for (i=1;i<16;i++)
		{
		    current = GetPalette(i);
		    SetPalette(i,last);
		    last=current;
		}
	    last = GetPalette(0);
	    SetPalette(0,current);
	}
    while (!(kbhit()));
    getch();

}

void sound(void)		    /* sounds of different frequencies */
{				    /* and durations */

    unsigned int     i,j;

    for (j=1;j<5;j++)
	{
	    for (i=1;i<25;i++)
		{
		    PlayNote(540,33);
		    PlayNote(650,26);
		}
	    Delay(30000L);
	}

    Delay(32767L);


    for (i=200;i<1005;i+=4)
	PlayNote(i,40);
    for (i=1000;i>700;i-=4)
	PlayNote(i,40);
    for (i=700;i<1004;i+=4)
	PlayNote(i,40);
    for (i=1000;i>700;i-=4)
	PlayNote(i,40);
    for (i=700;i<1004;i+=4)
	PlayNote(i,40);

    Delay(32767L);

    for (i=1;i<21;i++)
	{
	    PlayNote(11000,20);
	    Delay(300L);
	}

    Delay(32767L);

    for (i=1;i<11;i++)
	for (j=300;j<1800;j+=40)
	    PlayNote(j,1);

    Delay(32767L);

    for (i=1;i<31;i++)
	for (j=40;j<=80;j++)
	    PlayNote(j,1);

    Delay(32767L);

    for (i=1500;i>550;i-=4)
	PlayNote(i,30);
    PlayNote(140,100);
    for (i=1500;i>550;i-=4)
	PlayNote(i,30);
    PlayNote(140,100);

    Delay(32767L);

    for (i=1;i<9;i++)
	for (j=600;j<750;j+=8)
	    PlayNote(j,10);
    for (i=1250;i>600;i-=8)
	PlayNote(i,10);
    PlayNote(200,100);

    Delay(32767L);

    for (i=1;i<21;i++)
	PlayNote(40,50);

    Delay(32767L);

    for (i=1200;i>350;i-=50)
	PlayNote(i,30);

    for (i=1200;i>350;i-=50)
	PlayNote(i,30);

    for (i=1200;i>350;i-=50)
	PlayNote(i,30);


}
