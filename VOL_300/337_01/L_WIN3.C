/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_WIN3.C   ***************************/

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

 Usage: void win_ceol(int handle, int x, int y);

 handle= Window handle.
 x= column.
 y= row.

 Clears the window indicated by handle from the x-y coordinate to
 the end of the line.

*****************************************************************/

void win_ceol(int handle, int x, int y)
{
extern struct screen_structure scr;
extern struct window_structure w[];


  if(w[handle].buffer==NULL){  /* if it is the top window call ceol() */
    ceol(x,y);
    return;
   }

  win_point(handle);  /* direct output to handle */
   ceol(x,y);         /* now we can call ceol()  */
  win_point(handle);  /* a second call to win_point() redirects
                         output to original values.
                         note: the parameter "handle" is ignored */
}


/*****************************************************************

 Usage: void win_cls(int handle);

 handle= Window handle.

 Clears the screen of the window indicated.

*****************************************************************/

void win_cls(int handle)
{
extern struct screen_structure scr;
extern struct window_structure w[];

  /* if it is the top window call cls() */
  if(w[handle].buffer==NULL){  
    cls();
    return;
   }

  win_point(handle);/* direct output to handle */
   cls();           /* output is redirected, we can call cls() now */
  win_point(handle);/* a second call to win_point() redirects
                       output to original values. */
}


/*****************************************************************

 Usage: void win_scroll_up(int handle, int lines);

 handle= Window handle.

 Scrolls the window indicated by "handle" up by the number
 indicated by "lines".

*****************************************************************/

void win_scroll_up(int handle,int lines)
{
extern struct screen_structure scr;
extern struct window_structure w[];

  if(w[handle].buffer==NULL){  /* if the window is not virtual */
   scroll_up(lines);           /* call scroll_up(); */
   return;
   }

  win_point(handle);    /* redirect output to handle */
   scroll_up(lines);    /* now scroll_up */
  win_point(handle);    /* toggle output back to previous value */
}


/*****************************************************************

 Usage: void win_scroll_down(int handle, int lines);

 handle= Window handle.

 Scrolls the window indicated by "handle" down by the number
 indicated by "lines".

*****************************************************************/

void win_scroll_down(int handle,int lines)
{
extern struct screen_structure scr;
extern struct window_structure w[];

  if(w[handle].buffer==NULL){  /* if the window is not virtual */
   scroll_down(lines);         /* call scroll_down(); */
   return;
   }

  win_point(handle);   /* redirect output to handle */
   scroll_down(lines); /* now scroll_down */
  win_point(handle);   /* toggle output back to previous value */
}


/*****************************************************************

 Usage: void win_gotoxy(int handle, int x,int y);

 handle= Window handle.
 x,y   = column and row.

 Moves the cursor to the position x,y relative to window (handle).
 The cursor is visible only in the current active window.
 If the window (handle) is not active, the change is not seen
 until the window is made active (moved to the top).

*****************************************************************/

void win_gotoxy(int handle,int x,int y)
{
extern struct screen_structure scr;
extern struct window_structure w[];

  if(w[handle].buffer==NULL){  /* if the window is not virtual */
   gotoxy(x,y);                /* call scroll_up(); */
   return;
   }

  win_point(handle);    /* redirect output to handle */
    gotoxy(x,y);        /* move the cursor */
  win_point(handle);    /* toggle output back to previous value */

}


/*****************************************************************

 Usage: void win_print(int handle, int x, int y, char *string);

 handle= Window handle.
 x,y= Position (column and row).
 string= String to write.

 Writes string in window indicated by handle.

*****************************************************************/

void win_print( int handle, int x,int y, char *string)
{
extern struct screen_structure scr;
extern struct window_structure w[];

  if(w[handle].buffer==NULL){   /* if not virtual */
   print(x,y,string);           /* call print routine */
   return;
   }
   win_point(handle);   /* direct output to window */
    print(x,y,string);  /* call print routine */
   win_point(handle);   /* toggle pointer back to original values */
}


/*****************************************************************

 Usage: static win_point(int handle);

 handle= Window handle.

 This function is used internally by many of the win_ functions.

 It saves important values stored in the screen(scr) and window
 (w) data structures and replaces them with new values which point
 to the window indicated.  It in essence "tricks" other routines
 into acting on the new window.  A second call to win_point()
 acts as a toggle and restores the original values.

*****************************************************************/

static void win_point(int handle)
{
extern struct screen_structure scr;
extern struct window_structure w[];

static char far *hold_screen;
static int t,b,l,r;

/*
  We are going to trick the external definitions of the screen
  (scr.buffer, scr.rows, scr.columns, scr.top, scr.bottom, scr.left,
  and scr.right) so they "point" to the virtual window.  If the
  window is framed, the margins will show an indention of 1, if
  unframed then no indention. */

/* the following variables are used to store the "true"
   external variables */

static int wt,wb,wl,wr;
static int rows,columns;
static int in_use=FALSE;
static int update;
static int active;
static int snow;

if (w[handle].buffer==NULL) return;  /* return if window is active */

  if(in_use==FALSE){   /* enter here if the function is not already
                          pointing to another window? */

    in_use=TRUE;       /* mark it used */
    update=scr.update; /* store old update info */

    scr.update=FALSE;  /* don't update cursor, window is virtual */

    hold_screen=scr.buffer;  /* save screen buffer */
    snow =scr.snow;          /* save snow info */
    scr.snow= FALSE;  /* window is virtual, we are not writting to
                         screen so we don't need to worry about
                         snow */

    active=scr.active;  /* save old active */
    scr.active=handle;  /* set handle as new active window */


    t=scr.top,b=scr.bottom,  /* save more values */
    l=scr.left,r=scr.right;

    /* store the old row, columns setting */
    rows=scr.rows;columns=scr.columns; 

    /* the following are the margin values of the window.
       save them, and use for calculations of new margins */

    wt=w[handle].top;
    wb=w[handle].bottom;
    wl=w[handle].left;
    wr=w[handle].right;

    /* now make the pointer to the screen buffer
       point to our window buffer */
    scr.buffer=(char far *) w[handle].buffer;

/* now find out if the window has a frame and
   set the window margins accordingly */

    if(w[handle].frame==FALSE){  /* no frame */
     scr.rows=wb-wt+1;           /* set screen rows and columns */
     scr.columns=wr-wl+1;        /* to values of window */

     scr.top=1;             /* set margin values */
     scr.bottom=scr.rows;
     scr.left=1;
     scr.right=scr.columns;
    } else                  /* the window has a frame */
    {
      scr.rows=wb-wt+3;  /* include frame in rows and column count */
      scr.columns=wr-wl+3;

      scr.top=2;            /* set margins so frame not included */
      scr.bottom=scr.rows-1;
      scr.left=2;
      scr.right=scr.columns-1;
    }

  } else             /* active =TRUE */
  {                  /* enter here to restore original values */
   in_use=FALSE;
   scr.update=update;   /* restore update flag */

   /* restore values */

   scr.top=t; scr.bottom=b;
   scr.left=l;scr.right=r;
   scr.rows=rows; scr.columns=columns;
   scr.buffer=hold_screen;
   scr.active=active;
   scr.snow=snow;
  }
}
