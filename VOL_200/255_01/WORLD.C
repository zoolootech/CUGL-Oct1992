/*									      */
/*	EGA Graphic Primitive for Microsoft C 3.00, Version 01MAR86.	      */
/*	(C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518     */
/*									      */
/*	Description: This an example of how graphic primitives can be put     */
/*      together to form 'HIGH LEVEL' graphic routines.                       */
/*									      */

#include <mcega.h>

void SetViewport(x1,y1,x2,y2)
  int  x1,y1,x2,y2;
{
  GPVIEWPORT(x1,y1,x2,y2);
}

void SetWindow(x1,y1,x2,y2)
  int  x1,y1,x2,y2;
{
  GPWINDOW(x1,y1,x2,y2);
}

void MovAbs(x,y)
  int x,y;
{
  GDCURX1 = x;
  GDCURY1 = y;
}

void MovRel(x,y)
  int x,y;
{
  GDCURX1 = GDCURX1 + x;
  GDCURY1 = GDCURY1 + y;
}

void LnAbs(x2,y2)
  int x2,y2;
{
  int x1,y1;

  register i;

  x1 = GDCURX1;
  y1 = GDCURY1;
  GDCURX1 = x2;
  GDCURY1 = y2;

  if (GPCLIP2(&x1,&y1,&x2,&y2) != 2)
    {
    GPSCALE(&x1,&y1);
    GPMOVE(x1,y1);
    GPSCALE(&x2,&y2);
    GPLINE(x2,y2);
    };

}

void LnRel(x,y)
  int x,y;
{
  LnAbs(GDCURX1 + x, GDCURY1 + y);
}
