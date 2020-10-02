/*  034  14-Feb-87  ovwin.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <setjmp.h>
#include <dos.h>
#include "ov.h"

#ifndef NULL
#define NULL (0)
#endif

#define V_bar (0xba)
#define H_bar (0xcd)

int diridx = 0;                        /* # dir names when showall */
char **dirlst = NULL;                  /* ptr to dir name ptrs when showall */

static int win_update;                         /* NZ if refresh_screen update */
static unsigned char inwin = 0;                /* # active info windows */
static unsigned char numwin = 1;               /* # active windows */
static WINDOW *curwin = NULL, *winlis = NULL, *winp;

static char *tdir;                             /* target dir */
static char *tname;                            /* target name */
static FILE_ENT holdf;                         /* save file_ent */

static char *nomem = "Insufficient memory for another window, no window created!";
static char *noaccess = "Unable to access specified directory: ";

extern WINDOW cw;
extern int winupdate;
extern FILE_ENT files[];
extern unsigned dataseg;
extern jmp_buf back_to_main;
extern unsigned char anyshowall;

char far *malloc_f();
int ALTCALL scanwindows(), ALTCALL win_switch();
int addfile_ent(), delfile_ent(), refresh_window();
char *strrchr(), *strchr(), *parsepath(), *findir();
int ALTCALL wincpy(WINDOW *, WINDOW *), ALTCALL disp_empty_msg(int);
int ALTCALL savefiles(char far *, int), ALTCALL restorefiles(char far *, int);

/******************************************************************************
 **                         W I N _ O P E N                                  **
 *****************************************************************************/

win_open() {           /* open another file name display window */

   register WINDOW *wp;

   /* make sure the current window is large enough to split */

   if (cw.wrows < 6)
      show_error(0,10,1,"Current window is too small to open another");

   /* allocate memory for the new window structure */

   if ((wp = (WINDOW *) malloc(sizeof(WINDOW))) == NULL)
      show_error(0,11,1,nomem);

   wincpy(wp,&cw);             /* initialize new window via current window */

   /* allocate memory to store files[] data */

   if ((wp->save_files = malloc_f(MAX_FILES * sizeof(FILE_ENT))) == NULL) {
      free((char *)wp);
      show_error(0,12,1,nomem);
   }

   /*  if there is only one window open so far, create the window structure
       and files[] save area to store the current window stuff */

   if (numwin == 1) {
      curwin = (WINDOW *) malloc(sizeof(WINDOW));
      cw.save_files = malloc_f(MAX_FILES * sizeof(FILE_ENT));
      if (curwin == NULL || cw.save_files == NULL) {
         free((char *)wp);
         if (curwin) {
            free((char *)curwin);
            curwin = NULL;
         }
         show_error(0,13,1,nomem);
      }
      cw.prev = cw.next = winlis = curwin;
   }

   savefiles(cw.save_files,cw.nfiles); /* save current window's files[] data */

   /* link the new window just after the current window */

   wincpy(curwin,&cw);
   wp->next = curwin->next;
   wp->prev = curwin;
   wp->prev->next = wp;
   wp->next->prev = wp;
   wincpy(&cw,curwin);

   /* adjust the current window and new window display sizes */

   wp->wrows = cw.wrows / 2;           /* new is at most 1/2 of current */
   cw.wrows = cw.wrows - wp->wrows;    /* current keeps whats left */
   wp->fwrow = cw.fwrow + cw.wrows;    /* new start at end of current */
   cw.ndrows -= wp->wrows;             /* all new rows came from name disp */
   wp->fnrow = wp->fwrow + 1;          /* one overhead row for dir name */
   wp->ndrows = wp->wrows - 1;

   /* adjust the top window if it was the only one and it didn't already
      have a header line */

   if (numwin == 1 && !cw.showall) {
      cw.fnrow++;                         /* need a line to display window */
      cw.ndrows--;                        /*   directory line */
      insert_line(cw.fwrow+1,cw.wrows-1); /* scroll file info down a line */
      disp_dirname();                     /* disp the dir name */
   }

   /* adjust old window if current item is no longer visible and turn off
      the file pointer in the old window */

   if (!on_screen(cw.curidx)) {        /* adjust/redisplay window */

      adjust_window();                 /*   if current item no longer */
      update_window(0);                /*   displayed */

   } else              /* not redisplaying window, turn off highlighted ptr */

      if (cw.nfiles)                      /* turn off file ptr in old window */
         fp_off(cw.curidx);
      else                                /* or de-highlight empty dir msg */
         disp_empty_msg(0);

   /* make the new window the current one */

   wincpy(curwin,&cw);                 /* save the old current one */
   wincpy(&cw,wp);                     /* new one becomes current */
   curwin = wp;                        /* remember who it is */

   numwin++;                           /* one more window than there was */

   if (cw.info_display)                /* if info display is active, there */
      infocnt(1);                      /*   is now 1 more info window active */

   /* only one window can be in showall mode */

   if (cw.showall) {                   /* did the old window have showall? */
      cw.showall = 0;                  /* yes, turn off on this one */
      getfiles();                      /* reread current dir only */
      update_header();                 /* display current dir header info */
   }

   /* display the file data in the new window */

   adjust_window();
   update_window(1);

}


/******************************************************************************
                            W I N _ C L O S E
 *****************************************************************************/

win_close() {          /* close the current window */

   int fwrowadj;
   register WINDOW *wp;

   if (numwin == 1)            /* nothing to do if only one window */
      return;

   /* determine the window adjustment fudge factor, 1 extra line is available
      if only one window will remain and that window is not showall */

   fwrowadj = (numwin == 2 && (!anyshowall || cw.showall)) ? 0 : 1;

   /* adjust the next or prior window to take up the closing windows space
      on the screen.  Always pick the next window unless current is the
      last one displyed */

   if ((wp = cw.next) != winlis) {             /* next = winlis if last */

      wp->fwrow = cw.fwrow;                    /* adj nxt to take over */

   } else              /* closing last window on screen */

      wp = cw.prev;                            /* prev is current to be */

   wp->wrows += cw.wrows;              /* it has this many more rows */
   wp->fnrow = wp->fwrow + fwrowadj;   /*   name display rows */
   wp->ndrows = wp->wrows - fwrowadj;

   cw.next->prev = cw.prev;            /* delink the current (closing) */
   cw.prev->next = cw.next;            /* windows block from window list */

   if (curwin == winlis)               /* keep track of new top window if */
      winlis = wp;                     /* old top is being closed */

   if (cw.showall)                     /* turn off showall mode if this */
      showoff();                       /*   window had it on */

   if (cw.info_display)                /* one less info window if this wind */
      infocnt(-1);                     /*   had info display active */

   free_f(cw.save_files);              /* free the cur win's  files[] area */
   free((char *)curwin);               /* free the cur/old win's block */

   /* make the next (or prior) window current, restore its files */

   wincpy(&cw,curwin = wp);
   restorefiles(cw.save_files,cw.nfiles);

   numwin--;                           /* one less window now */

   /* when there is only one window, don't keep the block and files[]
      save area overhead */

   if (numwin == 1) {
      free((char *)curwin);
      free_f(cw.save_files);
      curwin = winlis = NULL;
      cw.save_files = (char far *) NULL;
   }

   update_header();            /* make volume, dir, file, tag info correct */

   adjust_window();            /* adjust the display */
   update_window(1);

   /* switch to the window's directory */

   if (change_dir(cw.dirbuf) != 0)
      show_error(1,0,1,noaccess);
}


/******************************************************************************
                            W I N _ E X P A N D
 *****************************************************************************/

win_expand() {         /* expand current window to full screen */

   int i;
   register WINDOW *wp, *np;

   if (numwin == 1)            /* nothing to do if only one window */
      return;

   /* there will only be one window left, free up all memory blocks */

   for (i == numwin, wp = winlis; i; i--) {
      np = wp->next;
      free_f(wp->save_files);
      free((char *)wp);
      wp = np;
   }

   numwin = 1;                 /* set counters/pointer to 1 window status */
   curwin = winlis = NULL;

   if (anyshowall && !cw.showall)      /* turn off showall mode if it was */
      showoff();                       /*   active in another window */

   infocnt(cw.info_display ? -(inwin-1) : -inwin); /* at most 1 info win left */

   /* adjust to full screen */

   cw.fwrow = cw.fnrow = FIRST_NROW + cw.info_display;
   cw.wrows = cw.ndrows = NAME_ROWS - cw.info_display;

   if (cw.showall) {           /* showall always has a header */
      cw.fnrow++;
      cw.ndrows--;
   }

   adjust_window();            /* redisplay the window with new size */
   update_window(1);

}


/******************************************************************************
 **                         W I N _ N E X T                                  **
 *****************************************************************************/

win_next() {           /* switch to the next window */

   if (numwin == 1)            /* nothing to do if only one window */
      return;

   win_switch(curwin->next);   /* switch to next window */
}


/******************************************************************************
                            W I N _ P R E V
 *****************************************************************************/

win_prev() {           /* switch to the previous window */

   if (numwin == 1)            /* nothing to do if only one window */
      return;

   win_switch(curwin->prev);   /* switch to previous window */
}


/******************************************************************************
                            W I N _ S W I T C H
 *****************************************************************************/

static int ALTCALL
win_switch(newp)       /* switch to a new window */
register WINDOW *newp;
{
   savefiles(cw.save_files,cw.nfiles); /* save current window's files[] data */

   if (cw.nfiles)                      /* deselect current item in widow */
      fp_off(cw.curidx);
   else                                /* or de-highlight empty dir msg */
      disp_empty_msg(0);

   wincpy(curwin,&cw);                 /* switch to new window */
   wincpy(&cw,curwin = newp);

   restorefiles(cw.save_files,cw.nfiles); /* get new windows files[] entries */

   if (cw.nfiles)                      /* select current item in new window */
      fp_on(cw.curidx);
   else                                /* or highlight empty dir msg */
      disp_empty_msg(1);

   if (change_dir(cw.dirbuf) != 0)     /* switch to the window's directory */
      show_error(1,0,1,noaccess);

   update_header();               /* make volume, dir, file, tag info correct */
}


/*****************************************************************************
                          A D D 2 W I N D O W S
 *****************************************************************************/

add2windows(todir,tofn,fp)     /* add file to any windows showing dest dir */
FILE_ENT *fp;
char *todir, *tofn;
{
   tdir = Strdup(todir);       /* save dir and fn in var's for addfile_ent */
   tname = Strdup(tofn);

   holdf = *fp;                /* save file_ent 'cause files[] is changed */
   holdf.flags &= ~TAGGED;     /* we don't want the tagged flag */

   scanwindows(addfile_ent);   /* add the file_ent to all matching windows */

   free(tdir);                 /* local strings alloc'd because callers tend */
   free(tname);                /* to use strings in files[] which is swapped */
}


/*****************************************************************************
                          D E L F R O M W I N S
 *****************************************************************************/

delfromwins(fromdir,fromfn)  /* delete file from all windows showing dest dir */
char *fromdir, *fromfn;
{

   tdir = Strdup(fromdir);     /* save target dir and name in var's */
   tname = Strdup(fromfn);     /*   for delfile_ent */

   scanwindows(delfile_ent);   /* delete the file_ent from all windows */

   free(tdir);                 /* local strings alloc'd because callers tend */
   free(tname);                /* to use strings in files[] which is swapped */
}


/****************************************************************************
                          A D D F I L E _ E N T
 ****************************************************************************/

static int
addfile_ent() {        /* add a FILE_ENT to target windows */

   register int i;
   register FILE_ENT *tfp;

   /* if the current window displays the target dir or the target drive
      in showall mode, add the file ent to it */

   if (stricmp(cw.dirbuf,tdir) == 0 || (cw.showall && *cw.dirbuf == *tdir)) {

      /* don't add the file if it doesn't match the selection criteria */

      if ((cw.selatrs & holdf.flags) != holdf.flags ||
          (*cw.mask && (cw.maskcmp != match_name(holdf.name,cw.mask))))
         return;

      if (numwin > 1)                       /* make sure files[] is ok */
         restorefiles(cw.save_files,cw.nfiles);

      /* see if there is an existing file by that name in the target dir */

      for (i = cw.nfiles, tfp = files; i; i--, tfp++)
          if (stricmp(tname,tfp->name) == 0 && stricmp(tdir,tfp->dirp) == 0)
             break;

      if (i) {                      /* back out info for an existing */
         delent(tfp);               /* file by the same name */
         packfiles();               /* pack files[] now, not later */
      }

      /* modify holding file entry to its final form */

      strncpy(holdf.name,tname,sizeof(holdf.name));        /* file name */
      holdf.dirp = cw.showall ? findir(tdir) : cw.dirbuf;  /* file's dir */
      holdf.index = cw.nfiles+1;                           /* a guess */

      /* add file to files[] if there is room */

      if (cw.nfiles < MAX_FILES) {

         /* since the files may be sorted by something other than name (say
            date for example) we need to scan files[] again to find where to
            insert the new entry - I used to just sort files[] again but qsort
            is way to slow when files[] is already almost sorted */

         for (i = cw.nfiles, tfp = files; i; i--, tfp++)
             if ((*cw.sortfunc)(&holdf,tfp) < 0)
                break;

         /* i > 0 if entry needs to be inserted, i = 0 if at end */

         if (i)
            memcpy((char *)(tfp+1),(char *)tfp,i*sizeof(FILE_ENT));

         *tfp = holdf;                 /* finally, add entry */
         cw.nfiles++;                  /* there is one more file now */
         cw.num_files++;
         cw.files_size += tfp->size;   /* taking this much more space */
         cw.updated = W_DISP;          /* win needs redisplay (not packing) */
         winupdate++;                  /* some window needs redisplay */

         if (numwin > 1)
            savefiles(cw.save_files,cw.nfiles);
      }
   }
}


/****************************************************************************
                          D E L F I L E _ E N T
 ****************************************************************************/

static int
delfile_ent() {        /* delete a FILE_ENT from target windows */

   register int i;
   register FILE_ENT *tfp;

   /* if the current window displays the target dir or the target drive
      in showall mode, delete the file ent */

   if (stricmp(cw.dirbuf,tdir) == 0 || (cw.showall && *cw.dirbuf == *tdir)) {

      if (numwin > 1)                       /* make sure files[] is ok */
         restorefiles(cw.save_files,cw.nfiles);

      /* see if there is an existing file by that name in the target dir */

      for (i = cw.nfiles, tfp = files; i; i--, tfp++)
          if (stricmp(tname,tfp->name) == 0 && stricmp(tdir,tfp->dirp) == 0)
             break;

      if (i) {                      /* back out info for an existing */
         delent(tfp);               /* file by the same name */
         winupdate++;               /* window needs redisplay */
         cw.updated |= W_PACK;      /* files[] needs to be packed */
         if (numwin > 1)
            savefiles(cw.save_files,cw.nfiles);
      }
   }
}


/*****************************************************************************
                                D E L E N T
 *****************************************************************************/

delent(fp)             /* remove an entry from files[] */
register FILE_ENT *fp;
{
   /* Note: this routine changes cw.num_files, but it doesn't change
      cw.nfiles - this is because the routine packfiles which should
      be called shortly needs the old value of cw.nfiles */

   cw.num_files--;                     /* one fewer file */
   cw.files_size -= fp->size;          /* that much space not used */
   if (fp->flags & TAGGED) {           /* was file tagged? */
      cw.num_tagged--;                 /* one less tagged file */
      cw.tag_size -= fp->size;         /* this space not tagged now */
   }
   *fp->name = '\0';                   /* mark this entry as deleted */
}


/****************************************************************************
                          S C A N W I N D O W S
 ****************************************************************************/

static int ALTCALL
scanwindows(func)      /* scan windows, call func for each one */
int (*func)();
{
   int i;
   register WINDOW *wp;

   /* if there is a list of windows, start with the 1st; switch if not there */

   if (numwin > 1) {
      wincpy(curwin,&cw);                      /* save current window if > 1 */
      savefiles(cw.save_files,cw.nfiles);
      if (curwin != winlis) {                  /* temp switch to top window */
         wincpy(&cw,winlis);
      }
   }

   /* Note: only the files for the original window are saved and restored,
      callers func should restore/savefiles if it needs to */

   for (i = numwin, wp = winlis; i; i--) {     /* check each file window */

      winp = wp;               /* let func know which window it is */

      (*func)();               /* invoke callers func with this window */

      if (numwin > 1) {        /* advance to next window if there is one */
         wincpy(wp,&cw);
         wincpy(&cw,wp = cw.next);
      }
   }

   if (numwin > 1) {           /* restore the current window if > 1 */
      wincpy(&cw,curwin);
      restorefiles(cw.save_files,cw.nfiles);
   }
}


/******************************************************************************
 **                             R E N E W                                    **
 *****************************************************************************/

renew() {              /* renew the file display */

   getcwd(cw.dirbuf,MAX_PATHLEN);        /* reinit drive/dir info incase */
   initdrive(*cw.dirbuf);                /*   it changed somehow */

   getfiles();                         /* reread the directory */
   setup_file_scr();                   /* display the static screen image */
   update_header();                    /* update header info */
   refresh_screen(0);                  /* redisplay all file data */
}


/******************************************************************************
                        R E N E W _ W I N D O W
 *****************************************************************************/

renew_window() {       /* renew the current window display */

   getfiles();                         /* reload the files[] structure */
   adjust_window();                    /* resize window data */
   update_header();                    /* update the header  */
   update_window(1);                   /* and the window data */
}


/******************************************************************************
                          R E F R E S H _ S C R E E N
 *****************************************************************************/

refresh_screen(wup)    /* (re)initialize the total file data display */
int wup;
{
   win_update = wup;          /* save wup in static for refresh_window */

   if (inwin && wup == 0)     /* display the info header if needed */
      infohead();

   scanwindows(refresh_window);        /* redisplay each window */
}

/*****************************************************************************
                        R E F R E S H _ W I N D O W
 *****************************************************************************/

static int
refresh_window() {     /* refresh one window - called by scanwindows */

   if (numwin > 1)
      restorefiles(cw.save_files,cw.nfiles);

   if (win_update && cw.updated) {     /* is this a window update call? */
      if (cw.updated & W_PACK)
         packfiles();
      if (numwin > 1)
         savefiles(cw.save_files,cw.nfiles);
   }

   /* redisplay this window if not a win_update call or win_update call and
      this window has been modified */

   if (!win_update || cw.updated) {
      adjust_window();                 /* calculate display parameters */
      update_window(winp == curwin);   /* update display window */
      cw.updated = 0;                  /* doesn't need to be updated again */
   }
}


/******************************************************************************
                    U P D A T E _ V O L _ S T A T S
 ******************************************************************************/

update_vol_stats() {   /* get and display volume statistics */

   getvolsiz(*cw.dirbuf,&cw.drivep->vol_size,&cw.drivep->vol_free,
             &cw.drivep->clustersiz);
   disp_vol_stats();
}


/******************************************************************************
                        U P D A T E _ H E A D E R
 ******************************************************************************/

update_header() {      /* update display header */

   gotorc(VOL_ROW,1);                  /* make sure volume, directory, */
   out_str(cw.drivep->volbuf,11,' ');  /* file, tagged info is correct */

   if (!cw.showall) {                  /* display the current dir name */
      gotorc(VOL_ROW,PATH_COL+1);      /*   but if showall is in effect */
      out_str(cw.dirbuf,65,' ');       /*   its done somewhere else */
   }

   disp_vol_stats();
   disp_file_stats();

   gotorc(MASK_ROW,MASK_COL-1);        /* display the selection mask */
   disp_char(cw.maskcmp ? ' ' : '~');
   out_str(cw.mask,MASK_LEN,' ');

   gotorc(MASK_ROW+1,MASK_COL-1);      /* display the selection attributes */
   disp_attrib(cw.selatrs);
   disp_str(cw.selatrs & DIR ? " D" : " .");
}


/******************************************************************************
                        U P D A T E _ W I N D O W
 *****************************************************************************/

int ALTCALL
update_window(fptr)    /* display a window full of file info */
int fptr;
{
   int col;
   register int i, m;

   if (numwin > 1 || cw.showall)       /* display window header if > 1 */
      disp_dirname();                  /*   window or showall mode */

   /* display a window of file names */

   for (i = 0; i + cw.fnrow < cw.fnrow + cw.ndrows; i++) {

      gotorc(i+cw.fnrow,0);

      if (i < cw.nrows) {
         for (col = 0, m = cw.nbase + i; m < cw.nfiles; col++, m += cw.nrows)
            disp_file(&files[m],(m == cw.curidx && fptr));

         if (col < cw.ncols)   /* try to only clear on rows where less */
            clr_eol();         /* than ncols are displayed */
      } else
         clr_eol();
   }

   /* display files dir path if showall mode */

   if (fptr && cw.showall && cw.curidx < cw.nfiles)
      disp_path(cw.curidx);

   if (cw.nfiles == 0)         /* display a msg if no files in dir */
      disp_empty_msg(fptr);
}


/******************************************************************************
                        A D J U S T _ W I N D O W
 *****************************************************************************/

int ALTCALL
adjust_window() {

   /* for MS DOS, assume the max length file name is 13.  8 for the primary
      name, 1 for '.', 3 for the extension, and 1 for '\' if its a directory */

   cw.maxlen = MAX_NAMELEN + 1;

   /* everything depends on # files in directory */

   if (cw.nfiles == 0)                 /* unusual, but dir may be empty */
      cw.nrows = cw.ncols = 1;
   else {                              /* are files, calc logical rows, cols */
      cw.ncols = (cw.info_display) ? 1 : 5;
      cw.nrows = (cw.nfiles+(cw.ncols-1)) / cw.ncols;
   }

   cw.colsiz = SCREEN_COLS / cw.ncols;         /* width of each column */

   /* update current logical idx */

   if (cw.curidx >= cw.nfiles)
      cw.curidx = cw.nfiles ? cw.nfiles - 1 : 0;

   /* make sure nbase is setup such that the current file is displayed */

   if (cw.nfiles <= cw.ndrows * cw.ncols) /* if file entries will fit on */
      cw.nbase = 0;                       /*   1 screen, start at 1st */
   else {
      cw.nbase = idx2lr(cw.curidx) - cw.ndrows / 2;  /* try to center curidx */
      if (cw.nbase > 0 && cw.nbase + cw.ndrows > cw.nrows) /* no blank rows  */
         cw.nbase = cw.nrows - cw.ndrows;                  /* if possible    */
      if (cw.nbase < 0)                              /* might overshoot */
         cw.nbase = 0;
   }

}


/******************************************************************************
                             I N F O C N T
 *****************************************************************************/

infocnt(chg)           /* display/remove info header when required */
int chg;
{
   int cur_isnt_top;

   inwin += chg;                       /* more or less info windows */

   cur_isnt_top = (numwin > 1 && curwin != winlis);

   /* if the count of info windows just went to 0, or it just went from 0 to 1,
      we need to add or remove the info display header.  The top display
      window losses or gains a row when this happens */

   if ((inwin == 0 && chg) || (inwin == 1 && chg > 0)) {

      /* do a temp switch to the top window if not already there */

      if (cur_isnt_top) {
         wincpy(curwin,&cw);
         savefiles(cw.save_files,cw.nfiles);
         wincpy(&cw,winlis);
         restorefiles(cw.save_files,cw.nfiles);
      }

      /* now add or remove a row from the window */

      if (inwin) {                     /* remove a line, display header */
         cw.fwrow++;  cw.fnrow++;
         cw.wrows--;  cw.ndrows--;
         infohead();
      } else {                         /* add a line, remove header */
         cw.fwrow--;  cw.fnrow--;
         cw.wrows++;  cw.ndrows++;
      }

      /* now update the top window display, but don't bother if the current
         window is the top window - it will be updated by caller */

      if (cur_isnt_top) {
         adjust_window();      /* calculate #rows, columns, etc to display */
         update_window(0);
      }

      /* restore windows if we did a temp switch above */

      if (cur_isnt_top) {
         wincpy(winlis,&cw);
         savefiles(cw.save_files,cw.nfiles);
         wincpy(&cw,curwin);
         restorefiles(cw.save_files,cw.nfiles);
      }
   }
}


/*****************************************************************************
                      D I S P _ E M P T Y _ M S G
 *****************************************************************************/

static int ALTCALL
disp_empty_msg(on)     /* display empty dir msg w/wo highlighting */
int on;
{
   if (on)
      setvattrib(DIS_HIGH);

   disp_str_at("No files!",cw.fnrow+1,SCREEN_COLS/2-5);

   if (on)
      setvattrib(DIS_NORM);
}


/*****************************************************************************
                        D I S P _ D I R N A M E
 *****************************************************************************/

disp_dirname() {       /* display the dir name header */

   char allmsg[20];

   if (cw.showall) {                   /* special header if showall mode */

      strcpy(allmsg,"FILES ON DRIVE ");
      strncat(allmsg,cw.dirbuf,2);
      center_text(cw.fwrow,allmsg);

   } else

      center_text(cw.fwrow,cw.dirbuf);    /* disp the dir name */
}


/*****************************************************************************
                          I N F O H E A D
 *****************************************************************************/

infohead() {           /* display the info header */

   setvattrib(DIS_HEAD);
   disp_str_at("      NAME           USED  ALLOCATED     DATE         TIME      R H S A DIR",
      FIRST_NROW,0);
   clr_eol();
   setvattrib(DIS_NORM);
}


/*****************************************************************************
                            S H O W _ A L L
 *****************************************************************************/

show_all() {

   if (anyshowall && !cw.showall)      /* only one window can do showall */
      show_error(0,17,1,"Only one window can show all files!");

   if (cw.showall ^= 1) {      /* toggle showall mode */

      /* showall mode is being turned on, allocate space for dir name ptrs */

      if ((dirlst = (char **) calloc(MAX_DIR,sizeof(char *))) == NULL) {
         cw.showall = 0;
         show_error(0,16,1,"Out of memory!  Show All is not active!");
      }

      if (numwin == 1) {       /* make row for showall header if not already */
         cw.fnrow++;
         cw.ndrows--;
      }

      anyshowall = TRUE;       /* yes, a window has showall turned on */

   } else              /* showall is being turned off */

      showoff();

   renew_window();     /* redo window with/without showall mode */
}

/*****************************************************************************
                              S H O W O F F
 *****************************************************************************/

showoff() {            /* turn off show all mode */

   register int i;
   register char **cp;

   for (i = 0, cp = dirlst; i < diridx; i++, cp++)     /* release dir name */
       free(*cp);                                      /*   memory */

   free((char *)dirlst);       /* now release pointer memory */

   diridx = 0;                 /* reset for next time */
   dirlst = NULL;

   cw.showall = anyshowall = FALSE;    /* no window has showall now */

   if (numwin == 1) {          /* release showall header line if not needed */
      cw.fnrow--;
      cw.ndrows++;
   }
}


/******************************************************************************
                              W I N C P Y
 ******************************************************************************/

static int ALTCALL
wincpy(to,from)        /* copy window structures - I made this a separate */
WINDOW *to, *from;     /* routine cause MSC generates a bunch of code for */
{                      /* each structure asignment - in reality, it just a */
                       /* little more than the code required to call this */
                       /* function, but... */

   *to = *from;        /* not much to look at */
}


/******************************************************************************
 **                  S A V E / R E S T O R E  F I L E S                      **
 ******************************************************************************/

static int ALTCALL
savefiles(fp,nf)       /* copy files[] to save area */
char far *fp;
int nf;
{
   movedata(dataseg,(unsigned int) files,FP_SEG(fp),FP_OFF(fp),
             nf * sizeof(FILE_ENT));
}

static int ALTCALL
restorefiles(fp,nf)    /* copy files[] from save area */
char far *fp;
int nf;
{
   movedata(FP_SEG(fp),FP_OFF(fp),dataseg,(unsigned int) files,
            nf * sizeof(FILE_ENT));
}
