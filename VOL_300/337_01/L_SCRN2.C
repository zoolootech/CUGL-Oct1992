/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_SCRN2.C    ***************************/

#include "mydef.h"
#include <dos.h>


/*****************************************************************

 Usage: void wherexy(int *x,int *y);

 Call with address of x and y  "wherexy(&x,&y);"

 Returns the current address of the cursor. x=column y=row
 Value is relative to active window.  1,1 is upper left corner.

*****************************************************************/

void wherexy(int *x,int *y)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

   union REGS regs;

   regs.h.ah = 3;  /* read cursor  function*/

   regs.h.bh = scr.page;      /* page to read */

   int86(0x10, &regs,&regs);  /* do interrupt */

   /* get x from dl and adjust for window */
   *x=( regs.h.dl +2-scr.left); 

   /* get y from dh and adjust for window */
   *y= (regs.h.dh +2-scr.top);    
}


/*****************************************************************

 Usage: void readxy(char *ch,char *attr);

 Call with address of ch and attr  "readxy(&ch,&attr);"

 Returns with ch= the character under the cursor.
 attr= text attribute under the cursor.

*****************************************************************/

void readxy(char *ch,char *attr)
{
extern struct screen_structure  scr;

   union REGS regs;

   regs.h.ah = 8;         /* read char function */
   regs.h.bh = scr.page;  /* page number to read */

   int86(0x10, &regs,&regs); /* do interrupt */

   *ch = regs.h.al;         /* get character from al */
   *attr = regs.h.ah;       /* get attribute from ah */
}


/*****************************************************************

 Usage: void what_cursor(int *start,int *end);

 Call with address of start and end  "what_cursor(&start,&end);"

 Returns the starting and ending scan lines for the cursor.

*****************************************************************/

void what_cursor(int *start, int *end)
{
extern struct screen_structure scr;

union REGS regs;

 regs.h.ah=0x3;       /* read cursor */
 regs.h.bh=scr.page;  /* must tell it what page for color systems */

 int86(0x10,&regs,&regs);  /* do interrupt */

 /* starting line in ch, ending line in cl */
 *start=regs.h.ch;  *end=regs.h.cl;
}
