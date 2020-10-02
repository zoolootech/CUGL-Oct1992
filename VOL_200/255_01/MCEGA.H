/******************************************************************************/
/*  EGA.H								      */
/*									      */
/*									      */
/******************************************************************************/

#define BELL	 007
#define BLANK    ' '

#define BLACK	 0x00
#define BLUE	 0x01
#define GREEN	 0x02
#define CYAN	 0x03
#define RED	 0x04
#define MAGENTA  0x05
#define BROWN	 0x06
#define WHITE	 0x07
#define GRAY	 0x08
#define LBLUE	 0x09
#define LGREEN	 0x0A
#define LCYAN	 0x0B
#define LRED	 0x0C
#define LMAGENTA 0x0D
#define YELLOW	 0x0E
#define IWHITE	 0x0F

#define ATTRIBUTE(b,f) ((unsigned char)(b * 16 | f ))

extern int  GDTYPE;
extern int  GDOTHER;
extern int  GDMEMORY;
extern int  GDMAXCOL;
extern int  GDMAXROW;
extern int  GDMAXPAL;
extern int  GDMAXCLR;
extern int  GDCOLOR;
extern int  GDASPC1;
extern int  GDASPC2;
extern int  GDCUR_X;
extern int  GDCUR_Y;

extern char GDC_FLG;
extern char GDS_FLG;
extern char GDW_FLG;

extern int  GDCURX1;
extern int  GDCURY1;
