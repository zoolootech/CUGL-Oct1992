/*  026   6-Jan-87  ovfmove.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include "ov.h"

/* The following define will effect the speed of some screen updates - for
   the PC set it to 1 - make it higher for slower terminals with hardware
   (firmware?) scrolling */

#define SCROLL_LIMIT 1         /* #lines to update b 4 just rewritting screen */

extern WINDOW cw;
extern FILE_ENT files[];


/******************************************************************************
 **                           M O V E _ F I L E                              **
 *****************************************************************************/

move_file(move_cmd)    /* move the file pointer around, scroll if needed */
register int move_cmd;
{

   register int dif;
   int currow, scrolled = FALSE;
   int last_idx, wrap_factor, redisplayed = FALSE;

   if (cw.nfiles == 0)                 /* don't even try if no files in dir */
      return;

   if (move_cmd == PGUP || move_cmd == PGDN) { /* special case PgUp & PgDn */
      pagedisp(move_cmd);                      /* all done by pagedisp() */
      return;
    }

   last_idx = cw.curidx;               /* remember where we were */

   wrap_factor = (cw.nrows * cw.ncols) - 1;  /* used for right/left wrap */

   /* translate left/right movement to up/down if only 1 column displayed */

   if (cw.ncols == 1)
      move_cmd = move_cmd == RIGHT ? DOWN : (move_cmd == LEFT ? UP : move_cmd);

   switch (move_cmd) {

      case LEFT:                       /* left arrow */
         cw.curidx -= cw.nrows;
         if (cw.curidx < 0) {
            cw.curidx += wrap_factor;
            while (cw.curidx >= cw.nfiles)
               cw.curidx -= cw.nrows;
            if (cw.curidx == cw.nfiles - cw.nrows - 1)
               cw.curidx += cw.nrows;
          }
          break;

      case RIGHT:                      /* right arrow */
         cw.curidx += cw.nrows;
         if (cw.curidx >= cw.nfiles) {
            cw.curidx -= wrap_factor;
            while (cw.curidx < 0)
               cw.curidx += cw.nrows;
            if (cw.curidx == cw.nrows)
               cw.curidx = 0;
         }
         break;

      case UP:                         /* up arrow */
         cw.curidx--;
         if (cw.curidx < 0)
            cw.curidx = cw.nfiles-1;
         break;

      case DOWN:                       /* down arrow */
         cw.curidx++;
         if (cw.curidx >= cw.nfiles)
            cw.curidx = 0;
         break;

      case HOME:                       /* home key */
         cw.curidx = 0;
         break;

      case END:                        /* end key */
         cw.curidx = cw.nfiles - 1;
         break;
   }

   /* scroll or redisplay the screen if the current position is not displayed */

   if (!on_screen(cw.curidx)) {        /* screen need to be adjusted? */

      /* if the display needs to be adjusted by less than SCROLL_LIMIT
         rows in the display area, use the scroll routines, otherwise
         just redisplay using new nbase value */

      currow = idx2lr(cw.curidx);      /* needed several times below */

      dif = (currow < cw.nbase) ? currow - cw.nbase :
                                  currow - cw.nbase - cw.ndrows + 1;

      if (abs(dif) <= SCROLL_LIMIT) {    /* scroll? */

         fp_off(last_idx);               /* this way, ptr doesn't move */

         if (dif > 0)
            while (dif--)
               scroll_dn();
         else
            while (dif++)
               scroll_up();
         scrolled = TRUE;                 /* saves a call to fp_on() */

      } else {                            /* redisplay screen */

         adjust_window();                 /* recalc cw.nbase, etc */
         update_window(1);                /* redisplay file names */
         redisplayed = TRUE;              /* avoid extra fp_*() calls */
      }
   }

   /* deselect the last item if the file pointer has moved and the last
      item is still displayed (and the screen wasn't adjusted) */

   if (!redisplayed && !scrolled && cw.curidx != last_idx &&
       on_screen(last_idx))
      fp_off(last_idx);

   /* highlight the current file if the file pointer moved and the screen
      wasn't adjusted (adjustments set the pointer) */

   if (!redisplayed && !scrolled && cw.curidx != last_idx)
      fp_on(cw.curidx);
}


/*****************************************************************************
                               P A G E D I S P
 *****************************************************************************/

static int
pagedisp(dir)          /* page the file display indicated direction */
int dir;
{
   int newidx;
   register int newnbase = cw.nbase;

   if (dir == PGUP) {                  /* page up a screen? */

      newnbase -= cw.ndrows;           /* backup a screen */

   } else {                            /* must want to page down */

      newnbase += cw.ndrows;
      if (newnbase + cw.ndrows > cw.nrows)  /* don't leave part of the  */
         newnbase = cw.nrows - cw.ndrows;   /*   screen blank if avoidable */
   }

   if (newnbase < 0)                   /* adjustments can go too far */
      newnbase = 0;

   if (newnbase == cw.nbase) {         /* is the page the same? */
      if (dir == PGUP)                 /* up direction? */
         newidx = newnbase;            /* yes, goto top to page */
      else
         newidx = cw.nfiles - 1;       /* no, goto last file */
      fp_off(cw.curidx);               /* move file pointer */
      fp_on(cw.curidx = newidx);

   } else {            /* the page base changed, redisplay window */

      cw.nbase = newnbase;             /*  this is the one to use now      */
      if (!on_screen(cw.curidx))       /*  will the old current file show? */
         cw.curidx = newnbase;         /*  if not, we know this one will   */
      update_window(1);                /*  display names at new offset     */
   }
}


/******************************************************************************
                        F P _ O N / O F F
 *****************************************************************************/

int ALTCALL
fp_on(i)               /* turn on the file pointer */
register int i;
{
   gotorc(idx2sr(i),idx2sc(i));
   disp_file(&files[i],1);
   if (cw.showall)
      disp_path(i);
}

int ALTCALL
fp_off(i)              /* turn off the file pointer */
register int i;
{
   gotorc(idx2sr(i),idx2sc(i));
   disp_file(&files[i],0);
}


/******************************************************************************
                            O N _ S C R E E N
 *****************************************************************************/

int ALTCALL
on_screen(idx)         /* determine if file[idx] is currently displayed */
register int idx;
{
   /* this used to be a macro, but it generates a lot of code */

   return(idx2lr(idx) >= idx2lr(cw.nbase) &&
          idx2lr(idx) <= min(cw.nrows-1,idx2lr(cw.nbase)+cw.ndrows-1));
}


/******************************************************************************
 **                    S C R O L L _ U P                                     **
 *****************************************************************************/

static int
scroll_up() {          /* scroll the file display up */

   register int m;

   cw.nbase -= 1;
   insert_line(cw.fnrow+1,cw.ndrows-1);

   gotorc(cw.fnrow,0);                 /* cursor to first display position */

   /* display a row of file names */

   for (m = cw.nbase; m < cw.nfiles; m += cw.nrows)
      disp_file(&files[m],m == cw.curidx);

   if (cw.showall)
      disp_path(cw.curidx);
}


/******************************************************************************
 **                    S C R O L L _ D N                                     **
 *****************************************************************************/

static int
scroll_dn() {          /* scroll the file display down */

   register int m;

   cw.nbase += 1;
   delete_line(cw.fnrow,cw.ndrows-1);

   gotorc(cw.fnrow+cw.ndrows-1,0);     /* cursor to the last display line */

   /* display a row of file names */

   for (m = cw.nbase + cw.ndrows - 1; m < cw.nfiles; m += cw.nrows)
      disp_file(&files[m],m == cw.curidx);

   if (cw.showall)
      disp_path(cw.curidx);
}


