/*      Front End Plot Utility Routines        File: DRAWLIB.C
        Robert L. Patton, Jr.
        1713 Parkcrest Terrace
        Arlington, TX 76012
           04-30-89
*/
#include <math.h>
#include "PLOX.H"
#include "DRAWLIB.H"
#include "MATHLIB.H"
#include "PXLIB.H"
#define  AX Scale->

void Annote (X, Y, Offset, Dir, Mode, Side, Word, Font)
/*   ====== */
/*  Draws upright text at a given offset from
    a point on a line in a given direction.   */

int  X,Y;     /* coordinates of point, pixels */
int  Offset;  /* distance from line, pixels   */
char Dir;     /* line direction, 'H' or 'V'   */
int  Mode;    /* 0 = text parallel to line centered on point
                 1 = text perpendicular, ended at point     */
int  Side;    /* 1 = clockwise, -1 = ccw      */
char *Word;   /* the text string              */
int  Font;    /* predefined character set number */
{
  int  S, C, Tlen, Xt, Yt, Adjust, Pica;

  Pica = TxHigh (Font);
  Tlen = TxWide (strlen (Word), Font);
  S = Isin(Dir);
  C = Icos(Dir);
  Xt = X + Offset * S * Side;
  Yt = Y - Offset * C * Side;
  if (Mode) {
    Adjust = Tlen * (1 - Side);
    Xt += (C*Pica - S*Adjust) / 2;
    Yt -= (S*Pica + C*Adjust) / 2;
  }
  else {
    Xt -= (C*Tlen - S*Pica) / 2;
    Yt -= (S*Tlen + C*Pica) / 2;
  }
  PX_Text (Xt, Yt, Font, 'H', Word);
}

void AxNum (Xa, Ya, Dir, Side, Span, Scale, Boxes)
/*   ===== */
/*            Draws a standard numeric axis */
int     Xa,Ya; /* start coordinates, pixels      */
char    Dir;   /* direction, 'H' or 'V'          */
int     Side;  /* side for numbers, 1=CW, -1=CCW */
int     Span;  /* length of axis, pixels         */
SCALING *Scale;/* pointer to scaling values      */
int     Boxes; /* number of divisions            */
{
  double Value;
  char   Digits[30];
  int    Bump, C, Font, Mode, S, X, Y;
  static unsigned Flag=2, Left=0, Right=1;
  #ifdef DEBUG
  printf("AxNum:Xa=%d,Ya=%d,Dir=%c,Side=%d,Span=%d,Boxes=%d\n",
          Xa,Ya,Dir,Side,Span,Boxes);
          ShowScale(Dir,Boxes,Scale);
  #endif
  Font = 1;              /* optional font later */
  S = Isin(Dir);
  C = Icos(Dir);
  Mode = (Dir == 'V');
  PX_Move (Xa, Ya);
  PX_Draw (Xa+Span*C, Ya+Span*S);
  X = Xa;
  Y = Ya;
  for (Value=AX Start; Value<=(AX Start+Boxes*AX DeltaB); Value+=AX DeltaB)
  {
    Bump = (int) ((Value-AX Start)*AX DeltaP+.5);
    X = Xa+C*Bump;
    Y = Ya+S*Bump;
    TicMark (X, Y, 3, Dir, Side);
    Ftoa (Value, Digits);
    Annote (X, Y, 8-3*Mode, Dir, Mode, Side, Digits, Font);
  }
}
/*@@*/
void Bar (X1, Y1, X2, Y2, Dir, Width, Hatch)
/*   ===   Draws a rectangular bar  */

int X1,Y1, /* center of one end   */
    X2,Y2, /* center of other end */
    Width, /* width in pixels     */
    Hatch; /* hatching pattern    */
char  Dir; /* direction, H or V   */
{
  int Xa, Ya, Xc, Yc, Xhalf, Yhalf;

  Xhalf = (Width * Isin(Dir)) >> 1;
  Yhalf = (Width * Icos(Dir)) >> 1;
  Xa = X1 + Xhalf;
  Ya = Y1 - Yhalf; /* diagonally */
  Xc = X2 - Xhalf; /* opposite   */
  Yc = Y2 + Yhalf; /* corners    */
  if (Hatch!=0) PX_Hatch (Xa,Ya,Xc,Yc);
  PX_Move (Xa,Ya);
  PX_Draw (Xa,Yc);
  PX_Draw (Xc,Yc);
  PX_Draw (Xc,Ya);
  PX_Draw (Xa,Ya);
}

void Box (X1,Y1,X2,Y2)
/*   ===              Draw a rectangle */
int X1,Y1,X2,Y2;
{
  PX_Move(X1,Y1);
  PX_Draw(X2,Y1);
  PX_Draw(X2,Y2);
  PX_Draw(X1,Y2);
  PX_Draw(X1,Y1);
}

void BoxTry (Span,MinBox,MaxBox)
/*   ======                      Choose the minimum and maximum   */
int Span, *MinBox, *MaxBox;   /* number of scale divisions to try */
{                             /* for any given span in pixels.    */
  if (Span<(1.5*INCH)) {
    *MinBox = 2;
    *MaxBox = 2;
  }
  else {
    *MinBox = Span/INCH;
    if (*MinBox<3) *MinBox = 3;
    *MaxBox = 2*( *MinBox);
  }
}

void CopyScale (From, To)
/*   =========       Makes a copy of a SCALING value */
SCALING *From, *To;
{
  To->Start  = From->Start;
  To->DeltaB = From->DeltaB;
  To->DeltaP = From->DeltaP;
}

void Fit (Min, Max, Boxes, Start, Delta)
/*   === */
int    Boxes;          /* Finds offset and units/division for   */
double Min,Max;        /* a given range and number of divisions */
double *Start,*Delta;  /* ensuring 1,2,4, or 5 X 10**N per division */
{
  double Divs;
  int  Trial=0;

  Divs = (double) Boxes;
  *Delta = 0.5 * Exp10 ((int) floor ((log10 (Max-Min)/Divs)) );
  do
  {
    Trial += 1;
    if (Trial%4 == 3)
         *Delta = 1.25 * *Delta;
    else *Delta = 2.00 * *Delta;
    *Start = *Delta * floor (Min / *Delta);
  }
  while (*Start + Divs * *Delta < Max);
}
/*@@*/
int IsHue (char *Word) {
 /* =====              Returns the color number if Word is a
                       microPLOX color.  Otherwise zero.
*/
#define HUES  15
#define HUEBUF 8
char *HueList[HUES] = {"blue  " ,"pea"   ,"aqua"  ,"coral" ,"plum",
                       "umber"  ,"chalk" ,"gray"  ,"azure" ,"lime",
                       "cyan"   ,"peach" ,"lilac" ,"lemon" ,"white"};
int HueNum;
  HueNum = iKeyFind (HueList,HUES,Word);
  return (HueNum<0 ? 0 : HueNum);
}

void Liner (Xptr, Xfit, Yptr, Yfit, LineType, PointType)
/*   =====    Draws straight lines between consecutive points in a set
              of X,Y points in arbitrary data units */
float    *Xptr,*Yptr; /* the data points       */
SCALING  *Xfit,*Yfit; /* scaling parameters    */
int      LineType,    /* line pattern, 1=solid */
         PointType;   /* point shape, 0=none   */
{
  int  I, Imax;

  PX_Trail (LineType);
  Imax = (int) *Xptr++;
  Yptr++;
  PX_Move (Scaled(Xptr++,Xfit), Scaled(Yptr++,Yfit) );
  if (PointType) PX_Symbol (PointType);
  for (I=1; I<Imax; I++,Xptr++,Yptr++)
  {
    if(LineType) PX_Draw (Scaled(Xptr,Xfit), Scaled(Yptr,Yfit) );
    else         PX_Move (Scaled(Xptr,Xfit), Scaled(Yptr,Yfit) );
    if (PointType) PX_Symbol (PointType);
  }
  PX_Trail (SOLID);
}

int Pct_Fit (Min, Max, Boxes)
/*  ======= */
/*             Returns a percent fit of the values generated
               by the standard scaling algorithm */
double Min,Max; /* data extremes */
int    Boxes;   /* number of scale divisions */
{
  double Delta, Start;

  Fit (Min, Max, Boxes, &Start, &Delta);
  return ((int) ((Max-Min)*100.0/(Delta*Boxes)+.5));
}

void Scale (Min, Max, Span, Exact, Boxes, Adjust)
/*   ===== */
/*             Provides scaling values for a set of data */
double  Min,Max; /* data extremes                        */
int     Span;    /* length of scale, pixels              */
int     Exact;   /* TRUE for ends of scale = Min and Max */
int     *Boxes;  /* number of scale divisions IN OUT     */
SCALING *Adjust; /* the scaling values returned          */
{
  double floor();
  int    BestFit,I,MinBox,MaxBox,ThisFit;
  if (Exact) {
    if (*Boxes) {
      Adjust->Start = Min;
      Adjust->DeltaB = (Max-Min)/(*Boxes);
    }
    else {      /* choose Boxes until 100% fit found */
      BoxTry(Span,&MinBox,&MaxBox);
      for (*Boxes=MinBox; *Boxes<=MaxBox; *Boxes+=1)
        if ((ThisFit=Pct_Fit(Min,Max,*Boxes))==100) break;
      if (*Boxes<=MaxBox)
        Fit(Min,Max,*Boxes,&Adjust->Start,&Adjust->DeltaB);
      else {
        *Boxes = 1;
        Adjust->Start = Min;
        Adjust->DeltaB = Max-Min;
      }
    }
  }
  else {   /* NOT Exact */
    if (*Boxes<=0) {
      BestFit = 0;
      *Boxes = 1;
      BoxTry(Span,&MinBox,&MaxBox);
      for (I=MinBox; I<=MaxBox; I++)
        if ((ThisFit=Pct_Fit(Min,Max,I))>BestFit) {
          *Boxes = I;
          BestFit = ThisFit;
        }
    }
    Fit(Min,Max,*Boxes,&Adjust->Start,&Adjust->DeltaB);
  }
  Adjust->DeltaP = (double) Span/(Adjust->DeltaB*(*Boxes));
}

int Scaled (Vptr, Vfit)
/*  ======             Returns the scaled pixel
                       location of a data value */
float   *Vptr; /* data value     */
SCALING *Vfit; /* scaling params */
{
  return ( (int) ((*Vptr - Vfit->Start) * Vfit->DeltaP + .5) );
}

void ShowScale (Dir,Boxes,Fit)
/*   =========     Displays scaling values */
char    Dir;   /* direction char (X,Y)      */
int     Boxes; /* number of scale divisions */
SCALING *Fit;  /* ptr to scaling record     */
{
  printf("--The %c scaling uses %d boxes and\n",Dir,Boxes);
  printf("    %f start value\n",Fit->Start);
  printf("    %f data units per box\n",Fit->DeltaB);
  printf("    %f pixels per data unit\n",Fit->DeltaP);
}

void TicMark (Xt, Yt, TicLen, Dir, Side)
/*   ======= */
/*   Draws a tic mark perpendicular to a line
     at a given point The pen is left at Xt, Yt. */

int  Xt,Yt;  /* point on the line, pixels     */
int  TicLen; /* length of tic, pixels         */
char Dir;    /* direction of line, 'H' or 'V' */
int  Side;   /* 1=Cw, -1=CCw, 0=both          */
{
  int  C, D, S, Xe, Ye;

  C = Icos(Dir);
  S = Isin(Dir);
  if (Side==0) {
    D = -1;
    Xe = Xt + TicLen*S;
    Ye = Yt - TicLen*C;
  }
  else {
    D = Sign(Side);
    Xe = Xt;
    Ye = Yt;
  }
  PX_Move(Xt+TicLen*S*D, Yt-TicLen*C*D);
  PX_Draw(Xe, Ye);
  if (Side==0) PX_Move(Xt, Yt);
}

int TxHigh (Font)
/*  ====== */
int Font;   /* returns pixel height of a given font */
{
  switch (Font)
  {
    case  1: return 5;
    case  2: return 6;
    default: return 0;
  }
}

int TxWide (Nchars, Font)
/*  ====== */
int Nchars,Font;   /* returns pixel width of a given number */
{                  /* of characters of a given font */
  switch (Font)
  {
    case  1: return (5*Nchars - 2);
    case  2: return (7*Nchars - 2);
    default: return 0;
  }
}
