/*                                                                            */
/*      EGA Graphic Primitive for Turbo Pascal 3.01A, Version 01FEB86.        */
/*      (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518     */
/*                                                                            */
/*      Description: Set the current style for line drawing primitives.       */
/*      Setting the shading will set the shading flag on and setting the      */
/*      current color will reset it zero. If the shading flag is zero then    */
/*      the current color will be used, else the line style.                  */
/*                                                                            */

void GPSHADE(shade)
  char *shade;
{
  extern char *GDSHADE;
  extern char GDSHAD1[64];

  int  mx,my,x,y,x1,y1,i;

  GDSHADE = shade;

  mx = shade[0];
  my = shade[1];

  x  = 0;
  y  = 0;
  x1 = 0;
  y1 = 0;

  for (i = 0; i < 64; i++)
    {
    if (x1 == 8)
      {
      x1 = 0;
      y1 = y1 + 1;
      y  = y + 1;
      };

    if (x == mx)
      x = 0;

    if (y == my)
      y = 0;

    GDSHAD1[y1*8+x1+2] = shade[y*mx+x+2];

    x  = x + 1;
    x1 = x1 + 1;

    };

  GDSHAD1[0] = 8;
  GDSHAD1[1] = 8;

  GPSHAD1();

}
