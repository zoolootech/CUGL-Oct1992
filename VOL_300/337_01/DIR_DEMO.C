/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   DIR-DEMO.C   ***************************/

#include "mydef.h"
#include <stddef.h>


int start(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int dir_win;        /* handle for directory window */
char selection[13]; /* string to receive the name of file selected */

cls();

dir_win= win_make(35,2,12,20,STD_FRAME,"",scr.normal,scr.normal);

dir("*.*",selection);
win_delete(dir_win);

if (strlen(selection)==0) print (1,1, "No file selected.");
 else
  print(1,1,"The selected file was "); print_here(selection);

return(0);
}
