/*  030  14-Feb-87  ovdir.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include "ov.h"
#include "dosfile.h"

#define DCOLSIZ 15
#define DCOLS (SCREEN_COLS / DCOLSIZ)

#define dr2sr(r) (FIRST_NROW + (r - drbase))
#define dc2sc(c) ((c - dcbase) * DCOLSIZ)

#define TO_NONE 0
#define TO_SUBDIR 1
#define TO_SIBLING 2
#define TO_PARENT 3
#define TO_ROOT 4

char *strrchr(), *strupr();
struct search_block *nxtfile();

struct dir_ent {
   struct dir_ent *subdir;
   struct dir_ent *sibling;
   struct dir_ent *parent;
   struct dir_ent *prev_sib;
   char name[13];
   unsigned char row;
   unsigned char col;
};

static last_drive = ' ';
static int drow, dcol, ldrow;
static char dirpath[MAX_PATHLEN+6];
static int drbase, drend, dcbase, dcend;
static struct dir_ent *curdir, *logdir, *findir();
static struct dir_ent root = { NULL, NULL, NULL, NULL, "", 0, 0 };

int dir_exit(), dir_login(), dir_mkdir(), dir_rmdir(), dir_new();
extern struct menu_selection top_file_menu[], *top_menu;

struct menu_selection top_dir_menu[] = {
   { "Login", "Login (switch) to the highlighted directory", dir_login, NULL },
   { "Mkdir", "Make a subdirectory of the highlighted directory", dir_mkdir, NULL },
   { "New", "Reread and redisplay directory tree", dir_new, NULL },
   { "Rmdir", "Remove (delete) the highlighted directory", dir_rmdir, NULL },
   { "Quit", "Return to file display", dir_exit, top_file_menu },
   { NULL, NULL, NULL, NULL }
};

extern struct window cw;
extern unsigned char dir_display, restricted;
char *strchr();


/******************************************************************************
 **                            D T R E E                                     **
 *****************************************************************************/

dtree() {              /* display / work with directory tree */

   int drive;

   dir_display = TRUE;                 /* dir tree is (will be) displayed */
   restricted = TRUE;                  /* disable some file commands */

   /* scan the current disk and create the internal directory tree if it
      hasn't already been built or the user has changed disks */

   if ((drive = current_drive()) != last_drive) {  /* has the drive switched? */

      if (root.subdir) {                       /* delete current tree */
         del_dtree(root.subdir);               /* if drive switch */
         root.subdir = NULL;
      }

      last_drive = drive;

      *dirpath = '\0';                         /* scan_dir() starts at */
      strncat(dirpath,cw.dirbuf,3);            /*   drives root dir */
      strcpy(root.name,dirpath);

      disp_msg(1,"Scanning disk");             /* takes awhile, tell user */

      scan_dir(&root);                         /* build new dir tree */

      clr_msg();                               /* done scanning */

      drbase = dcbase = 0;                     /* assume dir */
      drend = NAME_ROWS;                         /* will start */
      dcend = DCOLS;                               /* at the root */

      curdir = findir();                       /* locate current dir in tree */

   } else {            /* don't need to scan the disk */

      curdir = findir();               /* point to logged in dir */
      adj_dir_dis();                   /* make sure logged dir will show */
   }

   logdir = curdir;                    /* remember the logged dir */

   /* now display the current portion of the dir tree */

   show_tree();                        /* let user see it */

   /* this is a hack, but... if a new drive was scanned, we don't know for
      sure if the curent dir is really displayed because the row/column
      values aren't calculated until it's actully displayed.  If the
      user has more than a screen's worth of directories and he is in one
      of the ones off screen, adjust the offsets and redisplay, sigh... */

   if (adj_dir_dis())
      show_tree();

   top_menu = top_dir_menu;    /* setup the dir menu as the main menu */
}


/******************************************************************************
                              D I R _ N E W
 *****************************************************************************/

dir_new() {            /* rescan the disk and redisplay the dir tree */

   last_drive = ' ';   /* simply force a rescan */
   dtree();            /*   and let dtree() do the work */
   update_vol_stats(); /* in case its a new volume */
}


/******************************************************************************
 **                       D I R _ E X I T                                    **
 *****************************************************************************/

dir_exit() {           /* exit the dir display, return to file display */

   dir_display = FALSE;                /* dir tree will not be displayed */
   restricted = FALSE;                 /* allow all file commands */

   top_menu = top_file_menu;           /* file menu is main again */

   update_header();                    /* always rewrite the entire screen */
   refresh_screen(0);                  /* 'cause there may be > windows */
}


/******************************************************************************
 **                         S C A N _ D I R                                  **
 *****************************************************************************/

scan_dir(dp)           /* scan the specified dir tree for other directories */
struct dir_ent *dp;
{
   int dplen;
   int firsttime = TRUE;
   struct search_block *sbp;
   register struct dir_ent *ndp, *ldp = NULL;

   /* build the pathname of the dir to scan */

   dplen = strlen(dirpath);                    /* remember callers length */
   if (strcmp(dp->name+2,"\\") != 0) {         /* special case if root dir */
      strcat(dirpath,dp->name);                /* add name of dir to scan */
      strcat(dirpath,"\\");
   }
   strcat(dirpath,"*.*");                      /* add wildcard string */

   /* scan all files in directory looking for subdirectories.  When a
      subdirectory is found, add it to the dir_ent tree.  Note, the . and
      .. directory entries are ignored. */

   while (sbp = nxtfile(dirpath,0x16,&firsttime))
      if (sbp->attrib & DIR && *sbp->fn != '.') {

         /* found a subdir we want, build a struct dir_ent for it */
         ndp = (struct dir_ent *) Malloc(sizeof(struct dir_ent));
         strcpy(ndp->name,sbp->fn);
         ndp->subdir = NULL;
         ndp->sibling = NULL;
         ndp->parent = dp;
         ndp->prev_sib = ldp;

         /* now link it to the dir_ent tree either as a subdir of the
            parent (1st one only) or a sibling of the last one */

         if (ldp)
            ldp->sibling = ndp;        /* not 1st, is a sibling */
         else
            dp->subdir = ndp;          /* 1st one, subdir of parent */

         ldp = ndp;                    /* new one is now the last one */
      }

   /* if any subdirectories were found, scan 'em.  This isn't done
      earlier so the file search isn't complicated by the directory
      switches. */

   if (ldp) {                            /* NULL if no sub's found */
      dirpath[strlen(dirpath)-3] = '\0'; /* remove *.* for next level */
      ldp = dp->subdir;                  /* start with the first one */
      do {
         scan_dir(ldp);                /* call ourselves to scan this subtree */
      } while (ldp = ldp->sibling);    /* do all the subs found */
   }

   dirpath[dplen] = '\0';              /* restore dir pathname for caller */
}


/******************************************************************************
 **                           D E L _ D T R E E                              **
 *****************************************************************************/

del_dtree(dp)          /* purge the current in memory dir tree structure */
register struct dir_ent *dp;
{

   register struct dir_ent *ndp;

   /* delete this subdirectory ENTRY and all sibling ENTRIES (not the
      actual directories) */

   do {

      if (dp->subdir)            /* if it has a subdir, make a recursive call */
         del_dtree(dp->subdir);  /* to delete the substructure */

      ndp = dp->sibling;         /* get address of any sibling */

      free((char *)dp);          /* free the dir_ent space itself */

   } while (dp = ndp);           /* do until no more siblings */

}


/******************************************************************************
 **                        S H O W _ T R E E                                 **
 *****************************************************************************/

static int
show_tree() {          /* setup and display the dir tree */

   drow = 0;                           /* disp_dtree() needs these set */
   ldrow = dcol = -1;
   disp_dtree(&root);                  /* redisplay new part of tree */
}


/******************************************************************************
 **                         D I S P _ D T R E E                              **
 *****************************************************************************/

disp_dtree(dp)                 /* display the directory tree */
register struct dir_ent *dp;
{
   int next_row;

   ++dcol;             /* each invocation is another directory level */

   /* display this subdirectory and all its siblings */

   do {

      /* if drow != ldrow, then this is the first entry on this display
         row, clear the line to remove any old junk that might be there */

      if (drow != ldrow && drow >= drbase && drow < drend) {
         gotorc(dr2sr(drow),dc2sc(dcbase));
         clr_eol();
         ldrow = drow;
      }

      /* mark the dir_ent's with the logical display row/col so that
         dir_move() has an easy time displaying the entries */

      dp->row = drow;
      dp->col = dcol;

      /* only display a dir  entry if it falls within the boundries, figure
         out if this one does and display the name if so - the dir under
         the pointer is highlighted, the logged dir is 'tagged' all others
         are displayed 'normal' */

      if (dir_on_screen(drow,dcol))
         disp_dir_name(dp,dp == curdir ? DIS_HIGH :
                       (dp == logdir ? DIS_TAGD : DIS_NORM));

      /* if this dir has a subdir, process that now */

      if (dp->subdir) {

         next_row = drow + 1;          /* remember where next sibling goes */

         disp_dtree(dp->subdir);       /* display the subdir tree */

         /* if the current dir has more siblings (more names to display at
            this level) and the subdir tree changed the next display row,
            output bar spacers to get down to the sibling */

         if (drow > next_row && dp->sibling)
            for (; next_row < drow; next_row++)
               if (dir_on_screen(next_row,dcol))
                  disp_char_at(0xb3,dr2sr(next_row),dc2sc(dcol));

      } else                           /* has no subdir */
         ++drow;                       /* thats all for this line, advance */

   } while (dp = dp->sibling);         /* do all siblings at this level */

   --dcol;                             /* this level is done */

   /* if the root level is done (dcol < 0 - meaning there are no more dir
      entries to be displayed) and we didn't reach the bottom of the display
      area, clear out the rest of the lines to remove any garbage that
      might be there */

   if (dcol < 0 && drow < NAME_ROWS)
      for (; drow < NAME_ROWS; drow++) {
         gotorc(dr2sr(drow),dc2sc(dcbase));
         clr_eol();
      }

}


/******************************************************************************
 **                    D I S P _ D I R _ N A M E                             **
 *****************************************************************************/

disp_dir_name(dp,va)   /* display a single dir name */
register struct dir_ent *dp;
int va;
{
   gotorc(dr2sr(dp->row),dc2sc(dp->col));      /* move to display location */

   /* display the leadin bar to the dir name, it depends on whether
      this is the root, the 1st subdir, the last sibling, and if any
      siblings follow */

   if (dp->col) {                                 /* root dir? */
      if (dp->prev_sib == NULL)                   /* 1st subdir? */
         disp_char((dp->sibling) ? 0xc2 : 0xc4);  /* siblings follow? */
      else
         disp_char((dp->sibling) ? 0xc3 : 0xc0);
   } else
      disp_char(' ');                          /* root - no leadin */

   if (va != DIS_NORM)                         /* setup highlight if needed */
      setvattrib(va);

   disp_char(' ');
   disp_str(dp->name);                         /* display the name */
   disp_char(' ');

   /* draw bar to subdir or blank fill name */

   disp_rep(dp->subdir ? 0xc4 : ' ',DCOLSIZ - strlen(dp->name) - 3);

   if (va != DIS_NORM)         /* restore normal attribute if changed */
      setvattrib(DIS_NORM);
}


/******************************************************************************
 **                       D I R _ M O V E                                    **
 *****************************************************************************/

dir_move(move_cmd)     /* move the directory pointer around */
int move_cmd;
{
   int moved = TO_NONE;
   int redisplayed = FALSE;
   register struct dir_ent *last_dir, *cdp;

   last_dir = cdp = curdir;    /* remember where we are/were and fast ptr */

   switch (move_cmd) {

      case RIGHT:
         if (cdp->subdir) {
            cdp = cdp->subdir;
            moved = TO_SUBDIR;
         } else
            if (cdp->sibling) {
               cdp = cdp->sibling;
               moved = TO_SIBLING;
            }
            /* stuff about parents goes here */
         break;

      case LEFT:
         if (cdp->prev_sib) {
            cdp = cdp->prev_sib;
            moved = TO_SIBLING;
         } else
            if (cdp->parent) {
               cdp = cdp->parent;
               moved = TO_PARENT;
            }
         break;

      case UP:
         if (cdp->prev_sib) {
            cdp = cdp->prev_sib;
            moved = TO_SIBLING;
         }
         break;

      case DOWN:
         if (cdp->sibling) {
            cdp = cdp->sibling;
            moved = TO_SIBLING;
         }
         break;

      case HOME:
         cdp = &root;
         moved = TO_ROOT;
         break;

      case GOPAR:
         if (cdp->parent) {
            cdp = cdp->parent;
            moved = TO_PARENT;
         }
      break;

   }

   curdir = cdp;           /* assign it */

   /* adjust and redisplay the pathname as the user moves the dir pointer */

   if (moved != TO_NONE)
      update_dirpath(moved);

   /* display a different section of dir tree if current isn't displayed */

   if (adj_dir_dis()) {                /* is adjustment needed? */
      show_tree();                     /* display the tree */
      redisplayed = TRUE;              /* save a disp_dir_name() call */
   }

   /* deselect the last dir if the dir pointer moved and the last one
      is still on the screen and the entire tree wasn't redisplayed */

   if (!redisplayed && last_dir != curdir &&
       dir_on_screen(last_dir->row,last_dir->col))
      disp_dir_name(last_dir,last_dir == logdir ? DIS_TAGD : DIS_NORM);

   /* select (highlight) a new dir entry if pointer moved and the tree
      wasn't redisplayed */

   if (last_dir != curdir && !redisplayed) {
      disp_dir_name(curdir,DIS_HIGH);
   }
}


/******************************************************************************
 **                       D I R _ L O G I N                                  **
 *****************************************************************************/

dir_login() {          /* login the dir selected by the dir pointer */

   /* switch to selected dir, if can't switch, user may have changed
      disks, force a scan of the disk for dirs */

   if (switch_dir(dirpath) != 0)       /* switch to the selected dir */
      last_drive = ' ';

   dir_exit();                         /* exit the dir display mode */

}


/******************************************************************************
 **                       D I R _ R M D I R                                  **
 *****************************************************************************/

dir_rmdir() {          /* remove the dir selected by the dir pointer */

   int moveto;
   register struct dir_ent *cdp;

   cdp = curdir;               /* try to reduce code size */

   /* don't even let user try to delete the root or current directories */

   if (cdp->col == 0 || strcmp(dirpath,cw.dirbuf) == 0)
      show_error(0,4,3,"You can't delete the ",(cdp->col == 0) ? "root" :
                "current"," directory!");

   /* use DOS to acutally remove the directory */

   if (rmdir(dirpath) != 0)
      show_error(1,4,1,"Unable to remove dir: ");

   /* Okay, the dir is gone, now remove it from the dir tree */

   if (cdp->prev_sib)                                  /* make prev sib -> */
      cdp->prev_sib->sibling = cdp->sibling;           /* next sib */

   if (cdp->sibling) {                                 /* next sib -> */
      cdp->sibling->prev_sib = cdp->prev_sib;          /* prev sib && */
      if (cdp->parent->subdir == cdp)                  /* parent ->   */
         cdp->parent->subdir = cdp->sibling;           /* next sib    */
   }

   if (cdp->prev_sib == NULL && cdp->sibling == NULL)  /* parent has no */
      cdp->parent->subdir = NULL;                      /* more sub's */

   /* when deleting a subdirectory, try to make a sibling the current dir,
      if none, go back to parent */

   if (cdp->sibling) {
      curdir = cdp->sibling;
      moveto = TO_SIBLING;
   } else
      if (cdp->prev_sib) {
         curdir = cdp->prev_sib;
         moveto = TO_SIBLING;
      } else {
         curdir = cdp->parent;
         moveto = TO_PARENT;
      }

   update_vol_stats();                 /* should be more free space now */

   update_dirpath(moveto);             /* show user what the dir pathname is */

   free((char *)cdp);                  /* release mem used by the dir entry */

   /* redisplay the tree to adjust the display and reassign row, col values */

   adj_dir_dis();
   show_tree();
}


/******************************************************************************
 **                       D I R _ M K D I R                                  **
 *****************************************************************************/

dir_mkdir() {          /* make a subdirectory in the selected dir */

   int rc;
   char *name, *dirend;
   register struct dir_ent *ndp, *ldp;

   /* ask user what to call the new directory */

   name = strupr(prompt("","Enter the new subdirectory name: ",NULL,0,12));
   if (strlen(name) == 0)
      return;

   /* update dirpath to include the users new name */

   dirend = dirpath + strlen(dirpath); /* remember current dirpath end */
   if (*(dirend-1) != '\\')
      strcat(dirend,"\\");
   strcat(dirend,name);                /* add users dir name */

   /* use DOS to acutally make the directory */

   rc = mkdir(dirpath);                /* create it */

   *dirend = '\0';                     /* fixup dirpath */

   if (rc != 0)
      show_error(1,4,1,"Unable to make dir: ");

   /* Okay, the dir is created, now make a dir_ent and add it to the dir tree */

   ndp = (struct dir_ent *) Malloc(sizeof(struct dir_ent));
   strcpy(ndp->name,name);
   ndp->subdir = NULL;
   ndp->sibling = NULL;
   ndp->parent = curdir;

   if (ldp = curdir->subdir) {                 /* any sibs to new dir? */
      while (ldp->sibling)                     /* find end of sib list */
         ldp = ldp->sibling;
      ndp->prev_sib = ldp;                     /* new one is last */
      ldp->sibling = ndp;
   } else {
      ndp->prev_sib = NULL;                    /* no sibs */
      curdir->subdir = ndp;                    /* parent now has subdir */
   }

   update_vol_stats();                 /* less free space now */

   /* redisplay the tree to adjust the display and reassign row, col values */

   show_tree();
}


/******************************************************************************
 **                    U P D A T E _ D I R P A T H                           **
 *****************************************************************************/

static int
update_dirpath(moved)          /* update & redisplay dir path */
int moved;
{
   char *cp;

   switch (moved) {

      case TO_SUBDIR:                  /* moved to a subdir */
         if (curdir->col != 1)         /* append a \ if parent wasn't root */
            strcat(dirpath,"\\");
         strcat(dirpath,curdir->name); /* append the subdirectory name */
         break;

      case TO_SIBLING:                 /* moved to a sibling */
         *(strrchr(dirpath,'\\')+1) = '\0';
         strcat(dirpath,curdir->name);
         break;

      case TO_PARENT:                  /* moved to parent */
         cp = strrchr(dirpath,'\\');
         if (curdir->col == 0)
            ++cp;
         *cp = '\0';
         break;

      case TO_ROOT:                    /* moved to the root */
        dirpath[3] = '\0';
        break;
   }

   gotorc(VOL_ROW,PATH_COL+1);
   out_str(dirpath,65,' ');
}


/*****************************************************************************
                          A D J _ D I R _ D I S
 *****************************************************************************/

adj_dir_dis() {        /* make sure current dir ent will be on screen */

   register int r, c;

   /* adjust bounds and redisplay dir tree if the current entry is not
      displayed */

   r = curdir->row;  c = curdir->col;  /* cutout a level of indirection */

   if (!dir_on_screen(r,c)) {

      if (r >= drend)                  /* current below display? */
         drbase = r - NAME_ROWS + 1;
      else
         if (r < drbase)               /* current above display? */
            drbase = r;

      if (c >= dcend)                  /* current right of display? */
         dcbase = c - DCOLS + 1;
      else
         if (c < dcbase)               /* current left of display? */
            dcbase = c;

      drend = drbase + NAME_ROWS;      /* reset display end markers */
      dcend = dcbase + DCOLS;

      return(1);                       /* tell caller to redisplay */
   }

   return(0);                          /* no adjustment needed */
}


/*****************************************************************************
                             F I N D I R
 *****************************************************************************/

static struct dir_ent *
findir() {             /* find the current dir_ent */

   register char *lcp;
   char *cp = cw.dirbuf+3;
   char dirname[MAX_NAMELEN+2];
   register struct dir_ent *dp = &root;

   *dirpath = '\0';
   strncat(dirpath,cw.dirbuf,3);       /* roots better be the same */

   while (strcmp(dirpath,cw.dirbuf) != 0) {    /* have we found it yet? */

      /* haven't found it yet, go down another dir level, also do some
         protective error checks, I don't feel real secure 'bout this yet */

      if (dp == NULL || (dp = dp->subdir) == NULL || (lcp = cp) == NULL)
         return(&root);                        /* shouldn't happen, but... */

      if (cp = strchr(cp+1,'\\')) {            /* isolate the next dir level */
         *dirname = '\0';                      /*   in cw.dirbuf */
         strncat(dirname,lcp,cp-lcp);
      } else
         strcpy(dirname,lcp);                  /* this must be the last level */

      strcat(dirpath,dirname);                 /* add dir name to full path */

      if (*(lcp = dirname) == '\\')            /* 1st doesn't have leading \ */
         lcp++;                                /*   others do */

      /* check all siblings at this level until we find it (or run out?) */

      while (dp && strcmp(lcp,dp->name) != 0)
         dp = dp->sibling;
   }

   return(dp);                 /* this should be the one */
}


/*****************************************************************************
                        D I R _ O N _ S C R E E N
 *****************************************************************************/

static int
dir_on_screen(r,c)     /* determine if dir name is displayed */
register int r, c;
{
   return(r >= drbase && r < drend && c >= dcbase && c < dcend);
}
