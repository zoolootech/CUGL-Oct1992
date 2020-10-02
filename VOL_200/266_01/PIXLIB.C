/*
   PIXLIB is a file of compiler-independent routines for use in
   device drivers for pixel oriented devices.
                                                  File: PIXLIB.C
*/
#include "plox.h"
#include "pixlib.h"

static int  LineType = -1, Hatch, Spacing;

int Bit (N)
/*  ===    Returns 2 to the N power (i.e., Nth bit on)  */
int N;
{
  int M;
  for (M=1; N>0; --N)
    M = M << 1;
  return M;
}

int Dotter ()
/*  ======        Provides dot patterns for lines */
{
  #define CYCLE 6
  static int LastType=-1, Place=0;
  if (LineType != LastType) {
     Place = 0;
     LastType = LineType;
  }
  else Place = (++Place) % CYCLE;
  
  return ((LineType & Bit (Place))? 1: 0);
}

int GetWord (From)
/*  =======      reads a binary integer */
FILE *From;
{
  int hold;
  fread(&hold,sizeof(int),1,From);
  return (hold);
}
/**/
void HatchHow (Row, Hit, Gap)
/*   ======== */
int Row;          /* Provides dot-on params for given row for  */
int *Hit, *Gap;   /*  the hatching pattern currently in effect */
#define BIGNUM 32767
{
  switch (Hatch) {
                       /* horizontal */
  case 1: *Hit = 0;
          if (!((Row+1)%Spacing)) *Gap = 1;
          else                    *Gap = BIGNUM;
          break;
                       /* vertical */
  case 2: *Hit = 0;
          *Gap = Spacing;
          break;
                         /* left diagonal */
  case 3: *Gap = Spacing;
          *Hit = Row % *Gap;
          break;
                         /* right diagonal */
  case 4: *Gap = Spacing;
          *Hit = (*Gap-1) - (Row%*Gap);
          break;
                         /* square */
  case 5: *Hit = 0;
          if (!((Row+1)%Spacing)) *Gap = 1;
          else                    *Gap = Spacing;
          break;
                         /* blank */
  default: *Hit = 1;
           *Gap = BIGNUM;
           break;
  }
}

int InBounds (int X,int Y)
/*  ======== */
{ return (X>=0 && X<=XMAX && Y>=0 && Y<=YMAX); }

void SetHatch (int Pattern, int Density)
/*   ========        Sets the hatching pattern */
{
  Hatch = Pattern;
  Spacing = Density;
}

void SetLine (int Ltype)
/*   =======              Sets the LineType */
{
  LineType = Ltype;
}
int Sign (int X)
/*  ====          Returns 1 with sign of X  */
{
  if (X < 0) return -1; else return 1;
}
