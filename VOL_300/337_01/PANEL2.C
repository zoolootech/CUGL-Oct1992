/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   PANEL2.C   ***************************/


/*
Demonstrate the creation of three window panels.
The panels are unframed with the upper and lower panels
displayed in inverse.
The upper and lower panels occupy one line.
*/

#include "mydef.h"   /* always include this */
#include <stddef.h>  /* we need the definition of NULL from here */

int start(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int top, middle, bottom;


   cls();           /* clear initial window */
   alt_screen(ON);  /* let's draw the windows off screen */

     /* calculate the window sizes to fit true column and row */
     /* don't assume 80x25 */

     top=win_make(1,1,scr.columns,1,NO_FRAME,"",scr.normal,
                  scr.inverse);
        print(1,1,"test panel 1");

     middle=win_make(1,2,scr.columns,scr.rows-2,"","",scr.normal,
                     scr.normal);
        print(1,1,"test panel 2");

     bottom=win_make(2,scr.rows,scr.columns,1,"","",scr.normal,
                     scr.inverse);
        print(1,1,"test panel 3");

     win_pop_top(middle);  /* make middle window topmost */

  alt_screen(OFF); /* show the finished screen */

return(0);
}
