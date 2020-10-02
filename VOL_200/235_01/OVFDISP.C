/*  025  17-Jan-87  ovfdisp.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include "ov.h"

#define V_bar (0xba)
#define H_bar (0xcd)

#define INFO_COLS (60)                 /* extra file info uses 60 xtra cols */

extern FILE_ENT files[];
extern struct window cw;               /* the current widow structure */
unsigned char help_display;            /* NZ if help is active */

char *mmddyy();
unsigned long alloc_siz(unsigned long, unsigned int);


/******************************************************************************
 **                   S E T U P _ F I L E _ S C R                            **
 *****************************************************************************/

setup_file_scr() {     /* setup the static file display screen image */

   clr_scr();                          /* start off with clear screen */

   setvattrib(DIS_TEXT);

   disp_str_at("\xc9\xcd Volume \xcd\xcd\xcb\xcd Path ",VOL_ROW-1,0);
   disp_rep(H_bar,SCREEN_COLS-21);
   disp_char_at(0xbb,VOL_ROW-1,SCREEN_COLS-1);

   disp_char_at(V_bar,VOL_ROW,0);
   disp_char_at(V_bar,VOL_ROW,PATH_COL-1);
   disp_char_at(V_bar,VOL_ROW,SCREEN_COLS-1);

   disp_char_at(V_bar,VOL_ROW+1,0);
   disp_rep(' ',PATH_COL-2);
   disp_char_at(0xc8,VOL_ROW+1,PATH_COL-1);
   disp_rep(H_bar,SCREEN_COLS-14);
   disp_str_at("\xcb\xcd Files ",VOL_ROW+1,19);
   disp_char_at(0xb9,VOL_ROW+1,SCREEN_COLS-1);
   disp_str_at("\xcb\xcd Selection ",VOL_ROW+1,61);

   disp_str_at("\xba            TOTAL \xba SELECTED:       FILES             BYTES \xba MASK:",
      FILE_STAT_ROW,0);
   disp_char_at(V_bar,FILE_STAT_ROW,SCREEN_COLS-1);
   disp_str_at("\xba            FREE  \xba TAGGED:         FILES             BYTES \xba ATRS:",
      TAG_STAT_ROW,0);
   disp_char_at(V_bar,TAG_STAT_ROW,SCREEN_COLS-1);

   disp_char_at(0xc8,UP_BOUND,0);
   disp_rep(H_bar,SCREEN_COLS-2);
   disp_char_at(0xca,UP_BOUND,19);
   disp_char_at(0xca,UP_BOUND,61);
   disp_char_at(0xbc,UP_BOUND,SCREEN_COLS-1);

   disp_status();              /* display the status line - resets vid attrib */
}


/******************************************************************************
 **                        D I S P _ F I L E                                 **
 *****************************************************************************/

#pragma check_stack-

int ALTCALL
disp_file(fp,is_cur)   /* display one file name with approiate video attrib */
register struct file_ent *fp;
int is_cur;
{
   register int fast;
   int fillsiz, i, is_pm, fnlen, va;

   /* set the video attribute to use */

   va = is_cur ? DIS_HIGH : (fp->flags & TAGGED ? DIS_TAGD : DIS_NORM);
   if (va != DIS_NORM)
      setvattrib(va);

   if (fnlen = strlen(fp->name)) {
      disp_str(fp->flags & TAGGED ? "\x1A " : "  ");   /* tagged flag gets -> */
      disp_str(fp->name);                              /* display the name */
      if (fp->flags & DIR) {                           /* \ after dir names */
         disp_char('\\');
         fnlen++;
      }
   }

   disp_rep(' ',cw.maxlen - fnlen);    /* blank fill the name field */

   /* display extended file info if active */

   if (cw.info_display && fnlen) {

      out_long(fp->size,10,' ');       /* display used/allocated sizes */
      out_long(alloc_siz(fp->size,cw.drivep->clustersiz),11,' ');

      disp_str("   ");                 /* 3 blanks before date */
      disp_str(mmddyy(fp->date));

      disp_str("   ");                 /* 3 blanks before time */

      i = ((fast = fp->time) & 0xF800) >> 11;  /* i = hours */

      if (is_pm = (i > 12))            /* is it AM or PM? */
         i -= 12;

      out_int(i,2,' ');                /* out go the hours */
      disp_char(':');

      out_int((fast & 0x07E0) >> 5,2,'0');     /* now the minutes */
      disp_char(':');

      out_int((fast & 0x1F) << 1,2,'0');       /* seconds */

      disp_str(is_pm ? " pm  " : " am  ");

      disp_attrib(fp->flags);          /* display the R H S A attributes */

      disp_str(fp->flags & DIR ? " DIR" : "    ");   /* do the DIR attrib */

      fillsiz = cw.colsiz - cw.maxlen - INFO_COLS - 2;

   } else              /* no extended file info display */

      fillsiz = cw.colsiz - cw.maxlen - 2;

   disp_rep(' ',fillsiz);              /* blank fill the column */

   if (va != DIS_NORM)                 /* restore video attrib if not normal */
      setvattrib(DIS_NORM);
}
#pragma check_stack+


/******************************************************************************
 **                      D I S P _ A T T R I B                               **
 *****************************************************************************/

#pragma check_stack-

disp_attrib(attribs)   /* display the attributes in R H S A format */
register int attribs;
{
   static char *no_at = " .";

   disp_str(attribs & RDONLY  ? " R" : no_at);
   disp_str(attribs & HIDDEN  ? " H" : no_at);
   disp_str(attribs & SYSTEM  ? " S" : no_at);
   disp_str(attribs & ARCHIVE ? " A" : no_at);
}
#pragma check_stack+

/******************************************************************************
 **                    D I S P _ V O L _ S T A T S                           **
 *****************************************************************************/

int ALTCALL
disp_vol_stats() {

   gotorc(FILE_STAT_ROW,VOL_STAT_COL);
   out_long(cw.drivep->vol_size,10,' ');
   gotorc(TAG_STAT_ROW,VOL_STAT_COL);
   out_long(cw.drivep->vol_free,10,' ');
}


/******************************************************************************
 **                  D I S P _ F I L E _ S T A T S                           **
 *****************************************************************************/

int ALTCALL
disp_file_stats() {

   gotorc(FILE_STAT_ROW,NUM_FILES_COL);
   out_int(cw.num_files,5,' ');
   gotorc(FILE_STAT_ROW,SPACE_USED_COL);
   out_long(cw.files_size,10,' ');
   gotorc(TAG_STAT_ROW,NUM_FILES_COL);
   out_int(cw.num_tagged,5,' ');
   gotorc(TAG_STAT_ROW,SPACE_USED_COL);
   out_long(cw.tag_size,10,' ');
}


/*****************************************************************************
                           D I S P _ P A T H
 *****************************************************************************/

disp_path(idx)         /* display a files dir path - only used in showall */
int idx;
{
   gotorc(VOL_ROW,PATH_COL+1);         /* display the files pathname */
   out_str(files[idx].dirp,65,' ');
}


/******************************************************************************
                           D I S P _ S T A T U S
 *****************************************************************************/

disp_status() {        /* display the status line */

   setvattrib(DIS_HEAD);
   disp_rep_at(' ',SCREEN_COLS,STATUS_ROW,0);

   if (!help_display)
      disp_str_at(" Press F1 for Help ",STATUS_ROW,SCREEN_COLS/2-10);

   setvattrib(DIS_NORM);
}


/******************************************************************************
                        D I S P / C L R _ M S G
 *****************************************************************************/

#define MSGLEN 25

disp_msg(cnt,m1)       /* display msg strings on the status line */
int cnt;
char *m1;
{
   int len = 0;
   register char **mp;

   gotorc(STATUS_ROW,0);
   setvattrib(DIS_HEAD);

   mp = &m1;                           /* display cnt msg strings */
   for ( ; cnt; cnt--, mp++) {
      disp_str(*mp);
      len += strlen(*mp);
   }

   if (len < MSGLEN)                   /* blank fill msg field */
      disp_rep(' ',MSGLEN-len);

   setvattrib(DIS_NORM);
}

clr_msg() {    /* clear msg field on status line */

   gotorc(STATUS_ROW,0);
   setvattrib(DIS_HEAD);
   disp_rep(' ',MSGLEN);
   setvattrib(DIS_NORM);
}


/******************************************************************************
 **                         C E N T E R _ T E X T                            **
 *****************************************************************************/

center_text(row,text)          /* center text in a heading line */
int row;
char *text;
{
   setvattrib(DIS_HEAD);

   disp_rep_at(' ',SCREEN_COLS,row,0);
   disp_str_at(text,row,SCREEN_COLS/2 - strlen(text)/2);

   setvattrib(DIS_NORM);
}
