/*  019  14-Feb-87  ovtag.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include "ov.h"

#ifndef NULL
#define NULL (0)
#endif

static char *name;                     /* wildcard (maybe) name to tag */
static unsigned char attrib;           /* attribute to tag by */
static unsigned from_date, to_date;    /* dates to use in tag by date */
static unsigned from_time, to_time;    /* times to use in tag by time */

#define TDROWS 10                      /* box constants used by tag_date */
#define TDCOLS 32
#define TDFROW (FIRST_NROW+3)
#define TDFCOL 24

static struct {        /* defines fields used by tag by date/time */
   char row;
   char col;
   char len;
   char value[9];
} tdflds[] = { { TDFROW+4, TDFCOL+7, 8, "" },
{ TDFROW+4, TDFCOL+16, 8, "00:00:00" }, { TDFROW+4, TDFCOL+27, 1, "A" },
{ TDFROW+6, TDFCOL+7, 8, "" }, { TDFROW+6, TDFCOL+16, 8, "12:00:00" },
{ TDFROW+6, TDFCOL+27, 1, "P" } };

unsigned int cvtdate(), cvttime();

static struct {                /* used to check/convert time/date fields */
   int *cvtresult;
   int (*cvtrtn)();
   char *p1, *p2;
   char *name;
} cvtflds[] = {
   { &from_date, cvtdate, tdflds[0].value, NULL, "from date" },
   { &to_date, cvtdate, tdflds[3].value, NULL, "to date" },
   { &from_time, cvttime, tdflds[1].value, tdflds[2].value, "from time" },
   { &to_time, cvttime, tdflds[4].value, tdflds[5].value, "to time" } };

int by_name(), by_date(), by_attrib();

extern struct window cw;
extern struct file_ent files[];

unsigned int getdate();
char *strupr(), *mmddyy();


/******************************************************************************
 **                       T A G _ C U R R E N T                              **
 *****************************************************************************/

tag_current() {        /* toggle the tag state of the current file */

   tag_toggle(&files[cw.curidx],cw.curidx);  /* toggle the current file */

   disp_file_stats();                  /* display (maybe) updated file stats */
}


/******************************************************************************
 **                      T A G _ M O D I F I E D                             **
 *****************************************************************************/

tag_modified() {       /* tag files with the archive attriute set */

   attrib = ARCHIVE;                   /* tag files with this attribute */

   tag_files(by_attrib);               /* tag'em an' flag'em */
}


/******************************************************************************
 **                         T A G _ N A M E                                  **
 *****************************************************************************/

tag_name() {           /* wildcard tag of files by name */

   name = strupr(prompt("Tag by name","File name pattern: ",
          NULL,0,12));

   if (strlen(name) == 0)
      return;

   tag_files(by_name);                         /* tag all matchting files */
}


/******************************************************************************
 **                         T A G _ D A T E                                  **
 *****************************************************************************/

tag_date() {

   int cvterr;
   register int i;
   char *tmp, *boxsave;
   unsigned char quit = 0;

   /* display the tag by date dialog box */

   boxsave = (char *) Malloc(TDROWS * TDCOLS * 2);

   setvattrib(DIS_BOX);
   popup(TDFROW,TDFCOL,TDROWS,TDCOLS,boxsave);

   disp_str_at(" Tag by Date/Time ",TDFROW,TDFCOL+2);  /* display box title */

   disp_str_at("MM/DD/YY HH:MM:SS Am/Pm",TDFROW+2,TDFCOL+7);   /* box text */
   disp_str_at("From",TDFROW+4,TDFCOL+2);
   disp_str_at("To",TDFROW+6,TDFCOL+2);

   setvattrib(DIS_HIBOX);
   disp_str_at("Press ESC to Quit",TDFROW+8,TDFCOL+7);

   if (strlen(tdflds[0].value) == 0) {         /* default to todays date */
      tmp = mmddyy(getdate());
      strcpy(tdflds[0].value,tmp);
      strcpy(tdflds[3].value,tmp);
   }

   /* display the input fields and default values */

   for (i = 0; i < 6; i++)
      disp_str_at(tdflds[i].value,tdflds[i].row,tdflds[i].col);

   /* read the field values from user */

   for (i = 0; i < 6; i++) {
      gotorc(tdflds[i].row,tdflds[i].col);
      if (strlen(tmp = read_str(tdflds[i].len,tdflds[i].value,0)))
         strcpy(tdflds[i].value,tmp);

      else {                   /* quit if nothing read */
         quit = 1;
         break;
      }
   }

   setvattrib(DIS_NORM);

   /* convert ascii date/times to file date/time formats */

   if (!quit)
      for (i = 0; i < 4; i++) {
         *cvtflds[i].cvtresult = (*cvtflds[i].cvtrtn)(cvtflds[i].p1,
                                                      cvtflds[i].p2,&cvterr);
         if (cvterr) {
            show_error(0,0,2,"Invalid ",cvtflds[i].name);
            quit = 1;
         }
      }

   /* get rid of the display box */

   popdwn(TDFROW,TDFCOL,TDROWS,TDCOLS,boxsave);
   free(boxsave);

   /* finally tag the files */

   if (!quit)
      tag_files(by_date);

}


/******************************************************************************
 **                         T A G _ I N V E R T                              **
 *****************************************************************************/

tag_invert() {         /* invert the tag state of all files */

   register int i;
   register struct file_ent *fp;

   fp = files;
   for (i = 0; i < cw.nfiles; i++, fp++)
      tag_toggle(fp,i);

   disp_file_stats();                          /* disp updated stats */

}


/******************************************************************************
 **                         T A G _ R E S E T                                **
 *****************************************************************************/

tag_reset() {          /* clear all tagged file indicators */

   register int i;
   register struct file_ent *fp;

   fp = files;
   for (i = 0; i < cw.nfiles; i++, fp++)
      if (fp->flags & TAGGED) {
         fp->flags &= ~TAGGED;
         if (on_screen(i)) {
            gotorc(idx2sr(i),idx2sc(i));
            disp_file(fp,i == cw.curidx);
         }
      }

   cw.num_tagged = 0;                          /* no files tagged now */
   cw.tag_size = 0;
   disp_file_stats();                          /* disp updated (0) stats */

}


/******************************************************************************
 **                         T A G _ T O D A Y                                **
 *****************************************************************************/

tag_today() {          /* tag all files created/modified today */

   from_date = to_date = getdate();
   from_time = 0;
   to_time = 0xffff;

   tag_files(by_date);
}


/******************************************************************************
                          T A G _ F I N D
 *****************************************************************************/

tag_find(dir)          /* find (goto) the next or perv tagged file */
int dir;
{
   register int i;

   if (cw.num_tagged == 0)             /* nowhere to go if no files tagged */
      return;

   /* find the next (prev) tagged file */

   i = cw.curidx;
   do {
      i += dir;                        /* move forward or backward */
      if (i >= cw.nfiles)              /* gotta watch for wrap arounds */
         i = 0;
      else
         if (i < 0)
            i = cw.nfiles - 1;
   } while ((files[i].flags & TAGGED) == 0);   /* done if at a tagged file */

   /* figure out how to move the file pointer to the tagged file */

   if (on_screen(i)) {
      fp_off(cw.curidx);               /* if the new file is currently */
      fp_on(cw.curidx = i);            /*   displayed, just move the pointer */
   } else {
      cw.curidx = i;                   /* otherwise ... */
      adjust_window();                 /* update entire window if the */
      update_window(1);                /*   file isn't displayed */
   }
}


/******************************************************************************
 **                       T A G _ T O G G L E                                **
 *****************************************************************************/

static int
tag_toggle(fp,i)       /* toggle the tag state of file pointed to by fp */
register int i;
register struct file_ent *fp;
{

   if (!(fp->flags & DIR)) {
      if ((fp->flags ^= TAGGED) & TAGGED) {    /* toggle tag state */
         cw.num_tagged++;                      /* keep track of # tagged */
         cw.tag_size += fp->size;              /* and tagged size */
      } else {
         cw.num_tagged--;
         cw.tag_size -= fp->size;
      }
      if (on_screen(i)) {                      /* display file ent with */
         gotorc(idx2sr(i),idx2sc(i));          /* new display attribute */
         disp_file(fp,i == cw.curidx);
      }
   }
}


/******************************************************************************
 **                         T A G _ F I L E S                                **
 *****************************************************************************/

static int
tag_files(cmpfunc)     /* tag all files matching criteria */
int (*cmpfunc)();
{

   register int i;
   register struct file_ent *fp;
   int last_tagged = cw.num_tagged;

   /* scan the files[] structure looking for files to tag.  Don't tag empty
      files[] entries, directory entries, already tagged entries, or entries
      that don't meet the qualifications (as determined by cmpfunc()).  When
      an entry is tagged, redisplay it if its currently displayed, update
      num_tagged count, and tagged file size.  Update file status display
      when done. */

   fp = files;
   for (i = 0; i < cw.nfiles; i++, fp++)
      if (!(fp->flags & DIR) && !(fp->flags & TAGGED) && (*cmpfunc)(fp)) {
         fp->flags |= TAGGED;
         if (on_screen(i)) {
            gotorc(idx2sr(i),idx2sc(i));
            disp_file(fp,i == cw.curidx);
         }
         cw.num_tagged++;
         cw.tag_size += fp->size;
      }

   if (cw.num_tagged != last_tagged)           /* actually tag any? */
      disp_file_stats();                       /* disp updated stats */
}


/******************************************************************************
 **                         B Y _ A T T R I B                                **
 *****************************************************************************/

static int
by_attrib(fp)          /* determine if file should be tagged by attribute */
struct file_ent *fp;
{
   return(fp->flags & attrib);
}


/******************************************************************************
 **                         B Y _ N A M E                                    **
 *****************************************************************************/

static int
by_name(fp)            /* determine if file entry should be tagged by name */
struct file_ent *fp;
{
   return(match_name(fp->name,name));
}


/******************************************************************************
 **                         B Y _ D A T E                                    **
 *****************************************************************************/

static int
by_date(fp)            /* determine if file entry should be tagged by date */
register struct file_ent *fp;
{
   if ((fp->date > from_date ||
       (fp->date == from_date && fp->time >= from_time)) &&
       (fp->date < to_date   ||
       (fp->date == to_date && fp->time <= to_time)))
      return(1);
   else
      return(0);
}


/*****************************************************************************
                        C V T D A T E / T I M E
 *****************************************************************************/

static unsigned int
cvtdate(date,dummy,ep) /* convert ascii date to integer file fmt */
char *date, *dummy;
int *ep;
{
   int month, day, year;

   *ep = 0;                            /* assume no error */

   month = cvttwo(date);               /* cvt flds to binary */
   day   = cvttwo(date+3);
   year  = cvttwo(date+6);

   /* error check */

   if (month < 1 || month > 12 || day < 1 || day > 31 || year < 80 ||
       year > 99 || date[2] != '/' || date[5] != '/') {
      *ep = 1;
      return(0);
   }

   /* turn it into the integer format */

   return( ((year - 80) << 9) + (month << 5) + day );
}


static unsigned int
cvttime(time,ampmp,ep) /* convert ascii time to integer file fmt */
char *time, *ampmp;
int *ep;
{
   unsigned int hh, mm, ss, ampm;

   *ep = 0;                            /* assume no error */

   hh = cvttwo(time);                  /* cvt flds to binary */
   mm = cvttwo(time+3);
   ss = cvttwo(time+6);

   ampm = toupper(*ampmp);             /* make sure its upper case */

   /* error check */

   if (hh > 12 || mm > 59 || ss > 59 || time[2] != ':' || time[5] != ':' ||
       (ampm != 'A' &&  ampm != 'P')) {
      *ep = 1;
      return(0);
   }

   /* turn it into the integer format */

   if (ampm == 'P')
      hh += 12;

   return( (hh << 11) + (mm << 5) + (ss >> 1) );
}


static int
cvttwo(cp)             /* convert 2 ascii chars to binary */
register char *cp;
{
   if (*cp == ' ')             /* it may have a leading blank */
      *cp = '0';

   return( ((*cp - '0') * 10) + (*++cp - '0') );
}
