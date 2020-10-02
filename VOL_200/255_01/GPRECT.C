
gprect(x2,y2)
  int x2,y2;
{
  extern int gdcur_x, gdcur_y;

  int x1,y1;

  x1 = gdcur_x;
  y1 = gdcur_y;

  gpmove(x1,y1);
  gpline(x1,y2);
  gpline(x2,y2);
  gpline(x2,y1);
  gpline(x1,y1);

}
