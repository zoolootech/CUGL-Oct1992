/*  045  26-Feb-87  ovtty.c

        Copyright (c) 1986 by Blue Sky Software.  All rights reserved.
*/

#include <dos.h>
#include "ov.h"

#ifndef NULL
#define NULL (0)
#endif

static int vmode, vwidth, vpage;          /* initial video mode, width, page */
static int crow, ccol, cstscan, cendscan; /* initial cursor stuff */
static int save_row, save_col, save_stscan, save_endscan; /* save cursor */

char far *screen = NULL;               /* address of screen image */
char far *cursor = NULL;               /* cursor location on screen */
static char far *save_cursor;               /* loc to save cursor value */
static int imageidx = 0;                    /* index of next image[] to use */
static char far *image[2] = { NULL, NULL }; /* address of saved screen image */
static char far *savcursor[2];              /* saved cursor location */
char far *malloc_f();

unsigned char vid_attrib;              /* current video attribute to use */
unsigned char vid_mode;                /* video mode in use */
unsigned char attribs[7];              /* video attrib's in use */

/****************************************************************************
 Following is a patch area used, updated by ovdef.c.  Do not change anything
 in this area without also checking ovdef.c
 ****************************************************************************/

char patchstr[] = "VIDEO HERE";        /* identify vid_snow in .EXE file */
unsigned char vid_snow = 1;            /* NZ if must chk for video snow */

/* DIS_NORM, DIS_HIGH, DIS_BOX, DIS_HIBOX, DIS_HEAD, DIS_TEXT, DIS_TAGD */

unsigned char monattr[] = { 0x07, 0x70, 0x0F, 0x70, 0x70, 0x07, 0x0F };/*mono*/
unsigned char cgaattr[] = { 0x0B, 0x30, 0x0E, 0x30, 0x1B, 0x0A, 0x0E };/* cga*/

/*****************************************************************************
                            End of patch area
 *****************************************************************************/


/******************************************************************************
 **                 I N I T  /  R E S E T _ T T Y                            **
 *****************************************************************************/

init_tty() {           /* initialize the display (terminal?) */

   vmode = getvideomode(&vwidth,&vpage);   /* save current video state */

   /* initialize the display adapter, currently only modes 3 and 7
      are supported.  Set the mode (which also clears the screen),
      and if a CGA, make sure display page 0 is active */

   if (vmode < 7) {                            /* initialize a CGA */
      setvideomode(vid_mode = 3);              /* color, 80x25 text */
      setdisplaypage(0);                       /* display page 0 */
      strncpy(attribs,cgaattr,sizeof(cgaattr));/* set up cga display attr's */

      FP_SEG(screen) = 0xB800;                 /* set screen base address */

   } else {                                    /* initialize a MONO adapter */
      setvideomode(vid_mode = 7);              /* clears the screen */
      strncpy(attribs,monattr,sizeof(monattr));/* set up mono display attr's */
      vid_snow = 0;                            /* no snow on mono */
      FP_SEG(screen) = 0xB000;                 /* set screen base address */
   }

   setvattrib(DIS_NORM);          /* set default video attribute */
   cursor = screen;               /* cursor is at top of screen */

   /* save data about the cursor position and size, we're going
      to turn off the cursor and we want to be able to restore
      it later */

   readcursor(vpage,&crow,&ccol,&cstscan,&cendscan);  /* save info */

   setcursorsize(32,0);           /* turn off hardware cursor */
}

reinit_tty() {         /* reinitialize tty from unknown state */

   int mode, dummy;

   mode = getvideomode(&dummy,&dummy); /* get current video mode */
   if (mode != vid_mode)               /* reset video mode if need be */
      setvideomode(vid_mode);
   if (vid_mode == 3)                  /* set display page 0 if cga */
      setdisplaypage(0);
   hidecursor();                       /* make sure cursor is off */
}


reset_tty() {         /* restore the display to pre-overview status */

   setvideomode(vmode);                   /* restore the video mode */

   setdisplaypage(vpage);                 /* restore display page */

   setcursorsize(cstscan,cendscan);       /* turn the curosr back on */
}


/******************************************************************************
 **                      S E T V A T T R I B                                 **
 *****************************************************************************/

int ALTCALL
setvattrib(i)          /* set the video attribute */
int i;
{
   vid_attrib = attribs[i];
}


/******************************************************************************
 **                       D I S P _ S T R _ A T                              **
 *****************************************************************************/

int ALTCALL
disp_str_at(s,r,c)     /* display a string at specified row, column */
char *s;
int r,c;
{
   gotorc(r,c);        /* goto display location */
   disp_str(s);        /* use our own routine */
}


/******************************************************************************
 **                    D I S P _ C H A R _ A T                               **
 *****************************************************************************/

int ALTCALL
disp_char_at(ch,r,c)   /* display a char at specified row, column */
int ch, r, c;
{
   gotorc(r,c);        /* goto display location */
   disp_char(ch);      /* use our other routine to display it */
}


/******************************************************************************
                         D I S P _ R E P _ A T
 *****************************************************************************/

int ALTCALL
disp_rep_at(ch,cnt,r,c)        /* display cnt ch's at specified row, column */
int ch, cnt, r, c;
{
   gotorc(r,c);        /* goto display location */
   disp_rep(ch,cnt);   /* use our other routine to display it */
}


/******************************************************************************
 **                     P U T C H   /  P U T S T R                           **
 *****************************************************************************/

putchr(ch) {                   /* these routines only exist because they */
   putch(ch);                  /* update the hardware cursor position */
}                              /* while doing output, a function that is */
                               /* useful when doing direct screen i/o, but */
putstr(s)                      /* some functions require the user to see */
register char *s;              /* a cursor.  The simulated cursor could */
{                              /* be made to handle this, but this is so */
   while (*s)                  /* much easier and the usage of these */
      putch(*s++);             /* functions is quite low */
}

putstr_nomove(s)               /* here's a strange one, it displays a string */
char *s;                       /*   without moving the hardware or software */
{                              /*   cursor */
   int row, col, x;
   char far *savec;

   savec = cursor;                     /* save software cursor location */
   readcursor(0,&row,&col,&x,&x);      /* get hardware cursor location */

   /* move software cursor to hardware location and display string */

   cursor = screen + (row * 160) + (col << 1);
   disp_str(s);                               /* doesn't move hardware cursor */

   cursor = savec;             /* restore software cursor */
}


/******************************************************************************
 **                          G E T C H R                                     **
 *****************************************************************************/

getchr() {             /* get a character from the terminal */

   register int ch;

   static unsigned char kbdmap[] = {
      /* 3B */     HELP, TAG, GOPAR, GOSUB, NEXTT, PREVT,
      /* 41 */     OPENW, CLOSEW, NEXTW, PREVW, 0, 0,
      /* 47 */     HOME, UP, PGUP, 0, LEFT, 0, RIGHT, 0, END,
      /* 50 */     DOWN, PGDN, INS, DEL };

   /* get a char from the user with no echo.  If its a normal (not
      extended char, return it as is */

readch:

   if (ch = getraw())          /* get a char from user with no echo */
      return(ch);              /* return it if it's a normal character */

   /*  must be an extended char, map it to an internal code if used */

   ch = getraw();              /* get scan code of char */

   if (ch >= 0x3b && ch <= 0x53) { /* one of our mapped codes? */
      ch = kbdmap[ch-0x3b];        /* yes, translate it */
      if (ch == HELP) {            /* HACK is this a cry for help? HACK */
         help();                   /* do the help subsystem */
         goto readch;              /* then read another char from user */
      }
   }

   return(ch);                 /* tell caller what was read */
}


/******************************************************************************
 **                          O U T _ X                                       **
 *****************************************************************************/

int ALTCALL
out_int(num,fsize,fillch)      /* display an int in a fixed length field */
int num, fsize, fillch;
{
   int flen;
   char numstr[11];

   itoa(num,numstr,10);
   if ((flen = fsize - strlen(numstr)) > 0)
      disp_rep(fillch,flen);
   disp_str(numstr);
}

int ALTCALL
out_long(num,fsize,fillch)     /* display a long in a fixed length field */
long num;
int fsize, fillch;
{
   int flen;
   char numstr[11];

   ultoa(num,numstr,10);
   if ((flen = fsize - strlen(numstr)) > 0)
      disp_rep(fillch,flen);
   disp_str(numstr);
}

int ALTCALL
out_str(s,fsize,fillch)        /* disp a string (left justified) in a field */
register char *s;
int fsize, fillch;
{
   register char *ep;
   int len, flen, savch;

   if ((len = strlen(s)) > fsize) {    /* string too big for field? */
      savch = *(ep = s + fsize);       /* yes, fake it out by temp */
      *ep = '\0';                      /*   setting it to length of fld */
      disp_str(s);                     /*   display it like normal */
      *ep = savch;                     /*   restore it */

   } else {                    /* strlen <= field size */

      disp_str(s);
      if ((flen = fsize - len) > 0)
         disp_rep(fillch,flen);
   }
}


/******************************************************************************
 **            L O W   L E V E L   V I D E O   C O N T R O L                 **
 *****************************************************************************/

savescreen() {         /* save the current screen image */

   /* allocate a far buffer for the screen image */

   if ((image[imageidx] = malloc_f(SCREEN_ROWS*SCREEN_COLS*2)) == NULL)
      show_error(0,13,1,"No memory to save screen image");

   scrcpy(image[imageidx],screen);             /* copy the screen image */
   savcursor[imageidx] = cursor;               /* save the current cursor loc */
   imageidx++;                                 /* "stack" the screen image */
}

restorescreen() {      /* restore the saved screen image */

   imageidx--;                                 /* "pop off" a screen image */
   scrcpy(screen,image[imageidx]);             /* restore the screen image */
   cursor = savcursor[imageidx];               /* restore the cursor position */
   free_f(image[imageidx]);                    /* release screen memory */
}


savecursor() {         /* save the current cursor state */

   save_cursor = cursor;               /* save the software cursor */

   /* save the hardware cursor status */
   readcursor(0,&save_row,&save_col,&save_stscan,&save_endscan);
}

restorecursor() {      /* restore cursor to saved status */

   union REGS r;

   cursor = save_cursor;               /* restore software cursor */

   /* restore hardware cursor */

   r.h.ah = 2;                 /* position cursor */
   r.h.dh = save_row;
   r.h.dl = save_col;
   r.h.bh = 0;
   int86(16,&r,&r);

   setcursorsize(save_stscan,save_endscan);    /* restore cursor size */
}


hidecursor() {         /* hide the terminal cursor */
   setcursorsize(32,0);           /* turn off hardware cursor */
}


showcursor() {         /* make the terminal cursor visible */

  union REGS rg;

  rg.h.ah = 2;                       /* if direct screen i/o is being done, */
  rg.h.dh = FP_OFF(cursor) / 160;    /* the hardware cursor position is not */
  rg.h.dl = (FP_OFF(cursor)%160)>>1; /* updated along with the faked cursor */
  rg.h.bh = 0;                       /* position, sync the hardware to the  */
  int86(16, &rg, &rg);               /* simulated cursor before displaying it */

  setcursorsize(cstscan,cendscan);       /* turn the curosr back on */
}


clr_scr() {            /* clear the screen */

   gotorc(0,0);
   disp_rep(' ',SCREEN_ROWS*SCREEN_COLS);
   gotorc(0,0);
}

int ALTCALL
clr_eol() {            /* clear to the end of the current line */

   char far *cp;

   cp = cursor;                        /* start clearing at cursor loc */

   disp_rep(' ',(160 - (FP_OFF(cursor) % 160)) >> 1);

   cursor = cp;                        /* resotre cursor location */
}

beep() {               /* ring the bell on the teriminal */
   putchr(7);
}
