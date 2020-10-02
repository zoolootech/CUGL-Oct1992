/*
**   ciao.c
**   sept 10, 1986, by david c. oshel, ames, iowa
**
**  ----->  COMPILE USING UNSIGNED CHAR
**
**   Console input and output for the 101% IBM PC clone.  This is the first
**   module in my CIAO.LIB library.
**
**   These are FAST primitives to read from and write to the IBM video RAM.
**   Ignores the ROM-BIOS except to set text mode and/or to read or set the 
**   machine cursor (see just below).
**
**   The module is self-initializing.  Vid_init(n) is only required to set
**   a particular mode, e.g., vid_init(3) to set 80x25 color text.  The
**   requested mode is only set if the hardware supports it, and only
**   monochrome mode 7 and cga modes 2 or 3 are valid.  No graphics modes.
**
**   Global functions which ALTER THE CONTENTS OF THE SCREEN test the 
**   initialized flag.  In general, static, ROM-BIOS and cursor functions 
**   do NOT test the flag.  If the flag is still zero, vid_init executes.
**
**   The machine cursor and the video RAM write location ("soft cursor")
**   are independent, but are synchronized by default.  See setsynch fn.
**
**   Compiler is Microsoft C ver. 4.00, but this particular module should be 
**   fairly portable to another compiler (such as Microsoft C ver. 3.00). 
**
*/


/*=======================================================================*
     There are 16 public Video Attribute Registers:  vid[0] ... vid[15]

     The Clairol routine (CLAIROL.OBJ) recognizes four major message
     levels, associated with vid[0], vid[1], vid[2], vid[3].  This is
     the popout window that allows user access to the first four video
     registers (only).

     Programmers have access to all 16 registers at all times, using
     the CLAIROL.H header file.

     Clairol           VidReg   ^ commands that set the attribute
     -------------------------------------------------------------------
     Normal            vid[0]   wputs("^0"); wputs("^");
     Bold              vid[1]   wputs("^1"); 
     Emphasis          vid[2]   wputs("^2"); 
     Attention!        vid[3]   wputs("^3"); 

                       vid[4]   wputs("^4");
                       .
                       .
                       .
                       vid[ 9]  wputs("^9");
                       vid[10]  wputs("^ê"); keystroke is ALT 234
                       vid[11]  wputs("^ë");      "       ALT 235
                       vid[12]  wputs("^ì");      "       ALT 236
                       vid[13]  wputs("^í");      "       ALT 237
                       vid[14]  wputs("^î");      "       ALT 238
                       vid[15]  wputs("^ï");      "       ALT 239


     The DEFAULT contents of these registers is as follows:

     Contents      *Color/Graphics Adapt.   Monochrome Adapter
     -----------------------------------------------------------------
     Normal         brite white on blue     normal
     Bold           brite yellow on black   bright normal
     Emphasis       brite blue on white     reverse
     Attention      blink br. white on red  blinking reverse

     vid[ 4]       *red, 4                  underline
     vid[ 5]        magenta, 5              bright underline    
     vid[ 6]        dark yellow, 6          blinking normal
     vid[ 7]        ordinary white, 7       blinking underline
     vid[ 8]        dark grey, 8            blinking bright normal 
     vid[ 9]        brite blue, 9           blinking bright underline
     vid[10]        brite green, 0x0a       normal
     vid[11]        brite cyan, 0x0b        normal
     vid[12]        brite red, 0x0c         normal
     vid[13]        brite magenta, 0x0d     normal
     vid[14]        brite yellow, 0x0e      normal
     vid[15]        brite white, 0x0f       normal

     *The default background is black for registers vid[4]..vid[15], and
      blink is off.

 *=======================================================================*/



#define LINT_ARGS

#include <alloc.h>     /* _fmalloc(), _ffree()  */
#include <conio.h>      /* direct console: putch(), getch(), etc */

#include "ciao.h"

/* these defines are for ciao.c alone; they are local, not for ciao.h
*/


#define SCRLIM 4000                         /* 80x25 chars & attrs in screen */
#define TOPX 0                              /* 80x25 screen margin defaults  */
#define TOPY 0
#define BTMX 79
#define BTMY 24
#define GOXY (2*(col+(row*80)))             /* yields absolute screen address */
#define SPC ' '                             /* blank char, for clreol(), etc. */

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


/*
** globals
*/


int vid[16] =  /* vid_init() changes this table if cga */
{
    NRM, BRN, RVR, BLR, 
    UNL, BRU, BLN, BLU, 
    BBN, BBU, NRM, NRM,
    NRM, NRM, NRM, NRM
};

int vid_mode = 7;                  /* monochrome is default */
int rasterh  = 12, rasterl  = 13;  /* monochrome cursor default raster lines */


/*
** locals
*/

static int Initialized = 0;        /* are all the critical pointers set? */ 

static union REGS old_vid;

static int vid_seg  = 0xB000,      /* monochrome screen RAM base address */
           vid_attr = 7,           /* HEAVILY USED HEREIN */
           vid_page = 0,           /* "active" page is default (unused?) */ 
           vid_wide = 80;          /* unused */

static char far *scribble;         /* transfer depot for RAM read/write */
static char far *hidescreen;       /* pointer to invisible screen buffer */

static int activescreen = 0xB000, 
           row = 0,
           col = 0; 

static union REGS xy;              /* holds machine cursor address for set */

static int synchronized = 1;       /* default to hard & soft cursors alike */

static int lm = TOPX,
           rm = BTMX,
           tm = TOPY,
           bm = BTMY;              /* default window margins */



/* H_pfill().  Pattern fill routine, hardwired to active screen, scribble.
**
** Called by rptchar(), scrollup(), scrolldn().  Use with discretion
** because there is NO error checking!
**
** Assumes scribble is already set, plus any number of other hardwired
** characteristics.  Generalizing this for any size of pattern source buffer
** and any size of destination fill buffer might be useful.
**
** Movedata is very efficient.  I suspect it just sets up registers and
** then executes a single 8086 machine instruction to do the block move.  
** The result is instantaneous, at least to the proverbial naked orb.
*/

static void H_pfill( base, cnt ) int base, cnt;  
{
     static int width = 2;  /* hardwired pattern (scribble) size */

     cnt *= width;    /* translate number of pattern objects 
                      ** to number of destination bytes 
                      */

     /* SET PATTERN IN DEST BUFFER:  Write pattern at least once.
     */
     movedata( FP_SEG(scribble), FP_OFF(scribble), /* from */ 
               activescreen, base,                 /* to   */
               width);

     cnt -= width;                           /* one object already moved */
     if (cnt > 0)                            /* shall we continue? */
     {
        /* ULTRAFAST PATTERN FILL:  A source byte moved to the destination
        ** on iteration N extends the source pattern for iteration N+1.
        */
        movedata( activescreen, base,           /* srce (!)  */
                  activescreen, base + width,   /* dest (!!) */
                  cnt);
     }
}

static void setmchnloc(void); /* forward ref to static ROM-BIOS function, below */

void gotoxy( x, y ) /* 0,0 RELATIVE TO TOPLEFT CORNER OF CURRENT WINDOW! */
int x, y;
{

    y %= 1 + bm - tm;  /* mod number of lines in window */
    x %= 1 + rm - lm;  /* mod number of columns in line */

    row = tm + y;      /* translate window-relative to fullscreen-absolute */
    col = lm + x;

    if (synchronized)  /* update machine cursor? */
        setmchnloc();
} 




/* Scroll functions honor the current window, and do not alter global
** col & row settings.  The window scrolls up or down one line at a time.
**
** NOTICE:   The fresh, blank line which results from scrolling has the
**           Normal (vid[0]) attribute.  If this "fixup" were not attempted,
** the line would have the attribute of the last character in the text which
** formerly occupied the lowest (or highest) line.  Either approach is wrong
** in some circumstances, but the Normal attribute should be correct in most
** cases.  Caller may either execute clreol() to repair the window line, or
** modify vid[0] prior to scrolling.
*/

void scrollup()  /* scroll current window up one line */
{
    register int i, b1;
    auto int count, limit;

    if (!Initialized) vid_init(0);

    b1 = 2*(lm+(tm*80));           /* find top left corner of window */
    limit = bm - tm;               /* and number of lines in window, less one */
    count = 2*(1 + rm - lm);       /* width of line */

    for (i = 0; i < limit; i++)  /* move all contents up one line */
    {
        movedata( activescreen, b1 + 160,   /* source */
                  activescreen, b1,         /* destination */
                  count); 
        b1 += 160;
    }

    scribble[0] = SPC;           /* clear bottom line of window */
    scribble[1] = vid[0];
    H_pfill( 2*(lm+(bm*80)), 1 + rm - lm );
}

void scrolldn()  /* scroll current window down one line */ 
{
    register int i, b1; 
    auto int count, limit;

    if (!Initialized) vid_init(0);

    b1 = 2*(lm+(bm*80));           /* find bottom left corner of window */
    limit = bm - tm;               /* and number of lines in window, less one */
    count = 2*(1 + rm - lm);       /* width of line */

    for (i = 0; i < limit; i++)  /* move all contents down one line */
    {
        movedata( activescreen, b1 - 160,   /* source */
                  activescreen, b1,         /* destination */
                  count);
        b1 -= 160;
    }

    scribble[0] = SPC;           /* clear top line of window */
    scribble[1] = vid[0];
    H_pfill( 2*(lm+(tm*80)), 1 + rm - lm );
}



void getwindow( tx, ty, bx, by )  /* read current window dimensions */
int *tx, *ty, *bx, *by;
{
    *tx = lm;
    *ty = tm;
    *bx = rm;
    *by = bm;
}


void setwindow( tx, ty, bx, by )
int tx, ty, bx, by;
{
   if (bx > BTMX) bx = BTMX;
   if (tx < TOPX) tx = TOPX;
   if (by > BTMY) by = BTMY;
   if (ty < TOPY) ty = TOPY;

   if (tx >= bx || ty >= by)  /* absurd values? set full screen reduced by 1 */
   { 
       putch(7); /* complain but continue */
       lm = TOPX + 1; rm = BTMX - 1; tm = TOPY + 1; bm = BTMY - 1; 
   } 
   else
   {
       lm = tx;
       rm = bx;
       tm = ty;
       bm = by;
   }
}




void fullscreen()
{
   setwindow( TOPX, TOPY, BTMX, BTMY );
}




/*===========================*/
/* screen character handlers */
/*===========================*/

void setscreen( n ) int n;  /* set activescreen = 1 or 2, screen 1 is visible */
{
     if (!Initialized) vid_init(0);

     switch (n)
     {
         case 2:  { activescreen = FP_SEG( hidescreen ); break; }
         default:
         case 1:  { activescreen = vid_seg; break; }
     }
}


void swapscreen( n ) int n;  /* swap hidden screen n with visible screen 1 */
{                            /* for now, n can only be 2  */   
     char far *save;
     int tricseg, tracseg, saveseg, temp;

     if (!Initialized) vid_init(0);

     save = _fmalloc( SCRLIM );
     saveseg = FP_SEG( save );

     temp = activescreen;

     if (n < 2) n = 2;       /* just now, range check on n is trivial */
     if (n > 2) n = 2; 

     setscreen( n );              /* hidden screen n */
        tracseg = activescreen;
     setscreen( 1 );              /* visible screen 1 */ 
        tricseg = activescreen;

     activescreen = temp;

     movedata( tracseg, 0, saveseg, 0, SCRLIM);  /* swap contents of screens */
     movedata( tricseg, 0, tracseg, 0, SCRLIM);
     movedata( saveseg, 0, tricseg, 0, SCRLIM);

     _ffree( save );
}



void setsynch( on ) int on;  /* synchronized is ON by default */
{
     if ((synchronized = on? 1: 0) == 1)
         setmchnloc();
}




void rptchar( ch, cnt ) char ch; int cnt;  /* does NOT update screen position */
{
     if (!Initialized) vid_init(0);

     scribble[0] = ch;
     scribble[1] = vid_attr;
     if (cnt >= 1) 
         H_pfill( GOXY, cnt );  /* use pattern fill for speed */
}



void readscreenchar( ch ) SCW *ch;
{
    if (!Initialized) vid_init(0);

    movedata( activescreen, GOXY,
              FP_SEG(scribble), FP_OFF(scribble), 
              2);
    ch->byte = scribble[0];
    ch->attr = scribble[1];
}



/*============================*/
/* screen clearing functions  */
/*============================*/



void clreol()  /* honors current window setting */
{
    if (!Initialized) vid_init(0);

    rptchar(SPC, 1 + rm - col);
}


void clrwindow()   /* home cursor, clear window */
{
    int lin, yrel, xrel, temp;

    if (!Initialized) vid_init(0);

    temp = synchronized;
    setsynch( 0 );        /* don't bother gotoxy with machine cursor updates */
    lin = tm;
    xrel = yrel = 0;
    while (lin++ <= bm)
    {
        gotoxy(xrel, yrel++);
        clreol();
    }
    gotoxy(0,0);
    setsynch( temp );
}


void clreos()  /* screen only, ignores window */
{
    if (!Initialized) vid_init(0);
    rptchar( SPC, (SCRLIM - GOXY) / 2 );

}


void clrscrn()  /* resets full screen, homes cursor, clears screen */
{
    if (!Initialized) vid_init(0);

    fullscreen();
    gotoxy(0,0);
    rptchar(SPC,SCRLIM /2);
}


/*======================*/
/* WindowBox            */
/*======================*/

/* this version draws even the largest box almost instantaneously
** dco, 8/29/86
*/

void windowbox( tx, ty, bx, by ) int tx, ty, bx, by;
{
        register int i,j; 
        auto int temp;

        if (!Initialized) vid_init(0);

        /* first, clear the working area to startle and amuse...
        */

        setwindow( tx, ty, bx, by );
        clrwindow();

        /* now, draw a crisp double-lined box (user's vid_attr ignored)
        */

        temp = vid_attr;
        vid_attr = (vid_mode == 2)? vid[15]: vid[10];

        tx = lm; /* use values that setwindow range checking may have found! */
        ty = tm;
        bx = rm;
        by = bm;

        /* then, draw the border AROUND the area!
        */

        fullscreen();        /* allows gotoxy to use the argument dimensions */

        if (tx > TOPX) { --tx; }
        if (ty > TOPY) { --ty; }
        if (bx < BTMX) { ++bx; }
        if (by < BTMY) { ++by; }

        /* top line
        */
        gotoxy( tx, ty );
        rptchar('Í',bx - tx);

        /* right side
        */
        gotoxy( bx, ty );
        rptchar('»',1);

        j = by - ty;       /* put the for loop condition into a register int */
        for ( i = 1; i < j; i++ )
        {
             gotoxy( bx, ty + i);
             rptchar('º',1);
        }

        /* bottom line 
        */
        gotoxy( bx, by );
        rptchar('¼',1);

        gotoxy( tx, by );
        rptchar('Í',bx - tx);
        rptchar('È',1);

        /* left side
        */
        for ( i = 1; i < j; i++ )
        {
             gotoxy( tx, ty + i);
             rptchar('º',1);
        }

        gotoxy(tx,ty);
        rptchar('É',1);

        /* last, restore entry dimensions, with fixups on squashed margins
        */

        /* Box is AROUND window    Window is squashed onto box's outline */
        /* ---------------------   ------------------------------------- */
           ++tx;                   /* else tx always was TOPX, so tx++;  */
           ++ty;                   /* else ty always was TOPY, so ty++;  */
           --bx;                   /* else bx always was BTMX, so bx--;  */
           --by;                   /* else by always was BTMY, so by--;  */

        setwindow( tx, ty, bx, by );
        gotoxy(0,0);
        vid_attr = temp;  /* restore user's vid_attr */
}




void wink( c )  /* window character out, obeys backspace and newline */
char c;
{

    if (!Initialized) vid_init(0);

    /* newline? */
    if (c == '\n' || c == '\r') col = 80;  /* set range err to force CR */

    /* backspace? nondestructive = 8, destructive = 127 */
    else if ( c == '\b' || c == 127)  /* NOTICE:  THIS PATH RETURNS */
    {
        col--;               /* decrement the column */
        if (col < lm)        /* beyond left margin? */
        {
             row--;          /* yes, decrement the line */
             if (row < tm)   /* above top margin? */
             {
                row = tm;    /* yes, stick at 0,0 -- does not scroll down! */
                col = lm;
             }
             else col = rm;  /* no, jump to rightmost column */ 
        }
        if (c == 127) rptchar(SPC,1); /* destroy existing char on screen    */
        goto synch;
    } 

    else rptchar( c, 1 );    /* uses vid_attr attribute */

    /* bump x cursor position, do newline or scroll window up if necessary */

    if ( col >= rm )      /* need to newline? */
    {
        col = lm;
        if ( row >= bm )  /* need to scroll? */
        {
            row = bm;
            scrollup();
        }
        else row++;
    }
    else col++;

    synch:
    if (synchronized) /* update machine cursor */
        setmchnloc();

} /* wink */





void wputs( p )  /* write string into window, uses wink */
char *p;
{
     register int test;
     auto int temp;

     if (!Initialized) vid_init(0);
 
     temp = synchronized;
     setsynch(0);
     while (*p) 
     {
         if (*p == '^')  /* escape character? switch modes */
         {
             p++;              /* read escape command char */
             test = *p & 0xFF; /* kill sign extension */
             switch ( test ) 
             {
                 case  '0': { vid_attr = vid[ 0]; break; }
                 case  '1': { vid_attr = vid[ 1]; break; }
                 case  '2': { vid_attr = vid[ 2]; break; }
                 case  '3': { vid_attr = vid[ 3]; break; }

                 case  '4': { vid_attr = vid[ 4]; break; }
                 case  '5': { vid_attr = vid[ 5]; break; }
                 case  '6': { vid_attr = vid[ 6]; break; }
                 case  '7': { vid_attr = vid[ 7]; break; }
                 case  '8': { vid_attr = vid[ 8]; break; }
                 case  '9': { vid_attr = vid[ 9]; break; }

                 case  'ê': { vid_attr = vid[10]; break; } /* ea */
                 case  'ë': { vid_attr = vid[11]; break; } /* eb */
                 case  'ì': { vid_attr = vid[12]; break; } /* ec */
                 case  'í': { vid_attr = vid[13]; break; } /* ed */
                 case  'î': { vid_attr = vid[14]; break; } /* ee */
                 case  'ï': { vid_attr = vid[15]; break; } /* ef */

                 case  '^': { 
                            wink( *p );  /* verbatim ^ char */
                            break; 
                            }
                 default:   { 
                            --p; 
                            vid_attr = vid[0];  /* just ^, no command arg */
                            break; 
                            }
             }
             p++;  /* next after escape command */
         }
         else 
         {
             wink( *p++ );  /* obeys newline, tab, if present */
         }
     }
     setsynch(temp); /* update machine cursor only at last, if at all */

} /* wputs */



/*================================*/
/*  ROM-BIOS dependent functions  */
/*================================*/

static void setmchnloc(void)  /* the ROM-BIOS gotoxy function */
{
    xy.h.ah = 2;
    xy.h.bh = 0;
    xy.h.dh = row;
    xy.h.dl = col;
    int86( 0x10, &xy, &xy );
}


void setcursize( r1, r2 )
int r1,r2;
{
    union REGS rx;

    rx.h.ah = 1;
    rx.h.ch = r1;
    rx.h.cl = r2;
    int86( 0x10, &rx, &rx );
}


void defcursor()  /* set default cursor for cga or mono */
{
    setsynch(1);
    setcursize( rasterh, rasterl );
}


/* The cursor save and restore routines recognize the synchronized flag.
** If synchronized, the machine cursor is saved or restored, and the soft
** cursor is identical to the machine cursor; if not synchronized, the
** soft cursor is saved or restored, and the machine cursor (wherever it
** happens to be) is ignored.
**
** The value of the current synchronized flag is saved or returned:
**
**   Savecursor() returns the value of the synchronized flag, and saves it.
**   Restcursor() returns the value of the synchronized flag that was active
**                when the cursor was saved, but does not restore it.
*/

int savecursor( x )
union REGS *x;
{
    if (synchronized)  /* hard and soft are identical, save hard */
    {
       x->h.ah = 3;
       x->h.bh = 0;
       int86( 0x10, x, x );
    }
    else               /* save soft */
    {
       x->h.dh = row;
       x->h.dl = col;
    }
    x->h.ah = synchronized;  /* save the flag for proper kind of restore */
    return( x->h.ah );
}


int restcursor( x )
union REGS *x;
{
    if (x->h.ah != 0)        /* non-zero, synchronization was ON when saved */
    {
       x->h.ah = 1;          /* so restore machine size */
       x->h.bh = 0;          /* ensure "active" page */
       int86( 0x10, x, x );
       x->h.ah = 2;          /* and machine location */
       int86( 0x10, x, x );
    }
    row = x->h.dh;           /* in any case, set soft location to match */
    col = x->h.dl;
    return( x->h.ah );
}



void hidecursor()
{
    setsynch(0);
    setcursize( 32,32 );
}


/*
**  ================================================
**  Sample calls to savescreen(), restorescreen()
**  Note:  These only work with text screens, 
**  they do not save CRTpage/CPUpage screen info.
**  They DO save the current window margins!
**  ================================================
**
**   main() {
**
**   union REGS x,z;
**   char far *screen1, *screen2;
**
**   vid_init();                            WARNING: must call vid_init()
**   screen1 = savescreen( &x );
**   .
**   .
**          screen2 = savescreen( &z );     WARNING: these MUST nest properly
**          .                                        or heap allocation will
**          .                                        be annihilated! 
**          .
**          restorescreen( screen2, &z );
**   .
**   .
**   restorescreen( screen1, &x );
**   }
**
*/

static void sorry( n ) int n;
{
    char *p,*q;
    q = "init";
    switch (n)
    {
    case 2:  q = "restorescreen"; p = "no screen saved, can't restore"; break;
    case 1:  q = "savescreen";
    case 0:  p = "not enough k, can't continue"; break;
    default: q = "nasty"; p = "looks fatal"; break;
    } 
    printf("\n\aCIAO: %s error: %s\n",q,p);
    exit(1);
}


char far *savescreen( cursor )  /* caller must keep pointer to storage! */
union REGS *cursor;
{
    union REGS ry;
    char far *scrfptr;
     
    if (!Initialized) vid_init(0);

    ry.h.ah = 0x03;                   /* read cursor position...       */
    ry.h.bh = 0;
    int86( 0x10, &ry, cursor );       /* store result in caller's var  */

    cursor->x.cflag = 1066;           /* a famous date in history...   */
    cursor->h.ah = tm;                /* save caller's window settings */
    cursor->h.al = lm;                /* cannot use bx, cx or dx       */
    cursor->x.di = rm;
    cursor->x.si = bm;

    scrfptr = _fmalloc( SCRLIM );
    if ( scrfptr == 0L )
    {
        sorry(1);
    }
    movedata( vid_seg, 0, FP_SEG( scrfptr ), FP_OFF( scrfptr ), SCRLIM );
    return (scrfptr);
}




void restorescreen( scrfptr, cursor ) /* caller keeps pointer to storage! */
char far *scrfptr;
union REGS *cursor;
{
    union REGS rx;

    if (!Initialized || cursor->x.cflag != 1066)
    {
        sorry(2);
    }

    movedata( FP_SEG( scrfptr ), FP_OFF( scrfptr ), vid_seg, 0, SCRLIM );
    _ffree( scrfptr );

    tm = cursor->h.ah;        /* restore caller's window settings */
    lm = cursor->h.al;
    rm = cursor->x.di;
    bm = cursor->x.si;
    cursor->x.ax = cursor->x.si = cursor->x.di = cursor->x.cflag = 0;

    rx.h.ah = 1;
    rx.x.bx = cursor->x.bx;
    rx.x.cx = cursor->x.cx;     /* set old cursor size     */
    int86( 0x10, &rx, &rx );
    rx.h.ah = 2;
    rx.x.bx = cursor->x.bx;
    rx.x.dx = cursor->x.dx;
    int86( 0x10, &rx, cursor ); /* set old cursor position */
}




/*
** Initialize, Deinitialize
*/

static void init_criticals(mode) int mode;
{
    int i;
    union REGS rx;

    if (((scribble = (char far *) _fmalloc(2)) == 0L) ||
       ((hidescreen  = (char far *) _fmalloc(SCRLIM)) == 0L)) 
           sorry(0);

    rasterh  = 12;                 /* assume monochrome cursor */
    rasterl  = 13;

    rx.h.ah = 15;                  /* read current video mode */
    int86( 0x10, &rx, &old_vid );
    vid_page = old_vid.h.bh;
    vid_wide = old_vid.h.ah;
    vid_mode = old_vid.h.al;   /* a 7 means we found monochrome hardware */


    if (mode == 0) mode = vid_mode;  /* request to stay in current mode? */

    /* in any case, allow only mono mode 7, or cga modes 2 or 3 */

    if (vid_mode != 7)         /* is hardware a color/graphics adapter? */
    {
       vid_seg = 0xB800;       /* yes, screen RAM base address is different! */
       if ( mode == 3 )        /* asking for color?  set it up! */
       {
          for (i = 0; i < 16; i++) 
              vid[i] = i;                      /* set color defaults */
          vid[0]  = 0x17 | 0x08;               /* set normal         */
          vid[1]  = 0x06 | 0x08;               /*  "  bold           */
          vid[2]  = 0x71 | 0x08;               /*  "  emphasis       */
          vid[3]  = 0x47 | 0x08 | 0x80;        /*  "  attention      */
       }
       else 
       {
          mode = 2;                 /* only modes 2 and 3 are allowed, here */
          for (i = 4; i < 16; i++)  /* all else defaults to normal */
              vid[i] = vid[0];
       }
       rasterh = 6;               /* cga default cursor raster lines */
       rasterl = 7;
       if (vid_mode != mode)      /* not already in the requested mode? */
       {
          rx.h.ah = 0;            /* if not, set requested mode */
          rx.h.al = mode;
          int86( 0x10, &rx, &rx );
       }
       rx.h.ah = 15;              /* read current mode again */
       int86( 0x10, &rx, &rx );
       vid_page = rx.h.bh;        /* set globals accordingly */ 
       vid_wide = rx.h.ah;
       vid_mode = rx.h.al;
    }
    Initialized = 1;              /* we are, basically, but...  */
    setscreen(1);                 /* ensure activescreen is set */
    vid_attr = vid[0];            /* and ensure vid_attr is set */
}




/* Vid_Init() - This is a "MUST CALL", but if the various routines which
**              depend on finding initialized values and pointers find the
**              Initialized flag set to zero, they will take it on themselves
**              to call vid_init(0).  Any argument to vid_init is valid, but
**              mode 7 is set if the monochrome adapter is in the system, and
**              only modes 2 or 3 are set if the cga card is found.  The 
**              0 argument requests that a valid current mode not be changed;
**              this prevents the firmware from clearing the screen, if it's
**              not absolutely necessary.  Vid_init() will always select the
**              visible screen, and synchronize the soft & machine cursors.
**              Also, selects appropriate default cursor size; cga and mono
**              have different cursors.
*/

void vid_init( mode ) int mode;
{
    auto union REGS x;

    init_criticals(mode);      /* sets initialized flag on exit */

    savecursor( &x );          /* We can now make sure that hard and   */
    row = x.h.dh;              /* soft cursors actually are identical, */
    col = x.h.dl;              /* going soft <- hard!  Otherwise, the  */
    synchronized = 1;          /* first savescreen( &cursor ) doesn't! */
}                              




void vid_exit()
{
     if (Initialized)
     {
          if (old_vid.h.al != vid_mode)
          {
               old_vid.h.ah = 0;
               int86( 0x10, &old_vid, &old_vid );
          }
          _ffree(hidescreen);
          _ffree(scribble);
          Initialized = 0;  /* we're NOT initialized any more! */
     }
     setcursize( rasterh, rasterl );
}


/* eof: ciao.c */

