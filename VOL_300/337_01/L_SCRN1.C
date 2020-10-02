/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_SCRN1.C   ***************************/

#include "mydef.h"
#include <dos.h>


/*****************************************************************

 Usage: void ceol(int x, int y);

 x= column.
 y= row.

 Clears the active window from the x-y coordinate to the end
 of the line.

*****************************************************************/

void ceol( int x, int y)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int i;
char string[255];
int width;
char old_current =scr.current;
 if(x>(scr.right-scr.left+1))  return; /* out of bounds */
 if(y>(scr.bottom-scr.top+1))  return;

 scr.current=w[scr.active].attribute;  /* set scr.current to window
                                          attribute */
 width=scr.right-scr.left -x +2;       /* figure width to clear */

  for(i=0;i<width;i++) string[i]=' ';  /* build a string of blanks */
  string[i]='\0';                      /* terminate it */
  dma_print (&x,&y,string);            /* print the string but
                                          don't move cursor */
  scr.current= old_current;    /* restore the current attribute */
}


/*****************************************************************

 Usage: void cls();

 Clears active window.

*****************************************************************/

void cls()
{
extern struct  screen_structure scr;
extern struct window_structure w[];
int height;

 height=scr.bottom-scr.top +1;
   scroll_up(height+1); /* scrolling up more lines than possible
                           clears the screen */
   gotoxy(1,1);         /* move cursor to "home" position */
}


/*****************************************************************

 Usage: void gotoxy(int x, int y);

 x= column (x coordinate)
 y= row    (y coordinate)

 Moves cursor to position indicated by x,y.  Movement relative
 to active window.

 Numbering starts at 1,1.  ie gotoxy(1,1) moves cursor to upper
 left corner of active window.

*****************************************************************/

void gotoxy(int x,int y)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

union REGS regs;

  /* store new cursor in window array */
  w[scr.active].x=x; w[scr.active].y=y;

  if(scr.update!= TRUE ) return; /* return if update flag == FALSE */

   x=(x-1)+scr.left; /* adjust relative  x,y position */
   y=(y-1)+scr.top;  /* to true "physical screen" co-ordinate */

   /* if outside window boundaries, correct */
   if (x<scr.left) x=scr.left;
   if (x>scr.right)
         x=scr.right;
   if (y<scr.top)
         y=scr.top;
   if (y>scr.bottom) y=scr.bottom;

    /* perform interrupt */

    y--;x--;              /* adjust to BIOS numbering system */

   regs.h.ah=2;           /* ah=2, the cursor position function */
   regs.h.dh=y;           /* row */
   regs.h.dl=x;           /* column */
   regs.h.bh=scr.page;    /* page number */

   int86(0x10, &regs, &regs);    /* do interrupt */
}


/*****************************************************************

 Usage: void scroll_up (int lines);

 n= number of lines to scroll up.

 Scrolls the active window up n number of lines.

*****************************************************************/

void scroll_up(int lines)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int width,height;
int offset;
char far *from;char far *to;
int i,j;
int abs_x,abs_y;
char string[401];   /* big enough for 200-column display */

 width=scr.right-scr.left+1;
 height=scr.bottom-scr.top +1;

 if(lines<1)return;

 abs_x=scr.left-1;   /* figure absolute start of window */
 abs_y=scr.top-1;

    offset=( abs_x*2+(abs_y*scr.columns*2) );

    /* build a string (with attributes) = width */
    for(i=0;i<=width*2;i+=2){
     string[i]=' ';
     string[i+1]=w[scr.active].attribute;
    }
    string[i-2]='\0';

     j=0;               /* starting line */

 if(lines<height){

 /*
 The following code sets the j pointer to first line of the window
 the second pointer "i" is set equal the number indicated by "lines"
 each line of the window is moved from "i" to "j". This is repeated
 for each line in the window with "i" and "j" incrementing.
 If the call "scroll_up(3); " were made, line 4 would be moved to
 moved to line 1, line 5 to line 2 etc */

     for(i=lines;i<=height-1;i++){
      from=scr.buffer+offset+i*scr.columns*2;
      to=scr.buffer+offset+j*scr.columns*2;
        move_scr_mem(from,to,width*2);
      j++;
     }
 }

/* clear the left over lines, (if a scroll of 3 lines were made,
   the bottom 3 lines would need to be cleared) */

 for(i=j;i<height;i++){
  to=scr.buffer+(offset+i*scr.columns*2);
  move_scr_mem(string,to,width*2);
 }

 if(w[scr.active].y >= lines){
  w[scr.active].y=w[scr.active].y-lines;
 } else
  {
   w[scr.active].x=1;
   w[scr.active].y=1;
  }
  display_cursor();
}


/*****************************************************************

 Usage: void scroll_down (int lines);

 n= number of lines to scroll up.

 Scrolls the active window down n number of lines.

*****************************************************************/

void scroll_down(int lines)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int width,height;
int offset;
char far *from;char far *to;
int sourc_line,dest_line;
int i;
int abs_x,abs_y;
char string[401];   /* big enough for 200-column display */

 width=scr.right-scr.left+1;  /* figure width and height of window */
 height=scr.bottom-scr.top +1;

 if(lines<1)return;

 abs_x=scr.left-1;   /* figure absolute start of window */
 abs_y=scr.top-1;

    offset=( abs_x*2+(abs_y*scr.columns*2) );

    /* build a string (with attributes) = width */
    for(i=0;i<=width*2;i+=2){
     string[i]=' ';
     string[i+1]=w[scr.active].attribute;
    }
    string[i-2]='\0';

   /* The following works like the code described in scroll_up(), */
   /* except that index pointers move the lines down */

   dest_line=height-1;
   if(lines<height){

     for(sourc_line=height-1-lines;sourc_line>=0;sourc_line--){
      from=scr.buffer+offset+sourc_line*scr.columns*2;
      to=scr.buffer+offset+dest_line*scr.columns*2;
        move_scr_mem(from,to,width*2);
      dest_line--;
     }
    }

 for(sourc_line=dest_line;sourc_line>=0;sourc_line--){
  to=scr.buffer+(offset+sourc_line*scr.columns*2);

  move_scr_mem(string,to,width*2);
 }

 if(w[scr.active].y >= lines){
  w[scr.active].y+= lines;         /*  =w[scr.active].y-lines;*/
 } else
 {
  w[scr.active].y=height;
 }
  display_cursor();
}


/*****************************************************************

 Usage: void set_mode(int mode)

 Sets video mode.

*****************************************************************/

void set_mode(int mode)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

   union REGS regs;

   regs.h.ah= 0;         /* set video state function */
   regs.h.al=mode;       /* mode to set */

    int86(0x10, &regs,&regs);  /* do interrupt */
    scr.mode=mode;             /* update external variable */
}


/*****************************************************************

 Usage: void what_mode(int *mode,int *page);

 Call with address of mode and page "what_mode(&mode,&page);"

 Sets mode and page variables to actual mode and page.

 Defines for mode are found in "mydef.h".

*****************************************************************/


void what_mode(int *mode,int *page)
{
   union REGS regs;

   regs.h.ah= 15;         /* return video state function */

    int86(0x10, &regs,&regs);  /* do interrupt */

   *mode= regs.h.al;   /* set mode */
   *page= regs.h.bh;   /* set page */
}
