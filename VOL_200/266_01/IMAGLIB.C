/*            In-memory image managing library        FILE: IMAGLIB.C
                 for pixel printer devices
                 Robert L. (Bob) Patton, Jr.
                   1713 Parkcrest Terrace
                    Arlington, TX 76012
                       27 April 1990
*/
#include "PLOX.H"
#include "lptr.h"

#define VSIZE      (YMAX+7)/8
static char Image[XMAX+1][VSIZE];

void Dot (X,Y)
/*   ===      Turns on the pixel at X,Y  */
int X,Y;
{
  int V;
  if (InBounds(X,Y)) {
    V = YMAX/8 - Y/8;
    Image[X][V] = Image[X][V] | Bit(Y%8);
  }
}

/*
     This version of Draw is for Epson compatible printer devices
*/
void Draw(Xmax,Ymax,Bold)
/*   ====  Draws the memory image on the printer.  */
int  Xmax,Ymax,Bold;
#define GRAPHICS  8
#define NORMAL   12
#define CR       13
{
  unsigned I,J;
  Ymax=(Ymax+7)/8;
  LP_LineSpace(GRAPHICS);
  LP_CrLf(1);
  if (Xmax<=0 || Xmax>XMAX) Xmax=XMAX;
  for (J=((Ymax<=0)?0:VSIZE-Ymax); J<VSIZE; J++)
  {
    LP_GraphMode(Xmax+1);
    for (I=0; I<=Xmax; I++)
      LP_Send(Image [I][J]);
    if (Bold) {
      LP_Send(CR);
      LP_GraphMode(Xmax+1);
      for (I=0; I<=Xmax; I++)
        LP_Send(Image[I][J]);
    }
    LP_CrLf(1);
  }
  LP_LineSpace(NORMAL);
  LP_Reset();
  LP_CrLf(1);
}

void Line (X1,Y1,X2,Y2)
/*   ====               Draws a straight line */
int  X1,Y1,X2,Y2;
{
  int Dx, Dy, D1x, D1y, D2x, D2y, M, N, S, K;

  Dx = X2 - X1;
  Dy = Y2 - Y1;
  D1x = Sign(Dx);
  D1y = Sign(Dy);
  D2x = D1x;
  D2y = 0;
  M = abs(Dx);
  N = abs(Dy);
  if (M <= N) {
    D2x = 0;
    D2y = D1y;
    M = abs(Dy);
    N = abs(Dx);
  }
  S = M/2;
  for (K=0; K<=M; K++)
  {
    if (Dotter())  Dot (X1,Y1);
    S += N;
    if (S >= M) {
      S -= M;
      X1 += D1x;
      Y1 += D1y;
    }
    else {
      X1 += D2x;
      Y1 += D2y;
    }
  }
}

void NewImage()
/*   ========   Clears the plot image (to binary zeros) */
{
unsigned K,Kmax;
char    *Image_Ptr;
  Image_Ptr=&Image[0];
  Kmax=(XMAX+1)*(VSIZE);
  for (K=0; K<Kmax; K++,Image_Ptr++)
    *Image_Ptr = 0;
}

void NoDot (X,Y)
/*   =====      Turns off the pixel at X,Y  */
int X,Y;
{
  int V;
  if (InBounds(X,Y)) {
    V = YMAX/8 - Y/8;
    Image[X][V] = Image[X][V] & (~Bit(Y%8));
  }
}

