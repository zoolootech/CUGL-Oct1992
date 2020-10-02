/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   LISTDEMO.C   ***************************/

#include "mydef.h"
#include <stddef.h>

int start(void)      /* start is the entry point */
{
extern struct screen_structure scr;
extern struct window_structure w[];

char string[30];
int select_win;
int selection;
char *ptr[11]= {
                "Apple     ", "Boat      ", "Berry     ",
                "Car       ", "Computer  ", "Denver    ",
                "Donut     ", "Dog       ", "Elephant  ",
                "Egg       ", NULL
                };
cls();

select_win= win_make(1,2,10,5,STD_FRAME,"",scr.normal,scr.normal);

selection=list_select(ptr);

win_delete(select_win);

 if (selection==-1) print(1,1,"No selection made");
  else {
    sprintf(string,"Item number %i was selected",selection);
    print(1,1,string);
  }

return(0);
}
