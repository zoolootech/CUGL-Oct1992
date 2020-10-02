/*                                                                            */
/*      EGA Graphic Primitive for Turbo Pascal 3.01A, Version 01FEB86.        */
/*      (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518     */
/*                                                                            */
/*      Description: Set the current style for line drawing primitives.       */
/*      Setting the shading will set the shading flag on and setting the      */
/*      current color will reset it zero. If the shading flag is zero then    */
/*      the current color will be used, else the line style.                  */
/*                                                                            */

void GPFILL(color)
  int color;
{
  GPFLOOD(color);
}
