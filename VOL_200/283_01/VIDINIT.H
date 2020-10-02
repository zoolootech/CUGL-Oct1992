/* vidinit.h -- 9/5/88, d.c.oshel
   */ 

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <malloc.h>

#include "video.h"    /* Microsoft Systems Journal routines */

#define SCRLIM 4000   /* 80x25 chars & attrs in screen */
#define TOPX 0        /* 80x25 screen margin defaults  */
#define TOPY 0
#define BTMX 79
#define BTMY 24
#define SPC ' ' /* blank char, for clreol(), etc. */


/* useful macros */

#define ptrseg(P) ((int)(((long)P)>>16))  /* char far * ONLY! */
#define ptrofs(P) ((int)((long)P))

#define GOXY ((col+((row<<4)+(row<<6)))<<1) 

#define getsynch()   (synchronized)
#define setsynch(X)  {synchronized=(X);}

#define hidecursor() setcursize(32,32)
#define defcursor()  setcursize(rasterh,rasterl)

#define fullscreen() setwindow(TOPX,TOPY,BTMX,BTMY)

#define normal_video()     {vid_attr=vid[0];}
#define intense_video()    {vid_attr=vid[1];}
#define reverse_video()    {vid_attr=vid[2];}
#define warning_video()    {vid_attr=vid[3];}

#define rptchar(C,N) MSJ_SetFldAttr(C,row,col,vid_attr,N,&video)

/* monochrome monitor attributes :----------------*/

#define INV '\000'   /* invisible                 */
#define UNL '\001'   /* underline                 */
#define NRM '\007'   /* normal                    */
#define BRU '\011'   /* bright underline          */
#define BRN '\017'   /* bright normal             */
#define RVR '\160'   /* reverse                   */
#define BLU '\201'   /* blinking underline        */
#define BLN '\207'   /* blinking normal           */
#define BBU '\211'   /* blinking bright underline */
#define BBN '\217'   /* blinking bright normal    */
#define BLR '\360'   /* blinking reverse          */


/*=========== CIAO Global Data ===========*/

extern int vid[16];
extern int rasterh, rasterl;
extern int vid_attr, vid_page, row, col;
extern int lm, rm, tm, bm;
extern int synchronized; /* i.e., hard and soft cursors */
extern char far *hidescreen;
extern char far *scribble;
extern int activescreen, activeoffset;
extern int Initialized;

/*============ CIAO Functions ============*/

/* vidinit.c */

extern void vid_init( int mode );
extern void vid_exit( void );

/* gotoxy.c */

extern void gotoxy( int x, int y );
extern void setcursize( int r1, int r2 );
extern void clrscrn( void );
extern void clreol( void );
extern void clreos( void );

/* swapscrn.c */

extern void setscreen( int n );
extern void swapscreen( int n );

/* savescrn.c */

extern int savecursor( union REGS *x );
extern int restcursor( union REGS *x );
extern char far *savescreen( union REGS *cursor );
extern void restorescreen( char far *scrfptr, union REGS *cursor );

/* scrollup.c */

extern void scrollup( void );

/* scrolldn.c */

extern void scrolldn( void );

/* window.c */

extern void getwindow( int *tx, int *ty, int *bx, int *by );
extern void setwindow( int tx, int ty, int bx, int by );
extern void clrwindow( void );

/* wbox.c */

extern void windowbox( int tx, int ty, int bx, int by );

/* wink.c */

extern void wink( char c );

/* wputs.c */

extern void wputs( char *p );

