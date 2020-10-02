/*  005  24-Jan-87  ovselect.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include "ov.h"

extern MENU top_file_menu[], *top_menu;

int selatr_quit();
int selatr_set(), selatr_a(), selatr_d(), selatr_h(), selatr_r(), selatr_s();

MENU top_selatr_menu[] = {
   { "Enter", "Change the selection attributes as shown", selatr_set, top_file_menu },
   { "Archive", "Toggle selection of entries with Archive attribute", selatr_a, NULL },
   { "Dir", "Toggle selection of directory entries", selatr_d, NULL },
   { "Hidden", "Toggle selection of entries with Hidden attribute", selatr_h, NULL },
   { "Read/only", "Toggle selection of entries with Read/Only attribute", selatr_r, NULL },
   { "System", "Toggle selection of entries with System attribute", selatr_s, NULL },
   { "Quit", "Don't change the current selection attributes", selatr_quit, top_file_menu },
   { NULL, NULL, NULL, NULL }
};

static unsigned char selatr_to_set;

extern WINDOW cw;
extern FILE_ENT files[];
extern unsigned char restricted;

char *strupr();


/****************************************************************************
                           S E L _ A T T R I B
 ****************************************************************************/

sel_attrib() {         /* set the selection attributes */

   top_menu = top_selatr_menu; /* setup the select attrib menu as main menu */

   restricted = TRUE;          /* disable cursor movement, etc */

   selatr_to_set = cw.selatrs; /* initial attribs to select */
}


/*****************************************************************************
                            S E L A T R _ S E T
 *****************************************************************************/

selatr_set() {         /* set the selection attributes as show */

   cw.selatrs = selatr_to_set;         /* assign selection attributes */

   renew_window();                     /* reget/display files */

   selatr_quit();                      /* back to normal display mode */
}


/*****************************************************************************
                          S E L A T R _ Q U I T
 *****************************************************************************/

selatr_quit() {        /* exit back to normal file display mode */

   top_menu = top_file_menu;   /* restore file menu as main */

   restricted = FALSE;         /* enable all commands */
}


/*****************************************************************************
                             S E L A T R _ x
 *****************************************************************************/

selatr_a() {           /* toggle selection archive attribute */

   selatr_toggle(ARCHIVE);
}

selatr_d() {           /* toggle selection dir attribute */

   selatr_toggle(DIR);
}

selatr_h() {            /* toggle selection hidden attribute */

   selatr_toggle(HIDDEN);
}

selatr_r() {           /* toggle selection read/only attribute */

   selatr_toggle(RDONLY);
}

selatr_s() {           /* toggle selection system attribute */

   selatr_toggle(SYSTEM);
}


/*****************************************************************************
                          S E L A T R _ T O G G L E
 *****************************************************************************/

selatr_toggle(at)      /* toggle the state of the passed attribute */
int at;
{
   selatr_to_set ^= at;                /* toggle the attribute */

   gotorc(MASK_ROW+1,MASK_COL-1);      /* then redisplay the current settings */
   disp_attrib(selatr_to_set);

   if (at == DIR)                      /* disp_attrib() doesn't do dirs */
      disp_str(selatr_to_set & DIR ? " D" : " .");
}


/*****************************************************************************
                            S E L _ A L L
 *****************************************************************************/

sel_all() {            /* select everything in directory */

   cw.selatrs = ARCHIVE | DIR | HIDDEN | RDONLY | SYSTEM;

   mask_off();                 /* clear selection mask and renew window */
}


/*****************************************************************************
                            S E T _ M A S K
 *****************************************************************************/

set_mask() {           /* set the file selection mask */

   char *mask;

   mask = strupr(prompt("","Enter Selection Mask: ",NULL,0,MASK_LEN));
   if (strlen(mask) == 0)
      return;

   strcpy(cw.mask,mask);               /* copy new mask to window */

   renew_window();                     /* renew the window */
}


/*****************************************************************************
                            C L R _ M A S K
 *****************************************************************************/

clr_mask() {           /* clear the file selection mask */

   int ans;

   ans = ask("Clear the selection mask? (y/N): ");
   if (yes(ans))
      mask_off();                      /* wipe out the mask */
}


/*****************************************************************************
                         M A S K _ O F F
 *****************************************************************************/

static int
mask_off() {           /* force the selection mask off */

   *cw.mask = '\0';                 /* wipe out the mask */
   cw.maskcmp = 1;                  /* mask means include next time */
   renew_window();                  /* update the file display */
}

/*****************************************************************************
                       I N V E R T _ M A S K
 *****************************************************************************/

invert_mask() {        /* invert the include/exclude meaning of the mask */

   cw.maskcmp ^= 1;    /* invert the flag */
   renew_window();     /* update the file display */
}


/*****************************************************************************
                         S E L _ T A G G E D
 *****************************************************************************/

sel_tagged() {         /* "Select" the tagged files */

   register int i;
   register struct file_ent *fp;

   /* to select the tagged files, we delete every files[] that isn't tagged,
      note that the file isn't deleted, just the files[] entry */

   for (i = 0, fp = files; i < cw.nfiles; i++, fp++)
      if (!(fp->flags & TAGGED))
         delent(fp);

   disp_file_stats();                  /* disp updated file stats */
   packfiles();                        /* pack the files structure */
   adjust_window();                    /* redisplay window */
   update_window(1);
}
