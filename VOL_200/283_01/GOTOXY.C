/* gotoxy.c -- 9/5/88, d.c.oshel

   These are the functions that directly affect the machine cursor,
   and CIAO's soft cursor screen write location.
   */


#include "vidinit.h"


void setmchnloc( void )  /* the ROM-BIOS gotoxy function */
{
    static union REGS xy;

    xy.h.ah = 2;
    xy.h.bh = 0;
    xy.h.dh = row;
    xy.h.dl = col;
    int86( 0x10, &xy, &xy );
}


void setcursize( int r1, int r2 )
{
    union REGS rx;

    rx.h.ah = 1;
    rx.h.ch = r1;
    rx.h.cl = r2;
    int86( 0x10, &rx, &rx );
}


/* RELATIVE TO TOPLEFT CORNER (0,0) OF CURRENT WINDOW! */
void gotoxy( int x, int y ) 
{
    if (!Initialized) vid_init(0);

    y %= 1 + bm - tm;  /* mod number of lines in window */
    x %= 1 + rm - lm;  /* mod number of columns in line */

    row = tm + y;      /* translate window-relative to fullscreen-absolute */
    col = lm + x;

    if (synchronized)  /* update machine cursor? */
        setmchnloc();
}


void clrscrn( void )
{
    if (!Initialized) vid_init(0);

	lm = tm = row = col = 0;
    rm = 79;
    bm = 24;
    gotoxy(0,0);
    MSJ_SetFldAttr( SPC, row, col, vid_attr, 2000, &video );
}


void clreol( void )
{
    if (!Initialized) vid_init(0);

    MSJ_SetFldAttr( SPC, row, col, vid_attr, rm-col+1, &video );
}


void clreos( void )
{
    if (!Initialized) vid_init(0);

    clreol();
    MSJ_ClrRegion( row+1, lm, bm, rm, vid_attr );
}

