/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_PRINT.C  ***************************/

#include "mydef.h"
#include <dos.h>

#if defined TURBOC
#include <mem.h>
#endif

#include <string.h>


/*****************************************************************

 Usage: void print(int x, int y, char *string);

 int x,y       = column, row  at which text is printed.
 char * string = string to print.

 Prints the string at the location indicated for the active window.
 The string is printed with the text attribute stored in scr.current.
 If the external variable scr.bold_caps is set true, uppercase
 letters are printed with the attribute intensity set high.
 Text is wrapped to fit within the window, and the window is scrolled
 up when the bottom of the screen is reached.  The cursor position is
 updated.

 Calls dma_write() to do actual work.

*****************************************************************/

void print(int x,int y,char *string)
{
  int x2,y2;
  x2=x;y2=y;                /* make copies of x,y, we don't want
                               original values changed */
  dma_print(&x2,&y2,string);  /* call dma_print to write the text */
  gotoxy(x2,y2);              /* move the cursor */
}


/*****************************************************************

 Usage: void print_here(char *string);

 char * string = string to print.

 Just like print() except that it prints the text at the current
 cursor location.

*****************************************************************/

void print_here(char *string)

{
 int  x,y;
 wherexy(&x,&y);    /* find out where the cursor is */
 print(x,y,string); /* print the string */
}


/*****************************************************************

 Usage: void dma_print(int *x, int *y, char *string);

 int *x,*y       = column, row  at which text is printed.
 char * string = string to print.

 Works like print() except the cursor is not moved.  x and y are
 set to the location that the cursor should occupy. This function
 is useful if you don't actually want the cursor moved.

*****************************************************************/


void dma_print(int *x, int *y,char *string)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

 int i,j,str_index,write_width;
 int abs_x,abs_y;
 int orig_y;
 int width,height;
 unsigned  offset;
 char far *video_buffer;
 char string2[MAX_STRING*2];

 orig_y=*y;  /* save the original y */
 width=scr.right-scr.left+1;    /* figure the length of the window */
 height=scr.bottom-scr.top +1;  /* figure the height of the window */

 if(*y> height){  /* if y is beyond last line of window*/
  *y= height;     /* set y = last line of window */
  scroll_up(1);   /* scroll_up window to make space */
 }
  if(*x> width)*x=width; /* if x greater than last column, */
                         /* set equal to last column */

   abs_x=(*x-1)+scr.left;  /* adjust to screen coordinates */
   abs_y=(*y-1)+scr.top;

/* do final adjustment of coordinates so they fit screen */
   if (abs_x<scr.left) abs_x=scr.left;
   if (abs_x>scr.right) abs_x=scr.right;
   if (abs_y<scr.top) abs_y=scr.top;
   if (abs_y>scr.bottom) abs_y=scr.bottom;

/* the following code creates a new string, padded with attributes */
   i=0;
   while(*string){
      string2[i++]=*string; /*copy char to string2 */

       /* is it an uppercase char ?*/
       if (*string >=65 && *string <=90 && scr.bold_caps==TRUE)
         string2[i++]= set_intense(scr.current); else
         string2[i++]=scr.current;
          string++;
   }

      string2[i]='\0';  /* terminate string */

/* keep index pointer for location within string2, copy sections of
   the string into the screen buffer, staying within the screen
   margins */

       str_index=0;
       j=0;

       for(;;){  /* do for each row of window */
        /* calculate offset */
        offset=(((abs_y-1)*scr.columns)+(abs_x-1))*2; 

        /* figure width of sting to write */
        write_width=(scr.right-abs_x+1)*2;  

        /* trying to write beyond end of string ? */
        if (str_index+write_width>strlen(string2))
         write_width=strlen(string2)-str_index; /* adjust length */
         video_buffer=scr.buffer+offset;      /* set video pointer */

        /* move the string segment */
        move_scr_mem((char far *)&string2[str_index],
                      video_buffer,write_width);

        abs_x=scr.left;
        abs_y++; j++;   /* update index to new location */
        str_index=str_index+write_width; /* increment string index */

        if(str_index>=strlen(string2)) break;  /* end of string */
        if(abs_y>scr.bottom){   /* past bottom of window */
          scroll_up(1);
          abs_y--;
         }
      }

     /* update cursor location */

      *y=*y+j-1;
      if(*y>orig_y)
       *x=(write_width/2)+1;
      else
      *x=*x+(write_width/2);
     /* if last char printed on border */
     if((*x>width) && *y< height){
      (*y)++; *x=1;                   /* move cursor down */
     }
}


/*****************************************************************

 Usage: void move_scr_mem(char far *string,char far *video,
                          int number)

 char far *string = string to print.
 char far *video  = address within screen buffer to which text is
                    written.
 int number       = the number of bytes to write.

 Copies the string to the video buffer.  The pointers are far,
 so the string could be copied any where in conventional memory.
 Can also copy from screen to another location.  Avoids screen
 snow on CGA display adapters when the external variable scr.snow
 is set to TRUE.

*****************************************************************/

void move_scr_mem (char far *string,char far *video,int number)
{
unsigned int sseg,soff,dseg,doff;
int regval;
int origval;
int vert;
char far *ptr;

sseg=FP_SEG(string);soff=FP_OFF(string);
dseg=FP_SEG(video);doff=FP_OFF(video);

 if (scr.snow==TRUE){

   /* save the mode setting */
   ptr=MAKE_FP(0 ,0x465);  /* point to BIOS location for mode */
   regval=*ptr;            /* read the mode */
   origval=regval;         /* save the original value */
   regval=regval&0x37;     /* mask bit 3 off */


   /* wait for the CGA status register (bit 3) to go from 0 to 1 */
   for(;;){
    vert =inp(0x3da);        /* read the CGA status register */
    if ((vert&8)==0) break;  /* is bit three equal 0? if so, break */
   }

   for(;;){
    vert=inp(0x3da);        /* read the status register again */
    if((vert&8)==8) break;  /* exit when bit three equals 1   */
   }
   outp (0x3d8,regval);   /* send new mode to CGA controller */
 }

 movedata(sseg,soff,dseg,doff,number);  /* move the data */

 if (scr.snow==TRUE)
      outp(0x3d8,origval); /* set controller to original value */
}
