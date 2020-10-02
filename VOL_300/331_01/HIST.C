/*
HEADER:         CUG999.11;
TITLE:          GED (nee QED) screen editor -- part 2;
DATE:           12/19/86;

DESCRIPTION:   "Text changing routines for the GED editor. Eg,
                movechar, insertchar.";
SYSTEM:         MS-DOS;
FILENAME:       GED2.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      DeSmet C;
*/

/*
     e/qed/ged screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle

    FILE:      hist.c

    FUNCTIONS: undo, pop

    PURPOSE:   perform text changing commands

*/


#include <stdio.h>
#include <ctype.h>
#include "ged.h"


/* undoes edits on current line and then from history
 */
undo()
{
    int  l, inicnt, c;

    puttext();
    if ( histcnt == 0 )  {
        error( " Nothing to undo " );
        return;
    }
    inicnt = histcnt;
    c = OOPSKEY;            /* do the first one for free */

    do {
        vbord1 = MAXINT;
        vbord2 = 0;
        blocking = NO;

        switch (c) {
        case OOPSKEY:
        case '-':        /* ctrl key not requeired */
        case '_':
            if (histcnt > 0) {
                do {
                l = undoredo(-1);    /* pre decrements histptr */
                }
                while ( histcnt > 0  &&  history[histptr].histcomm ==
                history[ (histptr-1 >= 0) ? histptr-1 : HISTLEN-1 ].histcomm );

                hshow(l);

            }
            else {
                error("Can't undo more.  Redo or escape.  Escape obliterates redo.");
            }
            break;

        case '=':
        case '+':  /* same key */
            if (histcnt < inicnt) {
                do {
                l = undoredo(1);   /* post increments histptr */
                }
                while ( histcnt < inicnt  &&  history[histptr].histcomm ==
                history[ (histptr-1 >= 0) ? histptr-1 : HISTLEN-1 ].histcomm );

                hshow(l);
            }
            else {
                error("Can't redo future.  Now in edit mode.");
                resetpcursor();
                while (chkbuf() == 0)
                    ;
                goto leave;
            }
            break;

        default:
            error("Bad key.  ^- to undo more,  + or = to redo,  <esc> to resume editing");
            break;
        }
    }
    while ( (c = getkey()) != ESCKEY);

leave:;
    blocking = NO;
    putpage();
    blankedmess = YES;
    return;
}


/* local function */

hshow(l)
int l;
{
    int i;

/* leave room for the line number. not an error, but show in reverse field */
    hstart(FNPOS);
    error1(" - undo, +/= redo; or <esc> ");
    show_time(1);

    if (vbord2 >= vbord1)
        blocking=TRUE;

/* moveline requires that the existing physical display agree with memory.
 * They aren't the same at this point if lines within the active display
 * area have been changed.  A complete rewrite is necessary.
 */
    charn = offset + (offset>0);
/* the preferred cursor position is the one that does not cause scrolling */
    cursory += l - cline;
/* re-optimize the cursor if the new location is off-screen */
    if (cursory > SHEIGHT || cursory < topline)
        cursory = topline + (SHEIGHT - topline)/2;
/* but it is more important to see the whole block */
    i = cline;
    cline = l;
    calp();
    cline = i;
    if (vbord2 >= plast)
        cursory += plast - vbord2 -1;  /* limit checked in calp() */

    plast = -1;
    moveline(l-cline);
    return;
}
/* --------------------------------------- */
/* local function
 *  direc = 1 to redo, -1 to undo.
 */
int undoredo(direc)
int direc;
{
    int  l, hpage, hoff;

    if (direc < 0) {   /* post increment for redo */
        if ( --histptr < 0 )
            histptr = ( HISTLEN - 1 );
        histcnt--;
    }

    storehist = NO;

    l = history[histptr].histline;
    hpage = history[histptr].histp.page;
    hoff = history[histptr].histp.moffset;

/* the following comments apply to an undo step.  The same code is executed
 * for the redo, but then the undo/redo roles change.  The function is
 * fully symmetrical between redo/undo, and except for a wasted byte
 * in the inject() is indefinately reversible.
 */

    switch ( history[histptr].histtype )  {

    case HISTINSERT:
        history[histptr].histp.moffset = tp[l].moffset;  /* prepare for redo */
        history[histptr].histp.page = tp[l].page; /* prepare for redo */
        history[histptr].histtype = HISTDELETE;   /* prepare for redo */
        deltp(l,1);                               /* undo */
        if (l <= vbord2)
            vbord2--;
        break;
    case HISTDELETE:
        inject(l-1,"");                         /* undo */
        tp[l].moffset = hoff;                   /* undo */
        tp[l].page = hpage;                     /* undo */
        history[histptr].histtype = HISTINSERT; /* prepare for redo */
        if (vbord2 >= l)
            vbord2++;
        if (l > vbord2)
            vbord2 = l;
        if (l < vbord1)
            vbord1 = l;
        break;
    case HISTREPLACE:

        history[histptr].histp.moffset = tp[l].moffset; /* prepare for redo */
        history[histptr].histp.page = tp[l].page; /* prepare for redo */
        tp[l].page = hpage;                      /* undo */
        tp[l].moffset= hoff;                     /* undo */

        if (l > vbord2)
            vbord2 = l;
        if (l < vbord1)
            vbord1 = l;
        break;
    }
    if (direc > 0) {
        if (++histptr >= HISTLEN)
            histptr = 0;
        histcnt++;
    }

    return l;
}
/* --------------------------------------- */

/* pop the last deleted line from the history buffer.
 * This is a prelimenary implementation of stack operations and is
 * something of a kludge.  The stack needs its own table.  It can
 * use the text of the lines in the histroy buffer, though.
 * This "stack" will be overwritten after 100 editing operations.
 * Pops can be undone, so a different area of the data base used here
 * is modifed by addhistory() when a pop is performed.
 * Editing which occurs later than the pop can be undone without conflict,
 * provided there is not too much of it.
 */

pop(mode)
int mode;
{
    int  onpage;     /* flag for changes to current screen */
    int  l, slot;
    int h0, h1, h2;
    char *ptr;
    char *gethist();

    puttext();  /* must be done before the stack test */
    if ( stkcnt == 0 )  {
        error( " Stack empty " );
        return;
    }
    h0 = stkptr;
    h1 = stkcnt;
    h2 = 0;
    do {
        if ( --stkptr < 0 )
            stkptr = ( HISTLEN - 1 );
        stkcnt--;
        ptr = gethist(history[stkptr].histp.page,history[stkptr].histp.moffset);
        switch ( history[stkptr].histtype )  {
        case HISTREPLACE:
            break;
        case HISTINSERT:
            break;
        case HISTDELETE:
            h2 = 1;
            break;
        }

    }
    while ( !h2 && stkcnt &&  history[stkptr].histcomm ==
        history[( stkptr-1 < 0 ? HISTLEN-1 : stkptr-1) ].histcomm );
    if (mode == 1) {
        stkptr = h0;  /* copy and pop.  restore to original state. */
        stkcnt = h1;
    }
    if (!h2)
        error( " Stack empty " );
    else {
        vbord1=cline;
        vbord2=cline;
        blocking=TRUE;
        scrolldown(cursory);
        sync(offset + (offset>0));
        putline(cline, cursory, ptr);
        putlineno(cline);   /* new col no */
        resetpcursor();

/* do the slow work while waiting for a keystroke, otherwise the next
   operation is delayed
 */
        inject(cline-1,ptr);
        calp();  /* plast may have changed */
        while (chkbuf()==0)   /* wait for any key */
            ;

        blocking = FALSE;
        gettext( cline, charn );
        putline(cline, cursory, NULL);
    }
    return;

}

