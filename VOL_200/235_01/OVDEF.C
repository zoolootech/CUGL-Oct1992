/*  010  14-Feb-87  ovdef.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include "ov.h"

#ifndef NULL
#define NULL (0)
#endif

#define FIRST_AROW (3)         /* attribute display */
#define FIRST_ACOL (30)
#define FIRST_TROW (3)         /* text types */
#define FIRST_TCOL (2)
#define FIRST_IROW (14)        /* instructions/directions */
#define FIRST_ICOL (2)

#define V_bar (0xba)
#define H_bar (0xcd)

extern unsigned char vid_mode;         /* need to know video mode */
extern unsigned char vid_snow;         /* NZ when checking for snow */
extern unsigned char vid_attrib;       /* we hack directly with video attrib */
extern unsigned char def_display;      /* NZ when define screen displayed */
extern unsigned char attribs[], cgaattr[], monattr[]; /* arrays of attrib's */

extern char *ovname;                   /* might be name of OV file */
extern char *cantopen;                 /* common text message */
extern char patchstr[];                /* id's patch area in EXE file */

extern struct menu_selection top_file_menu[], *top_menu;
extern char *quit_txt;

int def_color(), def_snow(), def_exit();
int def_set(), def_noset(), def_reset(), def_write();

static struct menu_selection set_attr_menu[] = {
   { "Set", "Set the video attributes to the values shown", def_set, NULL },
   { "Reset", "Reset the video attributes to the default values", def_reset, NULL },
   { "Quit", "Quit without changing the video attributes", def_noset, NULL },
   { NULL, NULL, NULL, NULL }
};

static struct menu_selection top_def_menu[] = {
   { "Colors", "Select video display attributes", def_color, set_attr_menu },
   { "Snow", "Enable or disable video \"snow\" checking", def_snow, NULL },
   { "Write", "Make current settings permanent by writing to disk", def_write, NULL },
   { "Quit", "Return to the file display", def_exit, top_file_menu },
   { NULL, NULL, NULL, NULL }
};

static struct {                /* define stuff to display the type of */
   char *name;                 /*   parameter to be defined - this table */
   unsigned char row;          /*   must be in DIS_NORM - DIS_TAGD order */
   unsigned char col;
} def_text[] = {
   { "NORMAL TEXT            ", FIRST_TROW,   FIRST_TROW },
   { "HIGHLIGHTED TEXT       ", FIRST_TROW+1, FIRST_TROW },
   { "WINDOW TEXT            ", FIRST_TROW+2, FIRST_TROW },
   { "HIGHLIGHTED WINDOW TEXT", FIRST_TROW+3, FIRST_TROW },
   { "HEADING TEXT           ", FIRST_TROW+4, FIRST_TROW },
   { "BACKGROUND TEXT        ", FIRST_TROW+5, FIRST_TROW },
   { "TAGGED FILE NAME TEXT  ", FIRST_TROW+6, FIRST_TROW } };

static char *directions[] = {
     "PgUp & PgDn select the ", "type of text to be set.",
     "                       ", "\x18\x19\x1b\x1a select the colors." };

static int text_type;
static unsigned char newattr[7];
static unsigned char defining = FALSE; /* NZ - video attributes being def'd */

static char *ovfile = "ov.exe";
static char *nowrit = " - current settings not written";

char *getenv(), *strchr();
FILE *fopen(), *pathopen();


/*****************************************************************************
                               D E F I N E
 *****************************************************************************/

define() {             /* allow user to define screen colors / snow checking */

   def_display = TRUE;         /* define screen display is active */

   strncpy(newattr,attribs,sizeof(newattr));   /* start with current attribs */

   def_init();                 /* actually setup the display */

   top_menu = top_def_menu;    /* make define menu THE menu */
}


/*****************************************************************************
                            D E F _ E X I T
 *****************************************************************************/

static int
def_exit() {           /* leave define screen parameters mode */

   def_display = FALSE;                /* going back to normal */

   top_menu = top_file_menu;           /* file menu again */

   setup_file_scr();
   update_header();                    /* rewrite the entirey screen */
   refresh_screen(0);                  /* to use possible new colors */
}


/*****************************************************************************
                            D E F _ I N I T
 *****************************************************************************/

static int
def_init() {           /* initialize the define screen */

   int i, fg;
   register int bg;

   /* initialize the screen image */

   setvattrib(DIS_NORM);

   clr_scr();
   center_text(0,"DEFINE SCREEN PARAMETERS");

   disp_status();                              /* status line */

   setvattrib(DIS_BOX);                        /* box to enclose text types */
   popup(FIRST_TROW-1,FIRST_TCOL-1,9,27,NULL);

   for (i = DIS_NORM; i <= DIS_TAGD; i++)      /* display text types */
      select_t(0,i);

   setvattrib(DIS_BOX);                        /* box to enclose pattern */
   popup(FIRST_AROW-1,FIRST_ACOL-1,18,50,NULL);

   for (fg = 0; fg < 16; fg++) {               /* display color pattern */
      gotorc(FIRST_AROW+fg,FIRST_ACOL);
      for (bg = 0; bg < 8; bg++) {
         vid_attrib = (bg << 4) | fg;
         disp_str(" TEXT ");
      }
   }

   setvattrib(DIS_NORM);       /* back to "Normal" */

}


/*****************************************************************************
                            D E F _ C O L O R
 *****************************************************************************/

static int
def_color() {          /* define screen "colors" */

   int i;

   defining = TRUE;                    /* yes, we are defining attributes */
   select_t(1,text_type = DIS_NORM);   /* highlight normal text first */
   select_a(1,newattr[text_type]);     /* highlight current attr location */
   top_menu = set_attr_menu;           /* so ESC doesn't leave this menu */

   setvattrib(DIS_BOX);               /* box for directions/instructions */
   popup(FIRST_IROW-1,FIRST_ICOL-1,6,27,NULL);
   for(i = 0; i < 4; i++)
      disp_str_at(directions[i],FIRST_IROW+i,FIRST_ICOL+1);
   setvattrib(DIS_NORM);
}


/*****************************************************************************
                            D E F _ x S E T
 *****************************************************************************/

static int
def_set() {            /* use the currently displayed video attributes */

   strncpy(attribs,newattr,sizeof(newattr));   /* SET the current values */
   do_set();                                   /* rest of stuff */
}

static int
def_noset() {          /* back to the prior values */

   strncpy(newattr,attribs,sizeof(newattr));   /* back to current */
   do_set();
}

static int
def_reset() {          /* reset to overview default */

   strncpy(attribs,vid_mode == 7 ? monattr : cgaattr,sizeof(newattr));
   strncpy(newattr,attribs,sizeof(newattr));   /* back to default */
   do_set();
}

static int
do_set() {
   defining = FALSE;                           /* not defining anymore */
   def_init();                                 /* redisplay scr with new vals */
   top_menu = top_def_menu;                    /* back to def menu */
}


/*****************************************************************************
                            D E F _ S N O W
 *****************************************************************************/

static int
def_snow() {

   int ch;
   char msg[80];
   static char *enable = "ENABLE", *disable = "DISABLE";

   strcpy(msg,"\"snow\" checking is currently ");
   strcat(msg,vid_snow ? enable : disable);
   strcat(msg,"D, do you want to ");
   strcat(msg,vid_snow ? disable : enable);
   strcat(msg," it? (y/N): ");

   ch = ask(msg);                      /* ask user what to do */
   if (yes(ch))
      vid_snow = vid_snow ? 0 : 1;

}


/*****************************************************************************
                            D E F _ W R I T E
 *****************************************************************************/

static int
def_write() {

   int i;
   register int ch;
   char *buffer, *sp;
   register FILE *exe;

   /* try to open the EXE file somewhere in the PATH - if this is
      DOS 3.x or greater, ovname should already have the full pathname
      of the .EXE file. */

   if ((exe = pathopen(ovname ? ovname : ovfile,"r+b")) == NULL) {
      sp = prompt("","Can't locate OV.EXE, enter filename to write to:",
           NULL,0,MAX_PATHLEN);
      if (strlen(sp))
         exe = fopen(sp,"r+b");
   }

   if (exe == NULL) {          /* now give up if not open */
      show_error(0,0,3,cantopen,"OV.EXE",nowrit);
      return;
   }

   /* try to allocate a large buffer to access the EXE file */

   if (buffer = (char *) malloc(4096))
      setvbuf(exe,buffer,_IOFBF,4096);

   /******************* EXE file is open *********************/

   /* scan the file looking for the patch string id */

   while ((ch = fgetc(exe)) != EOF)            /* slow and stupid, but */
      if (ch == *patchstr) {
         i = 0;
         while ((ch = fgetc(exe)) && ch == patchstr[++i])
            ;
         if (patchstr[++i] == '\0')
            break;
      }

   if (ch != EOF) {            /* found it if not EOF */

      strncpy(vid_mode == 7 ? monattr : cgaattr,attribs,sizeof(newattr));
      fseek(exe,0L,SEEK_CUR);          /* seek so we can write */
      fwrite(&vid_snow,sizeof(vid_snow)+sizeof(newattr)+sizeof(newattr),1,exe);

   } else
      show_error(0,0,2,"Can't find parameters in file",nowrit);

   fclose(exe);                /* close */

   if (buffer)                 /*   and clean up */
      free(buffer);
}


/*****************************************************************************
                            D E F _ M O V E
 *****************************************************************************/

def_move(dir)          /* move the attribute pointer in given direction */
int dir;
{
   register unsigned int fbg, attr = newattr[text_type];

   if (!defining)                      /* don't do anything unless video */
      return;                          /*   attributes are being defined */

   select_a(0,attr);                   /* always deselect video attribute */

   if (dir == PGUP || dir ==PGDN)      /* maybe deselect text type */
      select_t(0,text_type);

   switch (dir) {

      case UP:                                 /* up arrow of course */

         if (fbg = attr & 15)
            attr = (attr & 0x70) | (fbg - 1);
         else
            attr = (attr & 0x70) | 15;         /* wraps to bottom */
         break;

      case DOWN:                               /* down arrow */

         if ((fbg = attr & 15) < 15)
            attr = (attr & 0x70) | (fbg + 1);
         else
            attr = attr & 0x70;                /* wraps to top */
         break;

      case LEFT:                               /* left arrow */

         if (fbg = (attr >> 4) & 7)
            attr = ((fbg - 1) << 4) | (attr & 15);
         else
            attr = 0x70 | (attr & 15);         /* wraps to right */
         break;

      case RIGHT:                              /* right arrow */

         if ((fbg = (attr >> 4) & 7) < 7)
            attr = ((fbg + 1) << 4) | (attr & 15);
         else
            attr = attr & 15;                  /* wraps to left */
         break;

      case PGUP:                               /* page up - new text type */

         if (text_type)
            --text_type;
         else
            text_type = 6;
         break;

      case PGDN:                               /* page dn - new text type */

         if (text_type < 6)
            ++text_type;
         else
            text_type = 0;
         break;
   }

   if (dir == PGUP || dir ==PGDN)              /* maybe select new text type */
      attr = newattr[text_type];               /* set attribute for new type */

   select_a(1,attr);                   /* show new attribute */
   newattr[text_type] = attr;          /* remember what it is */

   select_t(1,text_type);              /* display name in new attr */

   setvattrib(DIS_NORM);               /* stick with norm for now */
}


/*****************************************************************************
                            S E L E C T _ A
 *****************************************************************************/

static int
select_a(on_off,attr)  /* highlight the current attribute */
register int on_off, attr;
{
   int row, col;

   row = FIRST_AROW + (attr & 15);     /* decode attr to row/col */
   col = FIRST_ACOL + (attr >> 4) * 6;

   vid_attrib = attr;                  /* gotta use this attribute */

   disp_str_at(on_off ? "\x10TEXT\x11" : " TEXT ",row,col); /* (de)select */
}


/*****************************************************************************
                            S E L E C T _ T
 *****************************************************************************/

static int
select_t(on_off,type)  /* highlight the current text type */
register int on_off, type;
{
   vid_attrib = newattr[type];         /* gotta use this attribute */

   /* display pointer, name, pointer */

   disp_char_at(on_off ? 0x10 : ' ',FIRST_TROW+type,FIRST_TCOL);
   disp_str(def_text[type].name);
   disp_char(on_off ? 0x11 : ' ');
}
