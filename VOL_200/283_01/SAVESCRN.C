/* savescrn.c -- 9/5/88, d.c.oshel
*/

#include "vidinit.h"


/* savecursor() and restcursor() save all aspects of the current window
** and cursor environment, but do not save or restore screen contents;
** both return the value of the save/restored synchronized flag
*/

int savecursor( union REGS *x )
{
    if (!Initialized) vid_init(0);

    x->h.ah = 3;            /* read machine cursor into dh, dl */
    x->h.bh = 0;
    int86( 0x10, x, x );
    x->h.bh    = synchronized;
    x->h.bl    = vid_attr;
    x->x.ax    = (tm * 256) + bm;             /* save window */
    x->x.di    = (lm * 256) + rm;
    x->x.si    = (row * 256) + col;           /* save soft cursor */
    return (synchronized);
}


int restcursor( union REGS *x )
{
    union REGS rx;

    if (!Initialized) vid_init(0);

    /* restore user's window, soft cursor row & col, synch flag, vid_attr */

    tm = bm = x->x.ax;
    tm /= 256;
    bm %= 256;

    lm = rm = x->x.di;
    lm  /= 256;
    rm  %= 256; 

    row = col = x->x.si;
    row /= 256;
    col %= 256;

    synchronized = x->h.bh;
    vid_attr = x->h.bl;

    /* restore machine cursor size and location */

    rx.h.ah = 1;
    rx.x.bx = 0;                /* bh always has page 0, bl unused  */
    rx.x.cx = x->x.cx;          /* set old cursor size     */
    int86( 0x10, &rx, &rx );
    rx.h.ah = 2;
    rx.x.bx = 0;                /* always page 0 */
    rx.x.dx = x->x.dx;          /* dh has row, dl has col (machine's!) */
    int86( 0x10, &rx, x );      /* set old cursor position */
    return (synchronized);
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

static void sorry( int n )
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



/* caller must keep pointer to storage! */
char far *savescreen( union REGS *cursor )
{
    union REGS ry;
    char far *screen;
     
    if (!Initialized) vid_init(0);

    ry.h.ah = 0x03;                   /* read cursor position...       */
    ry.h.bh = 0;                      /* text page 0, only             */
    int86( 0x10, &ry, cursor );       /* store result in caller's var  */

    /* cannot use cx, dx; bh is always 0, bl is unused */

    cursor->x.cflag = 0xDC;                        /* save OKflag */
    cursor->h.bh    = synchronized;
    cursor->h.bl    = vid_attr;
    cursor->x.ax    = (tm * 256) + bm;             /* save window */
    cursor->x.di    = (lm * 256) + rm;
    cursor->x.si    = (row * 256) + col;           /* save soft cursor */

    screen = (char far *) _fmalloc( 4000 );
    if ( screen == (char far *) NULL )
    {
        sorry(1);
    }
    MSJ_MovScrBuf( screen, 0, 0, 2000, &video );

    return (screen);
}




/* caller keeps pointer to storage! */
void restorescreen( char far *screen, union REGS *cursor )
{
    extern int ScrollAttributeFlag;  /* in scroll.c */
    union REGS rx;

    if (!Initialized || (cursor->x.cflag != 0xDC))  /* 0xDC means screen saved */
    {
        sorry(2);
    }


    MSJ_MovBufScr( screen, 0, 0, 2000, &video );
    _ffree( screen );

    ScrollAttributeFlag = 0;

    /* restore user's window, soft cursor row & col, synch flag, vid_attr */

    tm = bm = cursor->x.ax;
    tm /= 256;
    bm %= 256;

    lm = rm = cursor->x.di;
    lm  /= 256;
    rm  %= 256; 

    row = col = cursor->x.si;
    row /= 256;
    col %= 256;

    synchronized = cursor->h.bh;
    vid_attr = cursor->h.bl;

    /* reset, especially, OKflag = 0 in cflag! */

    cursor->x.ax = cursor->x.si = cursor->x.di = cursor->x.cflag = 0;

    /* restore machine cursor size and location */

    rx.h.ah = 1;
    rx.x.bx = 0;                /* bh always has page 0, bl unused  */
    rx.x.cx = cursor->x.cx;     /* set old cursor size     */
    int86( 0x10, &rx, &rx );
    rx.h.ah = 2;
    rx.x.bx = 0;                /* always page 0 */
    rx.x.dx = cursor->x.dx;     /* dh has row, dl has col (machine's!) */
    int86( 0x10, &rx, cursor ); /* set old cursor position */
}

