/*  028  11-Jan-87  ovview.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ov.h"

#define H_bar (0xcd)
#define V_bar (179)

#define Vnextch(fh) ((curp < endp) ? *curp++ : vnextch(fh))  /* speed up */
#define Vprevch(fh) ((curp > bufp) ? *--curp : vprevch(fh))  /* buffer access */

extern unsigned char far *curp;                /* current char position */
extern unsigned char far *bufp, far *endp;     /* buffer begin/end pointers */

static int (*mark_func)();             /* pointer to marker function */
static int nlines;                     /* # lines displayed on screen */
static unsigned char ascmode;          /* true means ascii mode display */
static unsigned char bitmask;          /* for 7 or 8 bit display mode */
static int inf;                        /* handle for file being viewed */
static int margin;                     /* margin for right/left scrolling */
static long tos;                       /* top of screen offset in file */
static long markers[5];                /* marker positions  */

/* function delcarations created by -Zg option - ALTCALL added
   NOTE: anything called by menu routine cannot be ALTCALL */

/*global*/  int view(void);
/*global*/  int view_move(int );
static int view_exit(void), view_down(void), view_up(void), view_next(void);
static int view_prev(void), view_tof(void), view_eof(void);
static int view_right(void), view_left(void), view_set(void), view_goto(void);
static int do_mark(void), setmark(void), gomark(void), ALTCALL view_line(void);
static int ALTCALL fmt_asc_line(char *,int *), ALTCALL fmt_hex_line(char *,int *);
static int view_7bit(void), view_8bit(void), view_asc(void), view_hex(void);
static int ALTCALL align(void);
static int ALTCALL backup(int );
static int ALTCALL peol(void);
static int ALTCALL nsol(void);
static int ALTCALL more2view(void);

static struct key_ent {        /* table mapping movement keys to function */
   int key;
   int (*func)();
} key2func[] = { { DOWN, view_next }, { UP, view_prev }, { PGDN, view_down },
   { PGUP, view_up }, { HOME, view_tof }, { END, view_eof },
   { RIGHT, view_right }, { LEFT, view_left } };

#define NMOVKEYS (8)

extern MENU_STATE curmenu;
extern MENU top_file_menu[], *top_menu;

static char setgo[] = "Set/Goto marker";

static MENU mark_menu[] = {
   { "1", setgo, do_mark, NULL },
   { "2", setgo, do_mark, NULL },
   { "3", setgo, do_mark, NULL },
   { "4", setgo, do_mark, NULL },
   { "5", setgo, do_mark, NULL },
   { NULL, NULL, NULL, NULL }
};


MENU top_view_menu[] = {
   { "Dwn", "Page down in the file", view_down, NULL },
   { "Up", "Page up in the file", view_up, NULL },
   { "Nxt", "Advance one line", view_next, NULL },
   { "Prv", "Backup one line", view_prev, NULL },
   { "TOF", "Goto Top Of File", view_tof, NULL },
   { "EOF", "Goto End Of File", view_eof, NULL },
   { "Rght", "Scroll right 8 characters", view_right, NULL },
   { "Left", "Scroll left 8 characters", view_left, NULL },
   { "Set", "Set marker at current position", view_set, mark_menu },
   { "Goto", "Goto set marker position", view_goto, mark_menu },
   { "Ascii", "View file as ASCII characters", view_asc, NULL },
   { "Hex", "View file in hexadecimal", view_hex, NULL },
   { "7bit", "View low 7 bits of each character", view_7bit, NULL },
   { "8bit", "View all 8 bits of each character", view_8bit, NULL },
   { "Quit", "Return to file display", view_exit, top_file_menu },
   { NULL, NULL, NULL, NULL }
};

extern WINDOW cw;
extern char *cantopen;
extern FILE_ENT files[];
extern unsigned char view_display, restricted;

int ALTCALL vbuf_init(int );           /* declarations for buffer routines */
int ALTCALL vbuf_free(void);
unsigned long ALTCALL vtell(void);
int ALTCALL vseek(int ,long ), ALTCALL vnextch(int ), ALTCALL vprevch(int );

#define view_seek(off) vseek(inf,off)


/******************************************************************************
 **                             V I E W                                      **
 *****************************************************************************/

view() {               /* view the current file at the terminal */

   int i;
   register char *fn;
   register FILE_ENT *fp;

   /* don't try to view file if its empty */

   fp = &files[cw.curidx];             /* a couple of quick pointers */

   if (fp->size == 0)
      show_error(0,8,1,"This file is empty!");

   bitmask = 0xff;             /* defaults: 8 bit, ascii, left edge */
   ascmode = TRUE;
   margin = 0;

   /* open the file to be viewed, error out if can't open */

   if ((inf = open((fn = fname(fp)),O_RDONLY|O_BINARY)) == -1) {
      free(fn);
      show_error(1,8,3,cantopen,fp->name,": ");
   }

   savescreen();               /* save current display image */
   restricted = TRUE;          /* disable special file commands */
   view_display = TRUE;        /* yes, we are viewing */

   for (i = 0; i < 5; i++)     /* set markers to TOF */
      markers[i] = 0L;

   vbuf_init(inf);             /* initialize view buffer system */

   center_text(FIRST_VROW-1,fn); /* show file name */
   free(fn);

   view_down();                /* display the first screen of file data */

   top_menu = top_view_menu;   /* setup the view menu as the main menu */
}


/******************************************************************************
 **                        V I E W _ E X I T                                 **
 *****************************************************************************/

static int
view_exit() {          /* exit the view display, return to file display */

   close(inf);                 /* close file */
   vbuf_free();                /* release memory */
   top_menu = top_file_menu;   /* restore file menu as main */

   restricted = FALSE;         /* all commands are enabled */
   view_display = FALSE;       /* not viewing */

   restorescreen();            /* redisplay prior screen image */
}


/******************************************************************************
 **                        V I E W _ D O W N                                 **
 *****************************************************************************/

static int
view_down() {          /* page down into the view buffer/file */

   register int i;

   if (!more2view())           /* nothing to do if no more data to view */
      return;

   nlines = 0;                 /* no lines displayed yet */
   tos = vtell();              /* remember where top of screen is */

   /* display up to a screen full of file data, clear the screen as we go */

   for (i = 0; i < VIEW_ROWS; i++) {
      gotorc(i+FIRST_VROW,0);          /* position to line */
      if (view_line())                 /* display a single line */
         nlines++;                     /* count lines displayed */
   }
}


/******************************************************************************
 **                        V I E W _ U P                                     **
 *****************************************************************************/

static int
view_up() {            /* page up into the view buffer */

   long curoff;

   /* back two screen's worth, or to the top line in memory and use
      view_down() to display the screen - only display if we really
      backed up (might be at TOF) */

   curoff = vtell();                   /* where we were */
   view_seek(tos);                     /* quickly to top of screen */
   if (backup(VIEW_ROWS))              /* try to backup another screen */
      view_down();                     /* display prior screen if backed up */
   else
      view_seek(curoff);               /* must be tof, back to where we were */
}


/******************************************************************************
 **                        V I E W _ N E X T                                 **
 *****************************************************************************/

static int
view_next() {          /* display the next line in the buffer/file */

   long curoff;

   if (!more2view())           /* nothing to do if no more data to view */
      return;

   /* set the new top of screen location */

   if (ascmode) {                      /* ascii mode display? */
      curoff = vtell();                /* save where we are */
      view_seek(tos);                  /* to top of screen  */
      nsol();                          /* to start of next line */
      tos = vtell();                   /* its the new top of screen */
      view_seek(curoff);               /* back to bottom of screen */

   } else                              /* hex mode */
     tos += 16;                        /* tos is just the next hex line */

   /* scroll the screen up one line to make room for the new line at bottom */

   delete_line(FIRST_VROW,VIEW_ROWS-1);

   gotorc(FIRST_VROW+VIEW_ROWS-1,0);   /* cursor to the last display line */

   view_line();                        /* display the line */
}


/******************************************************************************
 **                        V I E W _ P R E V                                 **
 *****************************************************************************/

static int
view_prev() {          /* display the previous line in the buffer */

   long curoff;

   curoff = vtell();                   /* where we are now */
   view_seek(tos);                     /* old top of screen */
   if (backup(1)) {                    /* can we backup another line */
      tos = vtell();                           /* if so, its new tos */
      insert_line(FIRST_VROW+1,VIEW_ROWS-1);   /* insert a blank line */
      gotorc(FIRST_VROW,0);                    /* cursor to first line */
      view_line();                             /* display the line */
      view_seek(curoff);                       /* back to old bottom */
      if (nlines < VIEW_ROWS)                  /* is a full screen displayed? */
         nlines++;                             /* no, one more displyed now */
      else
         backup(1);                            /* yes, back one line */

   } else                      /* couldn't backup, must be tof */
      view_seek(curoff);
}


/******************************************************************************
 **                        V I E W _ T O F                                   **
 *****************************************************************************/

static int
view_tof() {           /* backup and display the top of the file */

   view_seek(0L);              /* just go to top of file */
   view_down();                /* and display a screen */
}


/******************************************************************************
 **                        V I E W _ E O F                                   **
 *****************************************************************************/

static int
view_eof() {           /* display the end of the file */

   view_seek(files[cw.curidx].size);       /* seek to eof */
   backup(VIEW_ROWS);                      /* backup a screen full */
   view_down();                            /* display a screen full */
}


/******************************************************************************
 **                        V I E W _ M O V E                                 **
 *****************************************************************************/

view_move(mov_cmd)     /* move around the view buffer/file in response to */
int mov_cmd;           /* single key commands when viewing a file */
{

   register int i;

   /* basically, we just map the special keys to the same functions
      performed by the menu options */

   for (i = 0; i < NMOVKEYS; i++)
      if (mov_cmd == key2func[i].key) {
         (*key2func[i].func)();
         break;
      }
}


/*****************************************************************************
                 V I E W _ S E T / G O T O / M A R K
 *****************************************************************************/

static int
view_set() {           /* prepare to set a marker to current position */

   mark_func = setmark;        /* use the setmark routine later */
}

static int
view_goto() {          /* prepare to goto a marker position */

   mark_func = gomark;         /* use the gomark routine later */
}

static int
do_mark() {            /* set or goto a marker position */

   (*mark_func)();
}

static int
setmark() {            /* set a marker position */

   markers[curmenu.current_selection] = tos;
}

static int
gomark() {             /* goto a marker position */

   tos = markers[curmenu.current_selection];   /* where tos should be */
   align();                                    /* start at begining of line */
   view_down();                                /* display screen full */
}


/*****************************************************************************
                           V I E W _ L I N E
 *****************************************************************************/

static int ALTCALL
view_line() {          /* display a single line of the file */

   int col, ch;
   char line[SCREEN_COLS+1];

   if (ascmode)                /* format the line as ascii or hex data */
      ch = fmt_asc_line(line,&col);
   else
      ch = fmt_hex_line(line,&col);

   disp_str(line);                     /* make one call to display line */

   if (col < margin+SCREEN_COLS-1)     /* clear if < full line displayed */
      clr_eol();

   return(ch != EOF || col);   /* return true if something displayed */
}


/*****************************************************************************
                         F M T _ A S C _ L I N E
 ****************************************************************************/

static int ALTCALL
fmt_asc_line(op,colp)  /* format an ascii line for display */
char *op;
int *colp;
{
   register int ch, col = 0;
   int i, endcol = margin + SCREEN_COLS;

   while ((ch = Vnextch(inf)) != EOF && (ch = ch & bitmask) != '\n')
      if (ch == '\t') {
         for (i = 8 - (col & 7); i; i--, col++)
            if (col >= margin && col < endcol)
               *op++ = ' ';
       } else
          if (ch != '\r' && ch != '\0') {
              if (col >= margin && col < endcol)
                  *op++ = ch;
             col++;
          }

   *op = '\0';                 /* null terminate it for disp_str */

   *colp = col;                /* tell caller the # cloumns */
   return(ch);                 /* and if EOF was reached */
}


/******************************************************************************
                          F M T _ H E X _L I N E
 *****************************************************************************/

static int ALTCALL
fmt_hex_line(op,colp)  /* format a hex line for later display */
register char *op;
int *colp;
{
   int ch, ach, j;
   register int i;
   char offstr[9], *ap;
   static char bin2hex[] = "0123456789ABCDEF";

   /* test if there is anything to format */

   if (vnextch(inf) == EOF) {
      *colp = 0;
      *op = '\0';
      return(EOF);
   } else
      vprevch(inf);

   /* format data offset into buffer */

   ultoa(vtell(),offstr,16);           /* offset to hex */
   j = strlen(offstr);
   for (i = 6 - j; i > 0; i--)         /* zero fill */
      *op++ = '0';
   if (j <= 6)                         /* don't use more than 6 digits */
     strcpy(op,offstr);
   else
     strcpy(op,offstr+j-6);

   strupr(op);                         /* ultoa leaves A-F in lower case */

   strcat(op,"  ");                    /* go beyond the offset */
   op += strlen(op);

   ap = op + 53;                       /* where ascii data starts */
   *ap++ = V_bar;                      /* might as well do it now */

   /* setup data in hex (and ascii) */

   for (i = 16; i; i--) {              /* at most 16 bytes to fmt */

      ch = Vnextch(inf);               /* next char from file */
      if (ch == EOF)                   /* done? */
         break;

      *op++ = bin2hex[(ch >> 4) & 0x0f];       /* hexalate it */
      *op++ = bin2hex[ch & 0x0f];
      *op++ = ' ';

      if (((i+3) & 3) == 0)            /* extra spacer every 4 bytes */
         *op++ = ' ';

      if ((ach = ch & bitmask) < ' ')  /* do the ascii char, '.' if ctrl ch */
         *ap++ = '.';
      else
         *ap++ = ach;
   }

   /* blank fill if EOF was reached */

   for ( ; i; i--) {           /* i is # chars to blank fill */
      strncpy(op,"   ",3);
      op += 3;
      if (((i+3) & 3) == 0)
         *op++ = ' ';
      *ap++ = ' ';
   }

   *op++ = ' ';                /* up to ascii data */

   *ap++ = V_bar;              /* closing bar at end */
   *ap = '\0';                 /* The Terminator */

   *colp = 74;                 /* tell call how many columns */
   return(ch);                 /* and if EOF was reached */
}


/*****************************************************************************
                      V I E W _ R I G H T / L E F T
 *****************************************************************************/

static int
view_right() {         /* scroll right 8 characters */

   margin += 8;
   view_current();
   disp_margin();
}

static int
view_left() {          /* scroll left 8 characters */

   if (margin >= 8) {
      margin -= 8;
      view_current();
      disp_margin();
   }
}

static int
disp_margin() {        /* display the viewing margins */

   char marstr[11];

   if (margin) {

      itoa(margin,marstr,10);
      disp_msg(2,"COL: ",marstr);

   } else              /* must have just gone to 0 */

      clr_msg();
}


/*****************************************************************************
                            V I E W _ n B I T
 *****************************************************************************/

static int
view_7bit() {          /* display data using low order 7 bits */

   if (bitmask != 0x7f) {
      bitmask = 0x7f;
      view_current();
   }
}

static int
view_8bit() {          /* display data using all 8 bits per char */

   if (bitmask != 0xff) {
      bitmask = 0xff;
      view_current();
   }
}

static int
view_current() {       /* redisplay the current screen */

      view_seek(tos);
      view_down();
}


/*****************************************************************************
                           V I E W _ A S C
 *****************************************************************************/

static int
view_asc() {           /* set ascii mode display */

   if (!ascmode) {                    /* only need to change if in hex mode */
      ascmode = TRUE;                 /* set ascii mode */
      align();                        /* make sure were at begining of line */
      view_down();                       /* redisplay in ascii format */
   }
}


/*****************************************************************************
                          V I E W _ H E X
 *****************************************************************************/

static int
view_hex() {           /* set hex mode display */

   if (ascmode) {                     /* only need to change if in ascii mode */
      ascmode = FALSE;                /* set hex mode */
      align();                        /* make sure were at begining of line */
      view_down();                    /* redisplay in ascii format */
   }
}


/*****************************************************************************
                                A L I G N
 *****************************************************************************/

static int ALTCALL
align() {              /* align tos to be at the start of a line */

   if (ascmode) {              /* ascii mode? */

      view_seek(tos);                 /* backup to current top of screen */
      if (backup(1))                  /* make sure were at the start of */
         nsol();                      /*   the current line */

   } else {                    /* hex mode */

      tos &= ~((long) 0x0f);          /* force a paragraph boundry */
      view_seek(tos);                 /* backup to current top of screen */
   }
}

/*****************************************************************************
                                B A C K U P
 *****************************************************************************/

static int ALTCALL
backup(todo)           /* backup todo lines from current position */
int todo;
{
   register int i;
   int part, bytes;
   unsigned long off, lines;

    /* backup in hex mode */

    if (!ascmode) {                    /* different if in hex mode */
       off = vtell();                  /* current loc in file */
       lines = off >> 4;               /* # full hex display lines above */
       part = (bytes = off & (long) 0x0f) > 0; /* may be partial line if at eof */
       if (todo > lines + part) {              /* more todo than are? */
          view_seek(0L);                       /* just goto tof */
          return(lines + part);                /* went back this # lines */
       } else {
          view_seek(off - (((todo - part) << 4)+bytes)); /* backup todo lines */
          return(todo);
       }
    }

    /* backup in ascii mode */

    /* special case backing up from EOF - there may or may not be a \n
       at the end of the last line */

    if (vnextch(inf) == EOF) {         /* at end of file? */
       vprevch(inf);                   /*   always backup at least one char */
       i = 1;                          /*   we will backup 1 line here      */
       if (peol() == '\n')             /*   goto start of prev line         */
          vnextch(inf);
    } else {           /* not at EOF, setup for loop down below */
       i = 0;
       vprevch(inf);
    }

    /* backup todo ascii lines, 1 line may have been done above */

    for ( ; i < todo; i++) {
      if (peol() == EOF)               /* goto end of prev line */
         break;
      if (peol() == '\n')              /* end of prev prev line */
         vnextch(inf);                 /* start of prev line    */
   }

   return(i);          /* tell caller how many ascii lines backed up */
}

static int ALTCALL
peol() {       /* move to end of prev line */

   register int ch;

   while ((ch = Vprevch(inf)) != '\n' && ch != EOF) ;  /* end of prev line */
   return(ch);
}

static int ALTCALL
nsol() {       /* move to start of next line */

   register int ch;

   while ((ch = Vnextch(inf)) != '\n' && ch != EOF) ;  /* start of next line */
   return(ch);
}


/*****************************************************************************
                            M O R E 2 V I E W
 *****************************************************************************/

static int ALTCALL
more2view() {          /* return true if more data to view */

   return(vtell() < files[cw.curidx].size);
}
