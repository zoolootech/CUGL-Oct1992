/*  009  14-Feb-86  ovhelp.c

        Copyright (c) 1986 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include <setjmp.h>
#include "ov.h"

#define H_bar (0xcd)

#define HLINESIZ 80    /* max size of a help line - coordinate with cvthlp.c */
#define NSELS 20       /* max # menu ents/topic   - coordinate with cvthlp.c */
#define STKSIZ 10      /* depth of prev screen stack */

static FILE *hlp;
static char *helpfile = "ov.hlp";
static char *nohelp = " - no help available!";

int help_exit(), help_back(), help_sel();

extern MENU *top_menu, top_help_menu[];

static MENU pmenu =
   { "Prev", "Return to the previous help screen", help_back, top_help_menu };
static MENU qmenu = { "Quit", "Quit help", help_exit, top_help_menu };

static int stkidx;
static char *buffer;
static MENU top_help_menu[NSELS];
static unsigned char save_restricted;
static long lastsel, offsets[NSELS], offstack[STKSIZ];

extern char *cantopen;
extern MENU_STATE curmenu;
extern jmp_buf back_to_main;
extern unsigned char restricted;
extern unsigned char help_display;

FILE *pathopen();
char *strchr(), *getenv(), *hgets();


/******************************************************************************
                                   H E L P
 *****************************************************************************/

help() {               /* provide help on using OverView */

   register int ch;

   if (help_display)                   /* don't work recurrsively */
      return;

   help_init();                        /* locate/open help file */

   save_restricted = restricted;       /* save current restricted flag and */
   menu_save();                        /*   menu state before changing them */

   restricted = TRUE;                  /* disable cmds we don't want */
   help_display = TRUE;                /* yes, help is active */
   top_menu = top_help_menu;           /* use (tbd) help menu */

   savescreen();                       /* save the current display screen */
   savecursor();                       /* save the current cursor state */
   hidecursor();                       /* force the cursor off */

   help_on(0L);                        /* provide help with top screen */

   disp_status();                      /* don't tell user to hit F1 for help */

   menu_init();                        /* initialize help menu */

   /* we do our own getchr() and menu selections here because we don't
      know what might have been going on before help was called - if
      we returned to the caller, things might get screwed up */

   while (help_display && (ch = getchr()) != EOF_CH) {

      switch(ch) {
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

         default:                      /* see if the 1st letter of selection */
            menu_do_char(ch);          /* do menu selection starting with ch */
            break;
      }
   }

   if (help_display)
      help_exit();                     /* in case user ^Z's out */
}


/****************************************************************************
                            H E L P _ I N I T
 ****************************************************************************/

static int
help_init() {          /* initialize the help subsystem */

   stkidx = 0;                  /* make sure stack of offsets is empty */
   lastsel = 0;                 /* there was no last help topic selection */

   /* try to open the default help file somewhere in the PATH -
      give up if helpfile doesn't open */

   if ((hlp = pathopen(helpfile,"rb")) == NULL)
      show_error(0,15,3,cantopen,helpfile,nohelp);

   /* try to allocate a large buffer to access the help file */

   if (buffer = (char *) malloc(4096))
      setvbuf(hlp,buffer,_IOFBF,4096);
}


/*****************************************************************************
                             H E L P _ E X I T
 *****************************************************************************/

static int
help_exit() {          /* exit from help subsystem */

   fclose(hlp);                /* close the help file */

   if (buffer)                 /* release buffer if used */
      free(buffer);

   menu_restore();                     /* restore prior menu */
   help_display = FALSE;               /* help is signing off for now */
   restricted = save_restricted;       /* restore prior restricted flag */

   purge_menu();               /* free strings used by menu */

   restorescreen();            /* restore the prior display screen */
   restorecursor();            /* restore the previous cursor state */
}


/****************************************************************************
                            H E L P _ O N
 ****************************************************************************/

static int
help_on(offset)         /* provide help on selected topic */
long offset;
{
   char *title, hline[HLINESIZ+1];

   *hline = 'X';                               /* error flag */

   if (fseek(hlp,offset,SEEK_SET) == 0)        /* seek to the help frame */
      hgets(hline);                            /* get the header line */

   if (*hline != '#') {                        /* header lines start with # */
      show_error(0,0,2,"Error reading help file",nohelp);
      help_exit();
      longjmp(16,back_to_main);        /* should never happen, but... */
   }

   /* display the topic title at top of screen */

   title = strchr(hline,' ');

   center_text(FIRST_HROW-1,title ? title+1 : hline+1);

   help_menu(hline,offset == 0);       /* create menu */

   help_text(hline);                   /* display text */
}


/****************************************************************************
                           H E L P _ T E X T
 ****************************************************************************/

static int
help_text(buf)         /* display help text */
char *buf;
{
   register int i, ntext;

   ntext = fgetc(hlp);                 /* read # text lines */

   for (i = 0; i < HELP_ROWS; i++) {

      gotorc(i+FIRST_HROW,0);

      if (ntext)
         if (hgets(buf)) {
            disp_str(buf);
            ntext--;
         } else
            ntext = 0;

      clr_eol();
   }
}


/****************************************************************************
                           H E L P _ M E N U
 ****************************************************************************/

static int
help_menu(buf,top)     /* build a menu_selection structure for selection */
char *buf;
int top;
{
   int i;
   char *bp;
   register MENU *mp;
   register int nmenu;

   purge_menu();                       /* clear out last menu */

   mp = top_help_menu;

   /* process each menu entry in the help file */

   for (nmenu = fgetc(hlp), i = 0; nmenu > 0; nmenu--, mp++) {

      fread(&offsets[i++],sizeof(long),1,hlp); /* read the ent's offset */

      hgets(buf);                              /* read keyword/prompt */

      /* build menu selection for this entry */

     if (bp = strchr(buf,' ')) {
         mp->choice = Strndup(buf,bp-buf);
         mp->prompt = Strdup(bp+1);
         mp->func = help_sel;
         mp->sub_menu = top_help_menu;
      } else {
        i--;                                   /* shouldn't happen */
        mp--;
      }
   }

   /* assign the two constant entries and finish it */

   if (!top)                           /* don't include Prev entry if */
      *mp++ = pmenu;                   /*   if this is the top help menu */
   *mp++ = qmenu;

   mp->choice = NULL;                  /* terminate menu structure */
}


/****************************************************************************
                        P U R G E _ M E N U
 ****************************************************************************/

static int
purge_menu() {         /* release the strings used by current help menu */

   register int i;
   register MENU *mp;

   for (i = 0, mp = top_help_menu; i < NSELS; i++, mp++) {

      if (mp->choice && mp->choice != pmenu.choice && mp->choice != qmenu.choice) {
         free(mp->choice);
         free(mp->prompt);
      }

      mp->choice = NULL;
   }
}


/*****************************************************************************
                            H E L P _ B A C K
 *****************************************************************************/

static int
help_back() {          /* backup to previous help display */

   if (stkidx)                 /* anything on stack? */
      help_on(lastsel = offstack[--stkidx]);
}


/*****************************************************************************
                            H E L P _ S E L
 *****************************************************************************/

static int
help_sel() {           /* select a help topic */

   /* put last selection on stack */

   if (stkidx < STKSIZ)
      offstack[stkidx++] = lastsel;

   /* give user help on whatever topic is desired */

   help_on(lastsel = offsets[curmenu.current_selection]);
}


/*****************************************************************************
                              H G E T S
 *****************************************************************************/

static char *
hgets(buf)             /* get a line from help file the way we want it */
register char *buf;
{
   if (fgets(buf,HLINESIZ,hlp)) {
      buf[strlen(buf)-1] = '\0';       /* we don't need no stinking \n's */
      return(buf);
   }

   return(NULL);                       /* couldn't get a line */
}
