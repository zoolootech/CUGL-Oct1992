
/* ciao.h - declarations for all global data & functions in ciao.lib
**          the module is self-initializing, vid_init is only required
**          to set a particular text mode (of 7, or 2 or 3).
**
** 8 sept 1986, by david c. oshel, ames, iowa
*/


#include <stdio.h>  /* list of files I almost always include */
#include <stdlib.h>
#include <dos.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

typedef struct screenword { unsigned byte, attr; } SCW;  

extern int vid[16],        /* video registers 0..15 (see clairol() fn) */
           vid_mode,       /* hardware configuration */
           rasterh,        /* machine cursor high raster line */
           rasterl,        /* machine cursor low  raster line */
           tone,           /* no sound effects if false */
           ambiguity,      /* clairol: ambiguous message levels on entry? */
           Control_C_Flag; /* Typeafile exit condition flag */

/* window primitives */

extern void fullscreen(void);     /* resets 80x25 full window dimensions */
extern void clreos(void);         /* ENTIRE SCREEN, from cursor */
extern void clrscrn(void);        /* ENTIRE SCREEN, reset 80x25, home, clear */

extern void gotoxy(int ,int );    /* 0,0 is top left corner of window */
extern void clreol(void);         /* window */
extern void clrwindow(void);      /* window */
extern void wink(char );          /* window character out, scroll as needed */
extern void wputs(char *);        /* window string out via wink, ^ is valid */
extern void scrollup(void);       /* window */
extern void scrolldn(void);       /* window */

/* top left, bottom right coords: args are relative to 80x25 screen size */

extern void windowbox(int ,int ,int ,int );       /*  tx,  ty,  bx,  by */
extern void setwindow(int ,int ,int ,int );       /*  tx,  ty,  bx,  by */
extern void getwindow(int *,int *,int *,int *);   /* &tx, &ty, &bx, &by */

/* character primitives, active at current screen position */

extern void rptchar(char ,int );    /* char, count */
extern void readscreenchar(SCW *);  /* byte and attr, see typedef, above */

/* machine cursor */

extern void setsynch(int );        /* 1 or 0, ON or OFF, synch machine cursor */
extern void defcursor(void);       /* visible, synchronized, default size */
extern void hidecursor(void);      /* invisible, de-synchronized */
extern void setcursize(int ,int ); /* high raster, low raster */

/* machine and/or soft cursor, depending on synchronized flag */

extern int savecursor(union REGS *); /* returns synchronized flag */
extern int restcursor(union REGS *); /* returns synch flag state when saved */

/* screen select, swap: contents only */

extern void setscreen(int );   /* write on selected: 1 visible, 2 hidden */
extern void swapscreen(int );  /* swap hidden (2 is only valid) with visible */

/* screen save, restore: contents, cursor, window dimensions */

extern char far *savescreen(union REGS *);           /* calls _fmalloc() */
extern void restorescreen(char far *,union REGS *);  /* calls _ffree()   */

/* initialize, de-initialize */

extern void vid_init(int );  /* call to set color 3, b&w 2, monochrome 7 */
extern void vid_exit(void);  /* defcursor(), frees up pointer allocations */


/* other functions */


/* xeq( directory, program_name );
**
** example: xeq("\\games", "zork /w");
** change directory, invoke program via system() call
** allows program arguments which contain the "/" switch character
** exec() and spawn() eat "/" because of "compatibility with Xenix"
** restores the original working directory on exit
** if there is one, the run time error message is from the new
** invocation of COMMAND.COM, not from C
*/

extern void xeq(char *,char *);


/* typeafile( pathname, wrap, tx, ty, bx, by );
**
** example: typeafile("\\games\\userman.doc", TRUE, 0,0,79,24 );
** types the file, if found, in a window box of the given dimensions
** word wrap in paragraphs if the wrap argument is TRUE
** hanging indentation in the document may cause a messy display because
** leading spaces are preserved in the middle of wrapped lines (!)
** the PgUp key allows you to scroll backwards in the file (!) -- there
** is a remote chance of stack overflow because the algorithm is recursive
** traps Control-C, Control_C_Flag = 1 if exit was via Control-C
** preserves entry screen, restores it on exit
*/

extern void typeafile(char *,int ,int ,int ,int ,int );


/* clairol();
**
** popout window gives users access to vid[0]..vid[3], interpreted as
** message levels Normal, Bold, Emphasis and Attention.  Users may select
** screen colors or monochrome attributes, depending on hardware.
*/

extern void clairol(void );


/* wprintf( fmt, variable_length_argument_list );
**
** calls wputs, recognizes ^ commands
** REQUIRES MICROSOFT C VER. 4.00
*/

extern void wprintf(char *,...);


/* keyin( wait_fn );
**
** wait_fn executed while awaiting keyboard input
** returns characters as positive integers from 1..255
** returns Fn keys in range 128..255 (i.e., high bit set)
*/

extern int keyin(void ( *)());


/* getms( receiving_buffer_pointer, maximum_length, wait_fn, esc_fn );
**
** wait_fn executed while awaiting keyboard input
** esc_fn executed if user types ESC
*/

extern int getms(char  *,int ,void ( *)(),void ( *)());


/* sound */

extern void HIclack( void );      /* musical gadget declarations */
extern void LOclack( void );
extern void bopbleet( void );
extern void blopbloop( void );
extern void thurb( void );
extern void setfrq( unsigned ); /* sound generation primitives declarations */ 
extern void spkron( void );
extern void spkroff( void );
extern void note( unsigned, unsigned );

/* miscellaneous */

extern void noop(void );                     /* do nothing, convenient!    */
extern void sleep(int );                     /* wait n tenths of a second  */
extern void bell(void );                     /* beep: i.e., putch('\a');   */



/* money.h, 11/27/86, d.c.oshel
*/

extern int ESCflag,           /* string edit functions return ESC or MINUS */ 
           MINUSflag;         /* if these booleans are non-zero */

extern char date_mask[9];     /* set_date() fills "00/00/00" to system date */

extern void set_date(void);

extern char *dlrcvt(long );         /* convert long to form 999,999,999.99- */ 
extern char *crdrform(long );            /* same but with trailing Dr or Cr */

extern int getkey(void);          /* keyin(noop) with key click and ^C trap */

extern int edit_masked_numeric(char *,void (*)());  /* Bufptr, FnKeyRoutine */
extern int edit_string(char *,void (*)());          /* Bufptr, FnKeyRoutine */

extern void message(int ,int ,char *,... );       /* x,y, variable arg list */


/* keydefs.h, 12/7/86, d.c.oshel
*/

extern int KeyDefs(char **, char *, char *); /* options array,title,prompt */


