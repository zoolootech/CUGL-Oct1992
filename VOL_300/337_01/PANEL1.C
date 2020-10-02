/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   PANEL1.C   ***************************/

/* demonstrate the creation of three window panels */

#include "mydef.h"   /* always include this */
#include <stddef.h>  /* we need the definition of NULL from here */


int start(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int top, middle,bottom;

 cls();           /* clear initial window */

 alt_screen(ON);  /* let's draw the windows off screen */

 /* calculate the window sizes to fit true column and row */
 /* don't assume 80x25 */

 top=win_make(2,2,scr.columns-2,1,TOP_FRAME,"",scr.normal,
              scr.normal);
 print(1,1,"test panel 1");

middle=win_make(2,4,scr.columns-2,scr.rows-6,MIDDLE_FRAME,"",
                scr.normal,scr.normal);
print(1,1,"test panel 2");

bottom=win_make(2,scr.rows-1,scr.columns-2,1,BOTTOM_FRAME,"",
                scr.normal,scr.normal);
print(1,1,"test panel 3");

win_pop_top(middle);  /* make middle window topmost */

alt_screen(OFF); /* show the finished screen */

return (0);
}
