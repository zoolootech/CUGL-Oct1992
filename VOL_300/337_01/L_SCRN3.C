/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_SCRN3.C    ***************************/

#include "mydef.h"
#include <dos.h>


/*****************************************************************

 Usage: int cursor(int size);

 Sets the size of the cursor.

 Uses the following definitions found in "mydef.h":

 BIG_CURSOR    = 2     Full-sized cursor.
 NORMAL_CURSOR = 1     Normal underline cursor.
 NO_CURSOR     = 0     Cursor invisible.

*****************************************************************/

void cursor(int size)
{
extern struct screen_structure scr;
int start,end;

    /* set cursor for color graphic type cards */

    if (scr.mode == COLOR_80 || scr.mode == BW_80){
        if (size == NO_CURSOR){
            start=16; end=0;
        }
        if (size == NORMAL_CURSOR){
            start=6; end=7;
        }
        if (size == BIG_CURSOR){
            start=0,end=7;
        }
    }

    /* set cursor for monochrome cards */

    if (scr.mode == MONOCHROME ){
        if (size == NO_CURSOR){
            start=16;end=0;
        }
        if (size == NORMAL_CURSOR){
            start=11;end = 12;
        }
       if (size == BIG_CURSOR){
            start=0;end =14;
       }
    }

   set_cursor(start,end);
}


/*****************************************************************

 Usage: void set_cursor(int start, int end);

 Sets the size of the cursor based on start and end scan lines.

 Gives more control that cursor(size);

*****************************************************************/

void set_cursor (int start, int end)
{
extern struct screen_structure scr;
extern struct window_structure w[];

 union REGS regs;

 if(scr.update==TRUE){  /* if update ok then set cursor */

  regs.h.ah=0x01;       /* set cursor function */

  regs.h.ch=start;      /* starting line in ch */
  regs.h.cl=end;        /* ending line in cl   */

  int86(0x10,&regs,&regs);   /* do interrupt */
 }

/* save cursor information */
w[scr.active].start=start;
w[scr.active].end=end;

}
