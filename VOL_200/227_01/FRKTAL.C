/**
*                           F R K T A L . C
*                           ===============
**/

#include <stdio.h>

#include "graphics.h"

#define HOR MAX_X
#define VER MAX_Y

static float x1 = -.25 /*-0.75*/;  /* Min. x */
static float x9 = 2.;     /* Max. x */
static float y1 = -0.5;    /* Min. y */
static float y9 = 1.;     /* Max. y */
static float zx, zy;
static float x, y;        /* Laufvariablen */
static float xk, yk, xk1, yk1;
static int k, k1;
static int fa;            /* Farbe des Pixel */
static int s = 1000;
static int fs = 2;        /* Farbabstufung */
static int tiefe = 50;    /* Berechnungstiefe */

void main()
{
setmode(0xf);
zx = (x9 - x1) / HOR;
zy = (y9 - y1) / VER;
for(x = x1 ; (x <= x9) /*&& !kbhit()*/ ; x += zx)
  for(y = y1 ; (y <= y9) /*&& !kbhit() */; y += zy)
    {
    xk = x;
    yk = y;
    for(k = 0 ; k <= tiefe ; k++)
      if((xk * xk + yk * yk) > s)
        break;
      else
        {
        xk1 = xk * xk - yk * yk -x;
        yk1 = 2. * xk * yk -y;
        xk = xk1;
        yk = yk1;
        }
    setpixel((int) ((x - x1) / zx), (int) ((y - y1) / zy),
             (k > tiefe) ? 0 : (((int) (k / fs)) + 1) % 3);
    }
getch();
setmode(7);
}
