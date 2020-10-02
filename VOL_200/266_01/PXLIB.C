/*       Front End Plot Drawing Routines      File: PXLIB.C
            08-30-87
         WITH MATHLIB
*/
#include "PLOX.H"
#include "stdio.h"
#include "PXLIB.H"
#define SWAP(A,B) {A=A^B;B=B^A;A=A^B;}
#define putw(W,F) fwrite(&W,sizeof(int),1,F) /* for ANSI C compatibility */

FILE   *Plot;

void PX_Close (X,Y)
/*   ========      Close the plot file. */
int X,Y;
{
  putc(CLOS,Plot);
  putw(X,Plot);
  putw(Y,Plot);
  fclose(Plot);
}

void PX_Dot (X,Y)
/*   ======        Turn on X,Y pixel. */
int X,Y;
{
  putc(ON,Plot);
  putw(X,Plot);
  putw(Y,Plot);
}

void PX_Draw (X,Y)
/*   =======       Draw vector to X,Y pixel loc. */
int X,Y;
{
  putc(MARK,Plot);
  putw(X,Plot);
  putw(Y,Plot);
}

void PX_Hue (int Color) {
/*   ======                Set current color */
  putc(HUE,Plot);
  putw(Color,Plot);
}

void PX_Hatch (X1,Y1,X2,Y2)
/*   ========              Hatch an area */
int X1,Y1,X2,Y2;
{
  if (X1>X2) SWAP(X1,X2)
  if (Y1>Y2) SWAP(Y1,Y2)
  putc(FILL,Plot);
  putw(X1,Plot);
  putw(Y1,Plot);
  putw(X2,Plot);
  putw(Y2,Plot);
}

void PX_Margin (Column)
/*   =========         Set plot left margin */
int Column;
{
  putc(MRGN,Plot);
  putw(Column,Plot);
}

void PX_Move (X,Y)
/*   =======       Move to X,Y pixel loc. */
int X,Y;
{
  putc(MOVE,Plot);
  putw(X,Plot);
  putw(Y,Plot);
}

void PX_Open()
/*   =======   Open the plot file */
{
  Plot = fopen("PLOTCOM.DAT","wb");
}

void PX_Origin (X,Y)
/*   =========      Set new plot origin */
int X,Y;
{
  putc(ORIG,Plot);
  putw(X,Plot);
  putw(Y,Plot);
}
/*@@*/
void PX_Style (Type,Spacing)
/*   ========               Set hatching pattern */
int Type,Spacing;
{
  putc(HTYP,Plot);
  putw(Type,Plot);
  putw(Spacing,Plot);
}

void PX_Symbol (Shape)
/*   =========      Draw symmetrical, centered symbol */
int Shape;       /* at current location */
{
  putc(SYMB,Plot);
  putc(Shape,Plot);
}

void PX_Text (X,Y,Size,Dir,Text_Ptr)
/*   =======      Draws a string of text */

int  X,Y;       /* lower, left corner of text start */
int  Size;      /* font number (1 or 2)             */
char Dir;       /* 'H' = horizontal, 'V' = vertical */
char *Text_Ptr; /* the text string                  */
{
  if (Size==2) putc(TX2H+(Dir=='V'),Plot);
  else         putc(TXTH+(Dir=='V'),Plot);
  putw(X,Plot);
  putw(Y,Plot);
  for (; *Text_Ptr!='\0'; Text_Ptr++)
    putc(*Text_Ptr,Plot);
  putc(0,Plot);
}

void PX_Trail (Mark)
/*   ========       Sets the line type */
int Mark;
{
  putc (LTYP, Plot);
  putc (Mark, Plot);
}
