/*                                                                            */
/*      EGA Graphic Demonstration, Turbo Pascal 3.01A, Version 10JAN86.       */
/*      (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518     */
/*                                                                            */
/*      This program will display over a hundred different colors by using    */
/*      a bit color map.  Only the standard palette setting are used, if      */
/*      changed, more colors could be produced (but not at the same time).    */
/*                                                                            */

#include <mcega.h>

main()
{
  char buffer[640];

  register x,y;

  GPParms();

  GPInit();

  gpinitwt(0,0,640,350,buffer,0);

  for (y = 0; y < GDMAXROW; y++)
    {
    for (x = 0; x < GDMAXCOL; x += 2)
      {
      buffer[x]   = y * GDMAXPAL / GDMAXROW;
      buffer[x+1] = x / 40;
      };

    gpwritrw();
    y++;

    for (x = 0; x < GDMAXCOL; x += 2)
      {
      buffer[x]   = x / 40;
      buffer[x+1] = y * GDMAXPAL / GDMAXROW;
      };

    gpwritrw();
    };

  gptermwt();

  getch();

  GPTerm();

}
