/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   HELPDEMO.C   ***************************/

#include "mydef.h"

/* link with readhelp.c */

start(){
extern struct screen_structure scr;
extern struct window_structure w[];
extern struct help_structure hlp;

char ch;
char ext;
char *ptr;

strcpy(hlp.filename,"helpdemo.hlp");
hlp.page=1;
hlp.interior_attr=scr.inverse;
hlp.frame_attr=scr.inverse;
cls();
print(1,1,"Press F1 to see help page 1, any other key to continue.");
get_key(&ch,&ch);

hlp.page=2;
print(1,4,"Press F1 to see help page 2, any other key to continue.");
get_key(&ch,&ch);

print (1,6,"bye");
return (0);
}
