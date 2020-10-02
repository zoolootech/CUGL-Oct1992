
#include <stdio.h>
#include <mcega.h>

int *open_popup(x2,y2)
  int  x2,y2;
{
  extern int  gdcur_x,gdcur_y;

  int  x1,y1,x3,y3,i;

  register int *sp;

  x1 = gdcur_x;
  y1 = gdcur_y;

  if (x1 > x2)
    {
    i = x2;
    x2 = x1;
    x1 = x2;
    }

  if (y1 > y2)
    {
    i = y2;
    y2 = y1;
    y1 = y2;
    }

  x3 = (x2-x1) + 1;
  y3 = (y2-y1) + 1;

  if ((sp = (int *)calloc(x3 * y3 + (sizeof(unsigned int) * 4), 1)) == 0)
    {
    gpterm();
    abort("Out of memory");
    }

  sp[0] = x1;
  sp[1] = y1;
  sp[2] = x3;
  sp[3] = y3;

  gpmove(x1,y1);
  gpmovgtm(&sp[4],x3,y3,x3);

  gpcolor(0);
  gpbox(x2,y2);

  gpcolor(1);
  gprect(x2,y2);

  return(sp);
}

void close_popup(sp)
  int *sp;
{
  gpmove(sp[0],sp[1]);
  gpmovmtg(&sp[4],sp[2],sp[3],sp[2]);

  free(sp);
}
