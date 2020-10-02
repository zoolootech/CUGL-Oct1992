gpamtgd(image,width,height,length)
  unsigned char far *image;
  unsigned int  width,height,length;
{
  extern unsigned int  gdvw_x1,gdvw_x2,gdvw_x3;
  extern unsigned int  gdvw_y1,gdvw_y2,gdvw_y3;
  extern unsigned int  Dither4[4][4];

  unsigned char xscale[640];
  unsigned char yscale[350];
  unsigned char row[640];

  int  w,h,x,y,c;

  int  xi,xc,xs,yi,yc,ys;

  gpcolor(0);
  gpmove(gdvw_x1,gdvw_y1);
  gpbox(gdvw_x2,gdvw_y2);

  /* The dimension to our idea image is width x height.  Determine the largest
     possible real image dimension for the current viewport. */

  if (((long)gdvw_y3 * (long)width * 4L) / (height * 3) <= gdvw_x3)
    {
    w = ((long)gdvw_y3 * (long)width * 4L) / (height * 3);
    h = gdvw_y3;
    }
  else
    {
    w = gdvw_x3;
    h = ((long)height * (long)gdvw_x3 * 3L) / (width * 4);
    };

  /* Generate X and Y scale tables */

  gpslope(xscale,width,w);
  gpslope(yscale,height,h);

  y = 0;
  for (ys = 0; ys < height; ys++)
    {
    if ((yc = yscale[ys]) != 0)
      {
      while (yc--)
        {
        x  = 0;
        xi = ys * length;
        for (xs = 0; xs < width; xs++, xi++)
          {
          if ((xc = xscale[xs]) != 0)
            {
            while (xc--)
              {
              c = image[xi];
              row[x++] = c / 16 + ( c % 16 > Dither4[y % 4][x % 4]) + 8;
           /* row[x++] = c / 13 + 8; */
              }
            }
          }
        gpmove(gdvw_x1,gdvw_y1+y);
        gpwtrow(row,w);
        y++;
        }
      }

    }

}
