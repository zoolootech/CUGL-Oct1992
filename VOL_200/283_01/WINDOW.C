/* window.c -- 9/5/88, d.c.oshel
   */

#include "vidinit.h"


/* read current window dimensions */
void getwindow( int *tx, int *ty, int *bx, int *by )
{
    *tx = lm;
    *ty = tm;
    *bx = rm;
    *by = bm;
}


void setwindow( int tx, int ty, int bx, int by )
{
   if (bx > BTMX) bx = BTMX;
   if (tx < TOPX) tx = TOPX;
   if (by > BTMY) by = BTMY;
   if (ty < TOPY) ty = TOPY;

   if (tx >= bx || ty >= by)  /* absurd values? set full screen reduced by 1 */
   { 
       putch(7); /* complain but continue */
       lm = TOPX + 1; rm = BTMX - 1; tm = TOPY + 1; bm = BTMY - 1; 
   } 
   else
   {
       lm = tx;
       rm = bx;
       tm = ty;
       bm = by;
   }
}






void clrwindow( void )
{
    MSJ_ClrRegion( tm, lm, bm, rm, vid_attr );
    gotoxy(0,0);
}


