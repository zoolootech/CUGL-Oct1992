/*                                                                            */
/*      EGA Graphic Primitive for Turbo Pascal 3.01A, Version 01Feb86.        */
/*      (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518     */
/*                                                                            */
/*      Description: Read the current color to be used by other graphic       */
/*      primitives.  If the current color is set to -1 then then current      */
/*      line style will be used for drawing lines or shading will be used     */
/*      instead of solid color.                                               */
/*                                                                            */

int GPRDCLR()
{
  extern int GDCOLOR;

  return (GDCOLOR);
}
