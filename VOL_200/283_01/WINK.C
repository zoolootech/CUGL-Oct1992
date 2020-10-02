/* wink.c -- 9/5/88, d.c.oshel
   */

#include "vidinit.h"
extern void setmchnloc( void );

void wink( char c )  /* window character out, obeys backspace and newline */
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
        if (c == 127)   /* destroy existing char on screen */
        {
            rptchar( SPC, 1 );
        }
        goto synch;
    } 

    else 
    {
        rptchar( c, 1 );
    }

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
