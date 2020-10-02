/* scroll.c -- 9/5/88, 12/9/88, d.c.oshel
   */

#include "vidinit.h"


/* Scroll functions honor the current window, and do not alter global
** col & row settings.  The window scrolls up or down one line at a time.
**
** NOTICE:   The fresh, blank line which results from scrolling should
**           normally have the CURRENT video attribute, vid_attr.
**           If scrolling is caused by wputs(), this might mean that 
**           the "current" video attribute is whatever wputs() last found
**           as an attribute escape command, e.g. "^1", whereas the
**           window itself should be normal or reversed, etc.
**
**           You can explicitly request a scrolling attribute by setting
**           ScrollAttributeFlag to a vid array index, plus 1 to make
**           the flag non-zero.
**
**           For example, ScrollAttributeFlag <-- 3
**                        2 <-- index <-- (ScrollAttributeFlag-1)
**                        attr <-- vid[2] <-- vid[index]
**
**           However, if  ScrollAttributeFlag == 0
**                        attr <-- vid_attr, whatever is current
**
**           WindowBox() automatically sets this flag when a window
**           is created.  Restorescreen() resets it to use vid_attr.
*/

int ScrollAttributeFlag = 0;
static char errmsg[] = "\a\nCIAO: scroll: out of memory\n";

void scrolldn( void )  /* scroll current window down one line */ 
{
    int far *buffer;
    int i,j,num;
    int va;

    num = rm-lm+1;  /* number of columns in current window */

    buffer = (int far *) _fmalloc( (num * sizeof(int)) + 4 );
    if ( buffer == (int far *) NULL )
    {
        printf( errmsg );
        exit (39);
    }

    for ( i = bm, j = bm - 1; i > tm; i--,j-- )  
    {
    	MSJ_MovScrBuf( (char far *) buffer, j, lm, num, &video );
    	MSJ_MovBufScr( (char far *) buffer, i, lm, num, &video );
    }

    /* went down, so clear top line */
    va = ScrollAttributeFlag ? vid[ScrollAttributeFlag-1] : vid_attr;
    MSJ_SetFldAttr( SPC, tm, lm, va, num, &video );

    _ffree(buffer);
}


void scrollup( void )  /* scroll current window up one line */
{
    int far *buffer;
    int i, j, num;
    int va;

    num = rm-lm+1;  /* number of columns in current window */

    buffer = (int far *) _fmalloc( (num * sizeof(int)) + 4 );
    if ( buffer == (int far *) NULL )
    {
        printf( errmsg );
        exit (40);
    }
    
    for ( i = tm, j = tm + 1; i < bm; i++,j++ )  
    {
    	MSJ_MovScrBuf( (char far *) buffer, j, lm, num, &video );
    	MSJ_MovBufScr( (char far *) buffer, i, lm, num, &video );
    }

    /* went up, so clear bottom line */
    va = ScrollAttributeFlag ? vid[ScrollAttributeFlag-1] : vid_attr;
    MSJ_SetFldAttr( SPC, bm, lm, va, num, &video );

    _ffree(buffer);
}


