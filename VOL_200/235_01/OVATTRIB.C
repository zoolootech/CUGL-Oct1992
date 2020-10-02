/*  015  14-Feb-87  ovattrib.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include "ov.h"

#define ABOXROWS (6)                   /* # rows in attribute dialog box */
#define ABOXCOLS (26)                  /* # columns/row in dialog box */
#define ABOXFROW (FIRST_NROW+3)        /* first screen row of dialog box */
#define ABOXFCOL (28)                  /* first screen col of dialog box */

extern struct menu_selection top_file_menu[], *top_menu;

int attrib_a(), attrib_h(), attrib_r(), attrib_s(), attrib_set(), attrib_quit();

struct menu_selection top_attrib_menu[] = {
   { "Enter", "Change the attributes as shown", attrib_set, top_file_menu },
   { "Archive", "Toggle the Archive attribute", attrib_a, NULL },
   { "Hidden", "Toggle the Hidden attribute", attrib_h, NULL },
   { "Read/only", "Toggle the Read/Only attribute", attrib_r, NULL },
   { "System", "Toggle the System attribute", attrib_s, NULL },
   { "Quit", "Don't change the current attributes", attrib_quit, top_file_menu },
   { NULL, NULL, NULL, NULL }
};

static char *boxsave;
static char *file_to_set;
static unsigned char attrib_to_set;
static char *tagged_name = "TAGGED FILES";

extern WINDOW cw;
extern FILE_ENT files[];
extern unsigned char attribs[], restricted;


/******************************************************************************
 **                    A T T R I B _ C U R R E N T                           **
 *****************************************************************************/

attrib_current() {     /* set the attributes of the current file */

   FILE_ENT *fp = &files[cw.curidx];

   /* setup the attribute screen with current file name and current file
      attributes */

   attrib_setup(fp->name,fp->flags & (RDONLY | HIDDEN | SYSTEM | ARCHIVE));
}


/******************************************************************************
 **                    A T T R I B _ T A G G E D                             **
 *****************************************************************************/

attrib_tagged() {      /* set the attributes of all tagged files */

   /* setup the attribute screen with tagged file indicator and default
      attributes (none) */

   attrib_setup(tagged_name,0);
}


/******************************************************************************
 **                      A T T R I B _ Q U I T                               **
 *****************************************************************************/

attrib_quit() {        /* quit the attribute setup with changing any */

   top_menu = top_file_menu;   /* restore file menu as main */

   restricted = FALSE;         /* enable all commands */

   /* remove the dialog box if attrib_set didn't already */

   if (boxsave) {
      popdwn(ABOXFROW,ABOXFCOL,ABOXROWS,ABOXCOLS,boxsave); /* remove the box */
      free(boxsave);              /* release display memory save area */
   }
}


/******************************************************************************
 **                    A T T R I B _ S E T U P                               **
 *****************************************************************************/

attrib_setup(name,def_attribs) /* setup the attribute modification window */
char *name;
int def_attribs;
{

   top_menu = top_attrib_menu; /* setup the attrib menu as the main menu */

   restricted = TRUE;          /* disable cursor movement, etc */

   /* allocate a save area for the video memory overlayed by the dialog box */

   boxsave = (char *) Malloc(ABOXROWS * ABOXCOLS * 2);

   /* create a dialog box for the attribute display */

   setvattrib(DIS_BOX);

   popup(ABOXFROW,ABOXFCOL,ABOXROWS,ABOXCOLS,boxsave);

   disp_str_at(" Set Attributes ",ABOXFROW,ABOXFCOL+2);  /* box title */

   setvattrib(DIS_HIBOX);              /* display the allowed attrib settings */
   disp_str_at(" R H S A ",ABOXFROW+2,ABOXFCOL+15);
   setvattrib(DIS_BOX);

   gotorc(ABOXFROW+3,ABOXFCOL+2);      /* display the file name */
   out_str(name,22,' ');

   gotorc(ABOXFROW+3,ABOXFCOL+15);     /* display the initial attributes */
   disp_attrib(def_attribs);

   setvattrib(DIS_NORM);

   file_to_set = name;                 /* remember the settings */
   attrib_to_set = def_attribs;
}


/******************************************************************************
 **                      A T T R I B _ S E T                                 **
 *****************************************************************************/

attrib_set() {         /* give the current or tagged the selected attributes */

   register int i;
   register FILE_ENT *fp;

   /* we are actually going to set attributes, remove the dialog box so its
      not in the way */

   popdwn(ABOXFROW,ABOXFCOL,ABOXROWS,ABOXCOLS,boxsave); /* remove the box */

   free(boxsave);              /* release display memory save area */
   boxsave = NULL;             /* so attrib_quit knows box is removed */

   if (file_to_set == tagged_name) {           /* set tagged files? */
      for (i = 0, fp = files; i < cw.nfiles && !brkout(); i++, fp++)
         if (fp->flags & TAGGED)
            change_attrib(fp,i);

   } else              /* only set current file */

      change_attrib(&files[cw.curidx],cw.curidx);

   attrib_quit();                      /* use the quit code to exit */
}


/******************************************************************************
 **                     C H A N G E _ A T T R I B                            **
 *****************************************************************************/

static int
change_attrib(fp,idx)          /* change the attributes of a file */
register FILE_ENT *fp;
register int idx;
{
   char *fn;

   fn = fname(fp);
   setattrib(fn,attrib_to_set);        /* zap the file attributes */
   free(fn);

   /* update the flags in the files[] structure */

   fp->flags = (fp->flags & ~(ARCHIVE | RDONLY | HIDDEN | SYSTEM)) | attrib_to_set;

   /* update the display if attributes are shown and this file is on screen */

   if (cw.info_display && on_screen(idx)) {
      gotorc(idx2sr(idx),idx2sc(idx));
      disp_file(fp,idx == cw.curidx);
   }
}


/******************************************************************************
 **                           A T T R I B _ x                                **
 *****************************************************************************/

static int
attrib_a() {           /* toggle the ARCHIVE attribute */

   attrib_toggle(ARCHIVE);
}


static int
attrib_r() {           /* toggle the READ ONLY attribute */

   attrib_toggle(RDONLY);
}


static int
attrib_h() {           /* toggle the HIDDEN attribute */

   attrib_toggle(HIDDEN);
}


static int
attrib_s() {           /* toggle the SYSTEM attribute */

   attrib_toggle(SYSTEM);
}


/******************************************************************************
 **                      A T T R I B _ T O G G L E                           **
 *****************************************************************************/

static int
attrib_toggle(at)      /* toggle the state of the passed attribute */
int at;
{

   attrib_to_set ^= at;                /* first toggle the attribute */

   gotorc(ABOXFROW+3,ABOXFCOL+15);     /* then redisplay the current settings */
   setvattrib(DIS_BOX);
   disp_attrib(attrib_to_set);
   setvattrib(DIS_NORM);
}
