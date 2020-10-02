/* swapscrn.c -- 9/5/88, d.c.oshel
   */

#include "vidinit.h"


/*=================*/
/* screen handlers */
/*=================*/

void setscreen( int n ) /* set activescreen = 1 or 2, screen 1 is visible */
{
     if (!Initialized) vid_init(0);

     /* this is kinduva donothing routine just now...
        might get around to enhancing or expanding it later
        */

     switch (n)
     {
         case 2:  { activescreen = ptrseg( hidescreen ); 
                    activeoffset = ptrofs( hidescreen );
                  break; }
         default:
         case 1:  { activescreen = video.SegAddr;
                    activeoffset = 0;
                  break; }
     }
}


void swapscreen( int n ) /* swap hidden screen n with visible screen 1 */
{                            /* for now, n can only be 2, is ignored  */   
     char far *p;
     char far *q;
     union REGS x;

     int i, c;

     if (!Initialized) vid_init(0);

     /* at first, the hidden screen contains a copy of whatever
        screen was active when vid_init() was called at startup

        NOTE: savescreen() and restorescreen() use the low-level MSJ
              functions, so swapscreen() now eliminates snow!
        */
        
     p = savescreen( &x );  /* copy visible screen into a buffer */
     q = hidescreen;        /* ptr to invisible hidden screen */

     for ( i = 0; i < SCRLIM; i++ )  /* swap the two buffers */
     {
        c = *( q + i );
            *( q + i ) = *( p + i );
                         *( p + i ) = c;
     }

     restorescreen( p,&x ); /* copy invisible buffer back onto screen */
}





