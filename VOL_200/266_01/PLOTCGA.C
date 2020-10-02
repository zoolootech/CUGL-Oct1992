/* MicroPLOX plot routine
   copyright Robert L. Patton, Jr. 15 Apr 1990
   A device driver for CGA high resolution monochrome graphics.
   The coordinates are ajusted to fill the screen without trying
   to match the original aspect ratio exactly.
   This does give a quick way to get a preview image on the screen
   although text may not be exactly aligned as it would be on a
   printed image.
      WITH PIXLIB                              File: PLOTCGA.C
*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>
#include "PLOX.H"
/* -------------------------------------------------------- */
#define DX(A) (A*5)/4 /* adjust x coordinate to fill screen */
#define DY(B) (B*5)/9 /* adjust y coordinate to fill screen */
#define NORMAL  1  /* default white */
#define TVON    _setvideomode(_HRESBW)
/* -------------------------------------------------------- */
#define TVOFF    _setvideomode(_DEFAULTMODE)
#define YMAXDY   DY(YMAX)
#define DOT(A,B) _setpixel((A),(YMAXDY-B))
#define NODOT(A,B) {Color=_setcolor(0); \
                   _setpixel((A),(YMAXDY-B));_setcolor(Color);}
#define NEWHUE(A)
#include "monitor.c"
