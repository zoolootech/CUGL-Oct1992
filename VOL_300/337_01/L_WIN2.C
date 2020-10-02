/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_WIN2.C   ***************************/

#include "mydef.h"
#include <dos.h>

#if defined QUICKC

#include "malloc.h"
#include "memory.h"

#endif

# if defined TURBOC

#include "alloc.h"    /* Turbo C header file */
#include "mem.h"
#include "string.h"
#include "stdlib.h"

#endif


/*****************************************************************

 Usage: void win_pop_top(int handle);

 int handle = window handle.

 Moves the window indicated by "handle" to the top of the stack.
 Updates the window list and redraws the screen.

*****************************************************************/

void win_pop_top(int handle)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int location,i;

 if (handle==0) return;  /* can't delete initial window */
 if(handle==scr.active)return;  /* already on top */

 /* get location of window(handle) in list */
 location=win_validate(handle);   

 if(location==-1)return;  /* invalid handle */
 win_save(); /* save current window, it is about to be overdrawn */

 /* Popping a window to the top requires that the window list be
    rearranged. We shift all values after the window being popped
    (handle) forward one, then put the popped window at the end  */

 for(i=location;i<scr.ptr+1;i++)
     scr.list[i]=scr.list[i+1];  /* shuffle */
 scr.list[scr.ptr]=handle;  /* move window to top of list */

 win_redraw(handle); /* redraw the window to make it appear on top */

 scr.active=scr.list[scr.ptr];/* reset pointer to reflect new list */

 /* do if window has allocated memory */
 if (w[scr.active].buffer !=NULL){ 
  free(w[scr.active].buffer);     /* free buffer of new top window */
  w[scr.active].buffer=NULL;      /* null the pointer */
 }
  update_margins(); /* a new window is on top,
                       update screen margins */

  display_cursor(); /* display cursor for new window */

}


/*****************************************************************

 Usage: void display_cursor(void);

 Moves the cursor to the location indicated by the variables for
 the active window.  Also resets the cursor starting/ending
 scan lines.

 Mostly used when a window redraw occurs.

*****************************************************************/


void display_cursor(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

gotoxy(w[scr.active].x,w[scr.active].y);  /* position cursor */

/* set cursor shape */
set_cursor(w[scr.active].start,w[scr.active].end); 
}


/*****************************************************************

 Usage: void win_redraw(int handle);

 int handle = handle of window to redraw.

 Redraws the specified window. Does not rearrange the list.

*****************************************************************/

static void win_redraw(int handle)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int t,b,l,r;
char far *source_ptr;
char far *scrn_ptr;

int i;

 if(w[handle].buffer==NULL) return; /* window is not virtual,
                                       cannot redraw */
 /* is handle requested in list? */
 if(win_validate(handle)==-1 ) return; 

  /* if the window is framed, the margins must be adjusted */

  if(w[handle].frame==TRUE){
    t=w[handle].top-1;b=w[handle].bottom+1;
    l=w[handle].left-1;r=w[handle].right+1;
  } else{
      t=w[handle].top;b=w[handle].bottom;
      l=w[handle].left;r=w[handle].right;
     }

      /* initialize pointers */
      scrn_ptr=scr.buffer;
      scrn_ptr=scrn_ptr+((t-1)*scr.columns*2)+(2*(l-1));

      source_ptr=w[handle].buffer;

 for(i=1 ;i<=b-t+1;i++){  /*do for each row in window*/
   move_scr_mem((char far *)source_ptr,
     (char far *)scrn_ptr,2*(r-l+1));
   /* update screen and destination pointer */
   scrn_ptr=scrn_ptr +(2*scr.columns);
   source_ptr=source_ptr+ (2*(r-l+1));
 }
}


/*****************************************************************

 Usage: void win_redraw_all(void);

 Redraws all the windows.
 The redraw is done on a virtual screen which is then copied to
 the true screen buffer.

*****************************************************************/

void win_redraw_all(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

char far *hold_buffer;
char *new_buffer;
int i,loc,buffsize;
int oldsnow=scr.snow;
hold_buffer=scr.buffer;

buffsize=scr.rows*scr.columns*2;  /* calc size of buffer */

new_buffer= NULL;

 /* save top window as all will be redrawn */
  loc=scr.list[scr.ptr];   /* loc= top window */

 if(w[loc].buffer==NULL)  /* save only if it is not virtual */
   win_save();

/* allocate memory */
new_buffer= malloc (buffsize*sizeof(unsigned char)); 

/* if we got a new buffer set external screen pointer to it */
/* write routine will now write to new buffer */

if (new_buffer!=NULL) scr.buffer=(char far *)new_buffer;

/* turn of snow flag, we are not writing to screen buffer */
scr.snow=FALSE;   

/* redraw all windows */
for(i=0;i<scr.ptr+1;i++)
    win_redraw(scr.list[i]);

scr.snow=oldsnow; /* turn snow flag back on */

/* free up buffer of top window */

  if(w[loc].buffer!=NULL){  /* be sure it is allocated */
    free(w[loc].buffer);    /* free it */
    w[loc].buffer=NULL;     /* mark it as freed */
  }

/* if we did get a new buffer, move it to screen */

 if(new_buffer!=NULL){
  scr.buffer =hold_buffer;   /* restore original screen buffer */
  move_scr_mem((char far *)new_buffer,scr.buffer,buffsize);
  free(new_buffer);  /*free memory */
  new_buffer=NULL;   /* mark it */
  display_cursor();  /* redisplay cursor */
 }
}
