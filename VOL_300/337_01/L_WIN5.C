/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_WIN5.C   ***************************/

#include "mydef.h"


/*****************************************************************

 Usage: int win_what_attr(int handle);

 Handle= window handle;

 Returns the screen attribute of "handle"

*****************************************************************/

char win_what_attr(int handle)
{
extern struct screen_structure scr;
extern struct window_structure w[];

return (w[handle].attribute); /* return the attribute */
}


/*****************************************************************

 Usage: void win_set_attr(int handle, char attribute);

 Handle= window handle;
 Attribute = text attribute;

 Sets the default text attribute of the window "handle" to
 "attribute".

*****************************************************************/

void win_set_attr(int handle,char attribute)
{
extern struct screen_structure scr;
extern struct window_structure w[];

 w[handle].attribute = attribute;
}
