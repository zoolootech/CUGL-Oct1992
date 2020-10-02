/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_WIN4.C   ***************************/

#include "mydef.h"
#include <dos.h>

#if defined QUICKC

#include "malloc.h"
#include "memory.h"

#endif

#if defined TURBOC

#include "alloc.h"    /* Turbo C header file */
#include "mem.h"
#include "string.h"
#include "stdlib.h"

#endif


/*****************************************************************

 Usage: void win_up(int handle, int amount);

 int handle = window handle
 int amount = the number of lines to move the window.

 Moves the window indicated up on the screen, the screen is
 updated to show the movement.  If there is insufficient space
 to move the requested amount, the window is moved as far as
 possible.

*****************************************************************/

void win_up(int handle, int amount)
{
extern struct  screen_structure scr;
extern struct window_structure w[];
 int win_top;

 /* if the window selected is on top then save */
 if(scr.active==handle)
    win_save();

    /* calculate true top of window.
       this depends on presence of frame */

    if(w[handle].frame==TRUE) win_top=w[handle].top-1;
     else win_top=w[handle].top;

     if(win_top==1) return;  /*return if already on top*/

     /* adjust amount of movement if there is not space */
     if(win_top-amount<=0)amount=win_top-1;

    /* change the window margin definitions to show the changes */
    w[handle].top-= amount;
    w[handle].bottom-= amount;

 win_redraw_all(); /* show the changes */

 if(scr.active==handle){     /* if the window is active */
   free(w[handle].buffer) ;  /* free the memory */
   w[handle].buffer=NULL ;
   update_margins();         /* change the screen margins */
                             /* to reflect the new active */
                             /* window area */
   display_cursor();     /* update cursor position */
 }
}


/*****************************************************************

 Usage: void win_right(int handle, int amount);

 int handle = window handle
 int amount = the number of lines to move the window.

 Moves the window indicated right on the screen, the screen is
 updated to show the movement.  If there is insufficient space
 to move the requested amount, the window is moved as far as
 possible.

*****************************************************************/

void win_right(int handle, int amount)
{
extern struct screen_structure scr;
extern struct window_structure w[];
 int win_right;

 /* if the window selected is on top then save */
 if(scr.active==handle)
    win_save();

   /* calculate true right edge of window.
      this depends on presence of frame */

   if(w[handle].frame==TRUE) win_right=w[handle].right+1;
    else win_right=w[handle].right;

    /*return if already on right border */
    if(win_right==scr.columns) return;  

    if(win_right+amount>=scr.columns)amount=scr.columns-win_right;

    /* change the window margin definitions to show the changes */
    w[handle].right += amount;
    w[handle].left+=amount;

 win_redraw_all();    /* show the changes */

 if(scr.active==handle){  /* if the window is active */
   free(w[handle].buffer) ; /* free the buffer */
   w[handle].buffer=NULL ;
   update_margins();        /* update screen margins
                               to reflect new active
                               window area */
    display_cursor();       /* update cursor position */
 }
}


/*****************************************************************

 Usage: void win_left(int handle, int amount);

 int handle = window handle
 int amount = the number of lines to move the window.

 Moves the window indicated left on the screen, the screen is
 updated to show the movement.  If there is insufficient space
 to move the requested amount, the window is moved as far as
 possible.

*****************************************************************/

void win_left(int handle, int amount)
{
extern struct screen_structure scr;
extern struct window_structure w[];

 int win_left;

 /* if the window selected is on top then save */
 if(scr.active==handle)
    win_save();

    /* calculate true left edge of window.
       this depends on presence of frame */

    if(w[handle].frame==TRUE) win_left=w[handle].left-1;
     else win_left=w[handle].left;

     if(win_left==0) return;  /*return if already on left border */

     if(win_left-amount<=0)amount=win_left-1;

    /* update the window margins to show the move */
    w[handle].left  -= amount;
    w[handle].right -= amount;

 win_redraw_all();   /* update the screen */
 if(scr.active==handle){  /* if the window is active */
   free(w[handle].buffer) ;
   w[handle].buffer=NULL ;
   update_margins();      /* update the screen margins
                             to reflect changes made to
                             the active window  */
    display_cursor();     /* update cursor position */
 }
}


/*****************************************************************

 Usage: void win_down(int handle, int amount);

 int handle = window handle
 int amount = the number of lines to move the window.

 Moves the window indicated down on the screen, the screen is
 updated to show the movement.  If there is insufficient space
 to move the requested amount, the window is moved as far as
 possible.

*****************************************************************/

void win_down(int handle, int amount)
{
int win_bottom;

extern struct screen_structure scr;
extern struct window_structure w[];

 /* if the window selected is on top then save */
 if(scr.active==handle)
    win_save();

    /* calculate true top of window.
       this depends on presence of frame */

    if(w[handle].frame==TRUE) win_bottom=w[handle].bottom+1;
     else win_bottom=w[handle].bottom;

     if(win_bottom==scr.rows) return;  /*return if already on top*/

     if(win_bottom+amount>=scr.rows)amount=scr.rows-win_bottom;

    /* update the window margins to reflect change */
    w[handle].top += amount;
    w[handle].bottom +=amount;

 win_redraw_all(); /* update the screen */
 if(scr.active==handle){  /* if the window is active */
   free(w[handle].buffer) ; /* free the buffer */
   w[handle].buffer=NULL ;
   update_margins();        /* update the screen margins
                               to reflect the new active
                               window area */
    display_cursor();     /* update cursor position */
 }
}


/*****************************************************************

 Usage: void win_insert(int handle, int position);

 int handle = window handle
 int position = relative position within the window stack.

 Places the window indicated by "handle" to the relative screen
 location indicated.  Position 1 would be the first window on top
 of the initial "desktop" window.  The screen is updated to show
 the move.

*****************************************************************/

void win_insert(int handle, int position)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int location,i;

if (handle==0) return;  /* can't move initial window */

if(position==scr.ptr) {  /* if it is moved to the top */
  win_pop_top(handle);   /* pop it */
  return;
}

/* return if move to same location is requested. */
if (handle==scr.list[position]) return; 

 location=win_validate(handle); /* see if window is open */
 if (location==-1) return;        /* exit if not in list */

/* inserting a window requires that the window list be
   rearranged.   The direction of the shift depends on whether the
   window is moved up or down in the stack. */

win_save();  /* save top window before shuffle */

 /* shuffle order */

    /* if the position requested is higher in the stack than the */
    /* actual location, do the following */

     if(position>location)    /* the window is moved up */
       for(i=location;i<position;i++) scr.list[i]=scr.list[i+1];
     else                     /* the window is moved down */
       for(i=location;i>position;i--) scr.list[i]=scr.list[i-1];

 scr.list[position]=handle;
win_redraw_all(); /* redraw the window */
}
