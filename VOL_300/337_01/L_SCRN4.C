/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_SCRN4.C  ***************************/

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

 Usage: void alt_screen(int action);

 int action = ON or OFF.

 Turns on the alternate virtual screen.  The true video buffer is
 copied to the alternate screen.  All output (screen writing,
 window creation etc.) is routed to the alternate screen until
 the command "alt_screen(OFF);" is issued.  The alternate screen
 is then copied to the video buffer where it may be seen.

*****************************************************************/

void alt_screen(int action)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

static char far *v_screen;
static char far *orig_screen;
static int old_snow;
static int old_update;
int buffsize=scr.rows*scr.columns*sizeof(unsigned char)*2;

 if (action==1){   /* open alternate (virtual) screen */
  if (orig_screen!=NULL)return;   /* screen already set */
  old_update=scr.update;   /* save old update info */
  scr.update=FALSE;   /* don't update cursor until done */

  old_snow=scr.snow;

  orig_screen=scr.buffer;  /* save original screen */
  v_screen=(char far *)malloc(buffsize);  /* alloc new screen */
  if(v_screen==NULL)return;  /* exit if allocation not made */
   move_scr_mem(scr.buffer,v_screen,buffsize);
   scr.snow=FALSE;
   scr.buffer=v_screen;
 }

 if (action==0){  /* close alternate screen */

  if(orig_screen==NULL)return;  /* not open */
   scr.buffer=orig_screen;   /* reset buffer */
   scr.snow=old_snow;
     move_scr_mem(v_screen,scr.buffer,buffsize);

     scr.update=old_update;  /* we can update cursor now */
     set_cursor(w[scr.active].start,w[scr.active].end);
     gotoxy(w[scr.active].x,w[scr.active].y);

   /* release v_screen memory */
   free((void *)v_screen); v_screen=NULL;  
   orig_screen=NULL;
 }
}
