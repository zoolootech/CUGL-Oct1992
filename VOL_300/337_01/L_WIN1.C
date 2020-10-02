/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_WIN1.C   ***************************/

#include "mydef.h"
#include <dos.h>

#if defined QUICKC

#include "malloc.h"
#include "memory.h"

#endif

#if defined TURBOC

#include "alloc.h"
#include "mem.h"
#include "string.h"
#include "stdlib.h"

#endif


/*****************************************************************

 Usage: int win_make(int x, int y, int width,int height \
            char *array char *title, char frame_attr,char win_attr);

  int x,y =  column,row of upper left corner of active window area
             (not upper left frame corner);
  int width= width of window (interior).
  int height= height of window (interior).

  char *array= pointer to string containing frame elements
               (macros defined in mydef.h).
  char *title= title to appear in upper left corner
               of frame.
  char frame_attr=  attribute to use when drawing frame.
  char win_attr= default attribute for window.

  Creates a window, returns a "handle" (integer) which is used
  to access the window in later operations.  The newly created
  window is placed on top of all existing windows, and is the
  default active window.  The newly created window is cleared.
  A -1 is returned if too many window are opened.

  The window created will be resized and repositioned if necessary
  to fit the screen.

*****************************************************************/

int win_make(int x,int y,int width,int height,char *array,
             char *title, char frame_attr, char win_attr)
{
extern struct screen_structure scr;
extern struct window_structure w[];

 int t,b,l,r;  /* hold calculated values of top,bottom,left,right */

   /* check parameters and correct if necessary */

   if(scr.ptr==MAX_WINDOW)return(-1);

   /* adjust x,y,width & height to fit screen */

  if(array[0]=='\0'){                             /* no frame */

    if(x<1)x=1;                                   /* too far left? */
    if(y<1)y=1;                                   /* too far up? */
    if(width>scr.columns) width=scr.columns;      /* too wide? */
    if(height>scr.rows) height=scr.rows;          /* too high? */
    if(x+width>scr.columns) x=scr.columns-width+1;/* too far right?*/
    if(y+height>scr.rows) y=scr.rows-height+1;    /* too far down?*/

  }
  else{                                        /* frame present */
   if(x<2)x=2;                                 /* allow for frame */
   if(y<2)y=2;
   if(width+2>scr.columns) width=scr.columns-2;   /* too wide? */
   if(height+2>scr.rows) height=scr.rows-2;       /* too high? */
   if(x+width+2>scr.columns) x=scr.columns-width; /* too far right */
   if(y+height+1>scr.rows) y=scr.rows-height;     /* too far down */
  }

  if (array[0]=='\0')                       /* no frame */
    w[scr.list[scr.ptr+1]].frame=FALSE;
  else                                      /* frame present */
    w[scr.list[scr.ptr+1]].frame=TRUE;

      t=y;b=y+height-1;l=x;r=x+width-1; /* calc. window coordinates*/

     win_save();    /* save window (include frame) */
     scr.ptr++;     /* increment screen pointer */

     /* make new window the active window */
     scr.active=scr.list[scr.ptr]; 

 /* release window so we can write anywhere
    (the printing routines can't write outside existing window
    so we temporarily expand window so we can draw frame)      */

  scr.top=1;
  scr.bottom=scr.rows;
  scr.right=scr.columns;
  scr.left=1;

  if (array[0]!='\0'){   /* if frame requested */

   draw_frame(x-1,y-1,width,height,array,title,frame_attr);

  }

/* set current attribute to window attribute */
scr.current = win_attr; 

w[scr.active].attribute=win_attr;  /* save the window attribute */

/* set active window values  */

w[scr.active].top=t;w[scr.active].bottom=b; /* save top and bottom */
w[scr.active].left=l;w[scr.active].right=r; /* save left and right */

update_margins();

    /* set default "underscore" cursor for new window */

    if (scr.mode == COLOR_80 || scr.mode == BW_80){
            w[scr.active].start=6; w[scr.active].end=7;
    }

    if (scr.mode == MONOCHROME ){
            w[scr.active].start=11;w[scr.active].end = 12;
    }

    /* save cursor information */
    cursor(NORMAL_CURSOR);  /* change acutal cursor */
     cls();


return(scr.active);  /* return the window "handle" */

}


/*****************************************************************

 Usage: void win_save(void);

 Saves the screen buffer of the top window.
 Used by other routines when the top window is about to be
 over written or the screen is being redrawn.

 This function should not be called directly.

*****************************************************************/

void win_save(void)

{
extern struct screen_structure scr;
extern struct window_structure w[];

 int y;
 char far *dest_ptr;
 char far *scrn_ptr;
 int buff_size;  /* memory size required to hold saved image */
 int t,b,l,r;

/* set t,b,l,r to represent window top,bottom,left and right
   adjust to include frame if necessary */

   if (w[scr.active].frame==TRUE){
     t=scr.top-1;b=scr.bottom+1;l=scr.left-1;r=scr.right+1;
    }
    else{
     t=scr.top;b=scr.bottom;l=scr.left;r=scr.right;
    }
   /* copy external screen pointer to local pointer */
   scrn_ptr=scr.buffer; 

    if (scr.ptr > MAX_WINDOW){
      cls();puts("too many windows ");exit(1);
      return;
    }

    /* calculate memory required to hold screen image */
    buff_size=( (r-l+1)*(b-t+1) )*2;

    if (w[scr.active].buffer==NULL)  /* if no memory is allocated */
    w[scr.active].buffer =
      (char  *)malloc (buff_size*sizeof(unsigned char));

              /* set up destination pointer (dest_ptr) and
                 screen pointer (scrn_ptr */

              dest_ptr=w[scr.active].buffer;

              /* initialize pointers */
              scrn_ptr=scr.buffer;
              scrn_ptr=scrn_ptr+((t-1)*(scr.columns*2))+(2*(l-1));
              dest_ptr=w[scr.active].buffer;

    /* move each row from screen to buffer */
    for(y=1 ;y<=b-t+1;y++){  
      move_scr_mem(scrn_ptr,dest_ptr,2*(r-l+1));
      scrn_ptr=scrn_ptr +(2*scr.columns); /* update screen pointer */
      dest_ptr=dest_ptr+ (2*(r-l+1));/* update destination pointer */
    }

}  /* end function */


/*****************************************************************

 Usage: void win_delete(int handle);

 Deletes the window indicated.
 Rearranges window list to reflect change.

*****************************************************************/

void win_delete(int handle)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int location,i;

if (handle==0) return;  /* can't delete initial window */

/*if it is the top window call win_delete_top */
if (handle==scr.active) {  
      win_delete_top();
      return;
}

location=win_validate(handle);
if(location==-1) return;         /* not a window */

win_save();  /* save top window */

/* Deleting a window requires that the window list be
   rearranged.   We shift all values after the window
   being deleted forward one, then put the deleted
   window at the end      */


for(i=location;i<scr.ptr+1;i++) scr.list[i]=scr.list[i+1];

scr.list[scr.ptr]=handle;  /* move window to top of list */

if (w[handle].buffer!=NULL){
 free(w[handle].buffer);  /*free buffer of deleted window */
 w[handle].buffer=NULL;  /* NULL pointer */
}
scr.ptr--;   /* point down one level (we have one less window) */
win_redraw_all();
}


/*****************************************************************

 Usage: int win_validate(int handle);

 Checks the list of allocated windows to see if the
 Window handle indicated is valid.

 Returns a -1 if not valid, else returns the location
 of the window in the list.

*****************************************************************/

int win_validate(handle)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int i,location=-1;
 for(i=0;i<=scr.ptr;i++){   /* find if handle is allocated window */
  if (scr.list[i]==handle){
   location=i;
   break;
  }
 }
 return location;
}


/*****************************************************************

 Usage: void win_delete_top(void);

 deletes the top window.

*****************************************************************/


void win_delete_top(void)
{
extern struct  screen_structure scr;

if (scr.ptr==0)return;  /* can't delete initial window */


scr.ptr--;              /* set the window pointer down one level */
scr.active=scr.list[scr.ptr];

update_margins();
win_redraw_all();      /* redraw all windows */

/* win_redraw will free buffer of top window when done
   we do not need to do it here */
}


/*****************************************************************

 Usage: void draw_frame(int x,int y,int width,int height,char *array,
                         char *title,char attribute)

 int x,y = Upper left corner of frame.
 int width,height= Width and height of interior of frame.
 char *array= Array of characters used to build frame. (see mydef.h)
 char *title= Title to use in upper left corner of frame
 int attribute= Text attribute to use for frame.

*****************************************************************/

void draw_frame (int x,int y,int width,int height,char *array,\
                 char *title,char attribute)
{
extern struct screen_structure scr;

 int old_attribute;
 int i,j,ctr,u_right,u_left;
 int x2,y2;
 char string[255];

 old_attribute=scr.current;  /* save current attribute */
 scr.current=attribute;
     /* draw first line of frame */
     string[0]= array[0];                      /* left corner */
     for(i=1;i<=width;i++) string[i]=array[4]; /* horizontal part */
     string[i++]=array[1]; string[i]='\0';     /* right corner and
                                                  terminator */
      x2=x;y2=y;
      dma_print(&x2,&y2,&string[0]);          /* print string */

     if (title!=NULL)          /*is there a title ? */
       if (strlen (title)<= width) {  /* will it fit? */
        x2=x+1;y2=y;
        dma_print(&x2,&y2,title );
       }

     y++;   /* move to next line */

       for (i=0; i<height;i++){         /* print vertical border */
        string[0]=array[5];string[1]='\0';  /* left border */
        x2=x;y2=y;
        dma_print(&x2,&y2,string);
        x2=x+width+1;y2=y++;         /* right border */
        dma_print(&x2,&y2,string);
      }
          string[0]=array[2];        /* draw bottom line of frame */
          for(i=1;i<=width;i++) string[i]=array[4];
          string[i++]=array[3];string[i]='\0';
          x2=x;y2=y++;
          dma_print(&x2,&y2,&string[0]);

          scr.current=old_attribute;    /* restore old attribute */
}


/*****************************************************************

 Usage: int win_center(int width,int height,char *array
                     char *title, char frame_attr,char win_attr);


  int width= width of window (interior).
  int height= height of window (interior).

  char *array= pointer to string containing frame elements
               (macros defined in mydef.h).
  char *title= title to appear in upper left corner
               of frame.
  char frame_attr=  attribute to use when drawing frame.
  char win_attr= default attribute for window.

  Creates a window which is centered in the middle of the
  computer screen.  It is called just like win_make(), except
  that no row and column is specified.

*****************************************************************/

int win_center(int width,int height,char *array,char *title,
                char frame_attr, char win_attr)
{
extern struct screen_structure scr;
extern struct window_structure w[];

 int x,y;

 x=(scr.columns/2)-(width/2); /* calculate window's column location*/
 y=(scr.rows/2)-(height/2);    /* calculate window's row location */

 /* call win make to create centered window */

 return(win_make(x,y,width,height,array,title,frame_attr,win_attr));

}
