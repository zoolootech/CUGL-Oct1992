/*  012  11-Oct-86  ovsort.c

        Sort routines for OVERVIEW.

        Copyright (c) 1986 by Blue Sky Software.  All rights reserved.
*/

#include "ov.h"

#ifndef NULL
#define NULL (0)
#endif

extern struct window cw;
extern struct file_ent files[];

static char *nullname = "";

int sortname(), sortext(), sortdate(), sortsize(), sortidx();

char *strchr();


/******************************************************************************
 **                             S O R T                                      **
 *****************************************************************************/

sort_asc() {           /* sort files in ascending order */
   cw.sortopt = 'A';
   sort_and_disp(NULL);
}

sort_desc() {          /* sort files in descending order */
   cw.sortopt = 'D';
   sort_and_disp(NULL);
}

sort_none() {          /* do not sort files (dos dir order) */
   sort_and_disp(sortidx);
}


sort_name() {          /* sort files by name */
   sort_and_disp(sortname);
}

sort_ext() {           /* sort files by extension */
   sort_and_disp(sortext);
}

sort_date() {          /* sort files by date */
   sort_and_disp(sortdate);
}

sort_size() {          /* sort files by size */
   sort_and_disp(sortsize);
}


/*****************************************************************************
                         S O R T _ A N D _ D I S P
 *****************************************************************************/

sort_and_disp(sortrtn) /* sort files and update window */
int (*sortrtn)();
{
   char fname[MAX_NAMELEN+1];
   register struct file_ent *fp;

   strcpy(fname,files[cw.curidx].name); /* save current file name */

   sort_files(sortrtn);                /* sort files in desired order */

   /* after doing a sort, curidx no longer points to the same file.
      Search files[] for the old file name and move current pointer there. */

   for (cw.curidx = 0, fp = files; cw.curidx < cw.nfiles; fp++, cw.curidx++)
       if (strcmp(fp->name,fname) == 0)
          break;

   adjust_window();                    /* update window display */
   update_window(1);
}


/******************************************************************************
 **                         S O R T _ F I L E S                              **
 *****************************************************************************/

sort_files(sortrtn)    /* sort the file info by the user selected method */
int (*sortrtn)();
{
   if (sortrtn)                /* if caller specified a sort routine, */
      cw.sortfunc = sortrtn;   /*   make it the new default */

   qsort(files,cw.nfiles,sizeof(struct file_ent),cw.sortfunc);

}


#define sortorder(x) ((cw.sortopt == 'A') ? x : -x)

sortname(f1,f2)        /* support function to sort by name */
struct file_ent *f1, *f2;
{
   return(sortorder(strcmp(f1->name,f2->name)));
}

static int
sortsize(f1,f2)        /* support function to sort by size */
register struct file_ent *f1, *f2;
{
   int rc;

   if (f1->size > f2->size)
      rc = 1;
   else
      if (f1->size < f2->size)
         rc = -1;
      else
         rc = strcmp(f1->name,f2->name);       /* sort by name if same size */

   return(sortorder(rc));
}

static int
sortext(f1,f2)         /* support function to sort by extension */
struct file_ent *f1, *f2;
{
   int rc;
   register char *ep1, *ep2;

   ep1 = strchr(f1->name,'.');         /* find the extension for f1 */
   if (ep1 == NULL)
      ep1 = nullname;
   else
      ep1++;

   ep2 = strchr(f2->name,'.');         /* find the extension for f2 */
   if (ep2 == NULL)
      ep2 = nullname;
   else
      ep2++;

   if ((rc = strcmp(ep1,ep2)) == 0)    /* sort by whole name if ext's equal */
      rc = strcmp(f1->name,f2->name);

      return(sortorder(rc));
}

static int
sortdate(f1,f2)        /* support function to sort by date/time */
register struct file_ent *f1, *f2;
{
   int rc;

   if (f1->date > f2->date)
      rc = 1;
   else
      if (f1->date < f2->date)
         rc = -1;
      else
         if (f1->time > f2->time)
            rc = 1;
         else
            if (f1->time < f2->time)
               rc = -1;
            else
               rc = strcmp(f1->name,f2->name);

   return(sortorder(rc));
}

static int
sortidx(f1,f2)         /* sort by index # (dos order) */
struct file_ent *f1, *f2;
{
   if (f1->index > f2->index)
     return(1);
   else
     return(-1);
}
