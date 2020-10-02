/*                                                                            */
/*      EGA Graphic Primitive for Turbo Pascal 3.01A, Version 01FEB86.        */
/*      (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518     */
/*                                                                            */
/*      Description: Set the current window coordinates.                      */
/*                                                                            */

void GPWINDOW(x1,y1,x2,y2)
  int  x1,y1,x2,y2;
{
  extern int  GDWD_X1,GDWD_X2,GDWD_X3;
  extern int  GDWD_Y1,GDWD_Y2,GDWD_Y3;
  extern char GDW_FLG;

  register i;

  if (x1 > x2)
    {
      i  = x1;
      x1 = x2;
      x2 = i;
    };

  if (y1 > y2)
    {
      i  = y1;
      y1 = y2;
      y2 = i;
    };

  GDWD_X1 = x1;
  GDWD_Y1 = y1;
  GDWD_X2 = x2;
  GDWD_Y2 = y2;

  GDWD_X3 = x2 - x1 + 1;
  GDWD_Y3 = y2 - y1 + 1;

  GDW_FLG = 1;

};
