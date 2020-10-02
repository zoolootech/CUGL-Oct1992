/*									      */
/*  Program:	 Demo, Version 01/20/86 				      */
/*									      */
/*  Description: Demonstrate calling by EGA routines using FAR calls.	      */
/*									      */
/*  Author:	 Kent Cedola						      */
/*		 2015 Meadow Lake Court, Norfolk VA, 23518. 1-(804)-857-0613  */
/*									      */
/*  Language:	 Microsoft C 4.0+					      */
/*									      */
/*  Comments:	 Must be compiled with option -Ox			      */
/*									      */
/*									      */

#include <stdio.h>

far GPPARMSX()
{
  GPPARMS();
}

far GPINITX()
{
  GPINIT();
}

far GPTERMX()
{
  GPTERM();
}

far GPCOLORX(c)
  int c;
{
  GPCOLOR(c);
}

far GPMOVEX(x, y)
  int x,y;
{
  GPMOVE(x, y);
}

far GPLINEX(x, y)
  int x,y;
{
  GPLINE(x, y);
}

far GPBOXX(x, y)
  int x,y;
{
  GPBOX(x, y);
}
