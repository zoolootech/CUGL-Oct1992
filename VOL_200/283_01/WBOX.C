/* wbox.c -- 9/5/88, d.c.oshel
   */

#include "vidinit.h"


/*======================*/
/* WindowBox            */
/*======================*/

/* this version draws even the largest box almost instantaneously
** dco, 8/29/86
*/

void windowbox( int tx, int ty, int bx, int by )
{
        extern int ScrollAttributeFlag;  /* in scroll.c */

        static int i,j; 
        static int temp;

        if (!Initialized) vid_init(0);

        ScrollAttributeFlag = vid_attr + 1;

        /* first, clear the working area to startle and amuse...
        */

        setwindow( tx, ty, bx, by );
        clrwindow();

        /* now, draw a crisp double-lined box (user's vid_attr ignored)
        */

        temp = vid_attr;

        /* unfortunately, this draws an invisible box with CGA card and b&w crt
        vid_attr = (video.mode == 2)? vid[15]: vid[10];
        */
        vid_attr = (video.mode == 7)? vid[10]: vid[15];
        
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

        j = by - ty;       /* put the for loop condition into a static int */
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



