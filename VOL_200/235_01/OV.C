/*  093  27-Feb-87  ov.c

                            O V E R V I E W

        This program allows the user to perform common operations on
        disk files in a visual, tabular manner.

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#define VERSION "OverView 1.02, 1-Mar-87"

#include <stdio.h>
#include <setjmp.h>
#include <dos.h>
#include "ov.h"


int brkhit;                            /* NZ when ^Break/^C hit */
int winupdate;                         /* NZ when windows need updating */
unsigned char anyshowall;              /* NZ when show all is active */
unsigned char dir_display;             /* NZ when dir tree displayed */
unsigned char def_display;             /* NZ when define screen displayed */
unsigned char help_display;            /* NZ when help is active */
unsigned char view_display;            /* NZ when viewing a file */
unsigned char restricted;              /* NZ when some functions disabled */

char *ovname = NULL;                   /* name used to invoke (if >= DOS 3) */

char *initialdir;                      /* name of initial dir */

unsigned int dataseg;                  /* ov's data segment reg value */

FILE_ENT files[MAX_FILES];             /* array of file entries */

char *cantopen = "Can't open ";        /* common message */

jmp_buf back_to_main;                  /* error handling jmp_buf block */

extern unsigned char _osmajor;         /* DOS major version # */

/* function declarations */

char *getcwd();
FILE_ENT *nxtfile();
int sel_all(), define(), help(), show_all();
int quit(), login(), erase_current(), erase_tagged(), view(), sortname();
int copy_current(), copy_tagged(), info(), renew(), do_cmd(), spawn_cli();
int dtree(), dir_login(), dir_mkdir(), dir_rmdir(), dir_exit(), sel_attrib();
int ren_cur(), ren_tag(), set_mask(), clr_mask(), invert_mask(), sel_tagged();
int sort_asc(), sort_desc(), sort_name(), sort_ext(), sort_date(), sort_size();
int set_vol(), tag_current(), tag_name(), tag_date(), tag_reset(), tag_today();
int tag_modified(), tag_invert(), attrib_current(), attrib_tagged(), execute();
int win_open(), win_close(), win_expand(), win_next(), win_prev(), sort_none();

/* define the current window structure */

struct window cw = { NULL, NULL, 0, FIRST_NROW, NAME_ROWS, FIRST_NROW,
   NAME_ROWS, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0, 0, 0, NULL, 'A', sortname, 0, 0,
   RDONLY | HIDDEN | SYSTEM | DIR | ARCHIVE, 1 };

/* define some of the menus */

char current[] = "Current";
char tagged[] = "Tagged";

extern MENU top_file_menu[], top_dir_menu[];

MENU *top_menu = top_file_menu;

MENU attrib_menu[] = {
   { current,"Change the current file's attributes", attrib_current, NULL },
   { tagged, "Change the attributes of all tagged files", attrib_tagged, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU copy_menu[] = {
   { current, "Copy the current file", copy_current, NULL },
   { tagged, "Copy the tagged files", copy_tagged, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU erase_menu[] = {
   { current, "Erase the current file", erase_current, NULL },
   { tagged, "Erase all tagged files", erase_tagged, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU mask_menu[] = {
   { "Set", "Set the file mask", set_mask, NULL },
   { "Clear", "Clear the file mask", clr_mask, NULL },
   { "Invert", "Invert the meaning of the mask", invert_mask, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU ren_menu[] = {
   { current, "Rename or move the current file", ren_cur, NULL },
   { tagged, "Move all tagged files", ren_tag, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU select_menu[] = {
   { "Attrib", "Set selection attributes", sel_attrib, NULL },
   { "Mask", "Set, Clear, or Invert file name selection mask", NULL, mask_menu },
   { "Reset", "Reset selection criteria to defaults", sel_all, NULL },
   { "Showall", "Toggle selection of all files on current drive", show_all, NULL },
   { "Tagged", "Select all tagged files", sel_tagged, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU sort_submenu[] = {
   { "Ascending", "Sort files in ascending order", sort_asc, NULL },
   { "Descending", "Sort files in descending order", sort_desc, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU sort_menu[] = {
   { "Name", "Sort by primary name", sort_name, NULL },
   { "Ext", "Sort by extension", sort_ext, NULL },
   { "Date", "Sort by date", sort_date, NULL },
   { "Size", "Sort by size", sort_size, NULL },
   { "Options", "Set sort options", NULL, sort_submenu },
   { "Unsorted", "Do not sort entries", sort_none, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU tag_menu[] = {
   { current, "Tag/Untag the current file", tag_current, NULL },
   { "Date", "Tag files by date and time", tag_date, NULL },
   { "Invert", "Invert the tag state of all files", tag_invert, NULL },
   { "Modified", "Tag files with Archive attribute", tag_modified, NULL },
   { "Name", "Tag files by name", tag_name, NULL },
   { "Reset", "Untag all files", tag_reset, NULL },
   { "Today", "Tag all files created/modified today", tag_today, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU win_menu[] = {
   { "Open", "Open an additional window", win_open, NULL },
   { "Close", "Close the current window", win_close, NULL },
   { "Expand", "Expand the current window to full screen (closes others)",
                                                         win_expand, NULL },
   { "Next", "Switch to the next window", win_next, NULL },
   { "Prev", "Switch to the previous window", win_prev, NULL },
   { NULL, NULL, NULL, NULL }
};

MENU other_menu[] = {
   { "Attrib", "Change the attributes of one or more files", NULL, attrib_menu },
   { "Command", "Execute a single DOS command", do_cmd, NULL },
   { "Define", "Define screen parameters", define, NULL },
   { "Interperter", "Spawn a DOS command interpreter", spawn_cli, NULL },
   { "Select", "Set file selection criteria", NULL, select_menu },
   { "Volume", "Create or change the volume label", set_vol, NULL },
   { "Window", "Open, close, switch file display windows", NULL, win_menu },
   { "Xecute", "Execute a program, command, or batch file", execute, NULL },
   { "Other", "Help, Copy, Dir, Erase, Info, Login, New, Rename, Sort, Tag, View, Quit", NULL, top_file_menu },
   { NULL, NULL, NULL, NULL }
};

MENU top_file_menu[] = {
   { "Help", "Get help on using OVERVIEW", help, NULL },
   { "Copy", "Copy one or more files", NULL, copy_menu },
   { "Dir", "Display the disk's directory tree", dtree, top_dir_menu },
   { "Erase", "Erase (delete) one or more files", NULL, erase_menu },
   { "Info", "Toggle display of extra file information", info, NULL },
   { "Login", "Login (switch) to another disk/directory", login, NULL },
   { "New", "Reread and redisplay the directory", renew, NULL },
   { "Rename", "Rename or move one or more files", NULL, ren_menu },
   { "Sort", "Sort the file entries by Name, Extension, Date, or Size", NULL, sort_menu },
   { "Tag", "Tag one or more files", NULL, tag_menu },
   { "View", "View the current file", view, NULL },
   { "Other", "Attrib, Command, Define, Interperter, Select, Volume, Window, Xecute", NULL, other_menu },
   { "Quit", "Exit OVERVIEW and return to DOS", quit, NULL },
   { NULL, NULL, NULL, NULL }
};



/******************************************************************************
 **                           M A I N                                        **
 *****************************************************************************/

main(argc,argv)
int argc;
char *argv[];
{
   setvattrib(DIS_NORM);       /* select initial video attribute */

   /* display the signon while initializing */

   init_tty();                         /* init the terminal settings */
   ovlogo(VERSION);                    /* display the logo screen */

   /* init internal data structures and display a screen of files */

   {  struct SREGS sr;                 /* get OverView's data seg reg value */
      segread(&sr);                    /*   for a couple of other routines  */
      dataseg = sr.ds;                 /*   to use */
   }

   if (_osmajor >= 3)                  /* if dos 3.0 or more, argv[0] has */
      ovname = *argv;                  /* the name ov was started with */

   getcwd(cw.dirbuf,MAX_PATHLEN);          /* get current directory pathname */
   initdrive(*cw.dirbuf);                  /* init DRIVE_ENT 4 current drive */
   initialdir = Strdup(cw.dirbuf);         /* save the initial dir spec */

   getfiles();                             /* get files from current directory */
   adjust_window();                        /* set # rows, cols, etc */

   getchr();                               /* any keypress to continue */

   setup_file_scr();                       /* display the static screen image */
   update_header();                        /* display header info */
   update_window(1);                       /*   and a window of data */


   grabbrk(&brkhit);           /* grab the ^Break/^C interrupt handler */

   /* call setjmp to establish the error handling jmp_buf,  errors in
      subfuntions will return here to start the next command */

   setjmp(back_to_main);

   /* main processing, obey each user command, terminate when done or
      user enters the EOF character or hits ^Break/^C */

   while (1) {
      obey();             /* only returns after ^Z or ^Break/^C */
      quit();             /* doesn't return if quitting */
   }

}


/******************************************************************************
 **                           O B E Y                                        **
 *****************************************************************************/

obey() {               /* obey / process a user command */

   register int ch, i;
   char *dp, *dirplus();

   menu_init();                /* display the initial menu */

   while ((ch = getchr()) != EOF_CH) { /* get a char from the user */

      brkhit = 0;              /* clr brk flag before each ov command */

      switch (ch) {

         case LEFT: case RIGHT:        /* left, right arrows */
         case UP:   case DOWN:         /* up, down arrows */
         case HOME: case END:
         case PGDN: case PGUP:

            if (dir_display)
               dir_move(ch);           /* move the dir pointer */
            else if (view_display)
               view_move(ch);          /* move within the view buffer/file */
            else if (def_display)
               def_move(ch);           /* move within define screen mode */
            else if (!restricted)
               move_file(ch);          /* move the file pointer */

            break;


         case ' ': case 9:             /* advance the menu selection pointer */
            menu_advance();
            break;


         case 8:                       /* backup the menu selection pointer */
            menu_backup();
            break;


         case RETURN:                  /* CR - do current menu selection */
            menu_do_current();
            break;


         case 27:                      /* escape - return to top menu */
            menu_init();               /* reset the menu subsystem */
            break;


         case TAG:                     /* single key command to tag current */
            if (!restricted) {
               tag_current();
               if (cw.curidx < cw.nfiles-1)
                  move_file(DOWN);
            }
            break;


         case GOPAR:                   /* login to the parent dir */
            if (dir_display)
               dir_move(GOPAR);
            else
               if (!restricted) {
                  switch_dir(dp = dirplus(&files[cw.curidx],".."));
                  free(dp);
                  menu_init();
               }
            break;

         case GOSUB:                   /* login to dir under file ptr */
            if (dir_display) {
               dir_login();
               menu_init();
            } else {
               if (!restricted)
                  if (files[cw.curidx].flags & DIR) {
                     switch_dir(dp = dirplus(&files[cw.curidx],
                                             files[cw.curidx].name));
                     free(dp);
                     menu_init();
                  } else
                     beep();
            }
            break;

         case NEXTT:                   /* goto next tagged file */
         case PREVT:                   /* goto prev tagged file */
            if (!restricted)
               tag_find(ch == NEXTT ? 1 : -1);
            break;

         case OPENW:                   /* open another file display window */
            if (!restricted)
               win_open();
            break;

         case CLOSEW:                  /* close current file display window */
            if (!restricted)
               win_close();
            break;

         case NEXTW:                   /* goto next file display window */
            if (!restricted)
               win_next();
            break;

         case PREVW:                   /* goto prev file display window */
            if (!restricted)
               win_prev();
            break;

         default:                      /* see if the 1st letter of selection */

            if (!menu_do_char(ch))     /* do menu selection starting with ch */
               beep();                 /* beep if not a valid selection */
            break;

      }

      if (winupdate) {                 /* some commands ask for one or */
         refresh_screen(1);            /* more windows to be updated */
         update_header();              /* make sure header is current */
         winupdate = 0;
      }
   }
}


/******************************************************************************
 **                             Q U I T                                      **
 *****************************************************************************/

quit() {               /* exit from overview */

   int ch;

   /* Ask the user if he really wants to exit (default = N).  If so, check
      to see if we are still in the initial startup dir.  If so, just exit.
      If not, ask if we should return to the original dir before
      quitting (default = return). */

   ch = ask("Quit OVERVIEW and return to DOS? (y/N): ");

   if (yes(ch)) {

      getcwd(cw.dirbuf,MAX_PATHLEN);   /* can mess cw.dibuf up cause quitting */

      if (strcmp(initialdir,cw.dirbuf) != 0) {
         ch = ask("Exit to current directory, or Return to original? (e/R): ");
         if (toupper(ch) != 'E')
            change_dir(initialdir);
      }
      reset_tty();                     /* restore pre OV tty settings */
      exit();                          /* So long, thanks for all the fish */
   }
}
