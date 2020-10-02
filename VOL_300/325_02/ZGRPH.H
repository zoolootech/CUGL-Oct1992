#ifndef GRPH_H
    #define GRPH_H

#ifdef __cplusplus
    extern "C" {
#endif

#define Word	unsigned int
#define Byte	unsigned char

#define     Mode640x350x16   0x10	/* Corresponds to Mode 0 */
#define     Mode640x480x16   0x12	/* Corresponds to Mode 1 */
#define     Mode320x200x256  0x13	/* Corresponds to Mode 2 */
#define     Mode640x400x256  0x5c	/* Corresponds to Mode 3 */
#define     Mode640x480x256  0x5d	/* Corresponds to Mode 4 */
#define     Mode800x600x16   0x5b	/* Corresponds to Mode 5 */
#define     Mode1024x768x16  0x5f	/* Corresponds to Mode 6 */

#define Max_Points	400
#define Max_Fill_Buffer 3500

struct	VP
{
    int x1;
    int y1;
    int x2;
    int y2;
};

struct	RGBstruct
{
    Byte    Red;
    Byte    Green;
    Byte    Blue;
};

extern	int	far pascal OpenGraph(Byte,Byte);
extern	void	far pascal CloseGraph(void);
extern	void	far pascal SetFillPattern(Byte,Byte,Byte,Byte far *);
extern	void	far pascal GetFillPattern(Byte *,Byte *,Byte *,Byte far *);

extern	void	far pascal SetViewPort(struct VP);
extern	void	far pascal GetViewPort(struct VP *);

extern	int	far pascal GetVMode(void);
extern	void	far pascal SetXLen(Word);

extern	void	(far pascal *GetImage)(int,int,int,int,void far *);
extern	void	(far pascal *PutImage)(int,int,void  far *);
extern	void	(far pascal *CopyImage)(int,int,int,int,int,int);
extern	void	(far pascal *CopyImageRev)(int,int,int,int,int,int);
extern	int	far pascal SaveImage(void *,void far *,Word);
extern	int	far pascal LoadImage(void *,void far *,Word);
extern	int	far pascal PolyLineD(Word,int far *,int far *);
extern	void	far pascal Rectangl(int,int,int,int);

extern	void	far pascal EllipseD(int,int,int,int);

extern	void	far pascal FillPolyD(Word, int far *,int far *);
extern	void	far pascal FillArea(int,int,int);
extern	void	(far pascal *FillRectangle)(int,int,int,int);
extern	void	far pascal FillAgain(void);

extern	void	far pascal SetPalette(int,int);
extern	Byte	far pascal GetPalette(int);
extern	void	far pascal SetPalettes(void  far *);
extern	void	far pascal GetPalettes(void  far *);
extern	void	far pascal SetClrReg(void  far *,int);
extern	void	far pascal GetClrReg(void  far *,int);
extern	void	far pascal SetRGBDAC(int,int,void  far *);
extern	void	far pascal GetRGBDAC(int,int,void  far *);
extern	void	far pascal GreyScale(int,int);
extern	void	far pascal SelectDAC(int);
extern	void	far pascal DefaultPalette(char);

extern	void	far pascal ClearScreen(void);
extern	void	far pascal SetOrigin(int,int);
extern	void	far pascal SplitScreen(int);
extern	void	far pascal SScroll(int,int);

extern	void	(far pascal *SetGC)(void);
extern	void	(far pascal *ReSetGC)(void);
extern	int	far pascal SetVdOn(void);
extern	int	far pascal SetVdOff(void);
extern	Byte	(far pascal *GetPixel)(int,int);
extern	void	(far pascal *SetPixel)(int,int);

extern	void	(far pascal *LineD)(int,int,int,int);

extern	int	far pascal  ImageSize(int,int,int,int);
#define ImageSize(x1,y1,x2,y2) ((abs(x2-x1)+1)*(abs(y2-y1)+1)+5)

extern	Byte	near SysPllts[17];

extern	Byte	near WrtMd;
extern	Byte	near ClipTrue;

extern	Byte	near PxlClr;

extern	Byte	near LineStyle;

extern	Byte	near EllpsStyle;

extern	Byte	near FllPttrn[8];
extern	Byte	near FllClr;
extern	Byte	near FllPttrnIndxX;
extern	Byte	near FllPttrnIndxY;

extern	int	near XMax;
extern	int	near YMax;
extern	int	near XRes;
extern	int	near YRes;
extern	int	near XOrgn;
extern	int	near YOrgn;
extern	int	near XLen;

extern float   near rpi;

extern float  near stvwx,stvwy;
extern float  near Wrldxy[8];

extern	Byte  near _ZMode;

#define SetClipOn()   ClipTrue=1
#define SetClipOff()  ClipTrue=0
#define GetClip()     ClipTrue

#define SetDraw(Mode)	WrtMd=(Mode << 3);SetGC()
#define GetDraw()	(WrtMd >> 3)

#define SetLineStyle(Styl)   LineStyle=Styl
#define GetLineStyle(Styl)   Styl=LineStyle

#define SetEllipseStyle(Styl)	  EllpsStyle=Styl
#define GetEllipseStyle(Styl)	  Styl=EllpsStyle

extern	void	far pascal  WindScrollUp(int,int,int,int,int);
extern	void	far pascal  WindScrollDown(int,int,int,int,int);
extern	void	far pascal  WindScrollLeft(int,int,int,int,int);
extern	void	far pascal  WindScrollRight(int,int,int,int,int);

#define WindScrollUp(x1,y1,x2,y2,l)  CopyImage(x1,y1+l,x2,y2,x1,y1)
#define WindScrollDown(x1,y1,x2,y2,l) CopyImageRev(x1,y1,x2,y2-l,x1,y1+l)
#define WindScrollLeft(x1,y1,x2,y2,l)  CopyImage(x1+l,y1,x2,y2,x1,y1)
#define WindScrollRight(x1,y1,x2,y2,l) CopyImageRev(x1,y1,x2-l,y2,x1+l,y1)

#ifdef __cplusplus
    }

#endif

#endif
