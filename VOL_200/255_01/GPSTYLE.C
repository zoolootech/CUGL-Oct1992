/*                                                                            */
/*      EGA Graphic Primitive for Turbo Pascal 3.01A, Version 01FEB86.        */
/*      (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518     */
/*                                                                            */
/*      Description: Set the current style for line drawing primitives.       */
/*      Setting the line style will set the line style flag on and setting    */
/*      the current color will reset it zero.  If the line style flag is      */
/*      zero then the current color will be used, else the line style.        */

void GPSTYLE(style)
  char *style;
{
  extern char *GDSTYLE;
  extern char GDS_FLG;

  GDSTYLE = style;
  GDS_FLG = -1;
}
