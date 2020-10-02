
/*
HEADER:         CUG999.03;
TITLE:          SE (nee E) screen editor
DATE:           5/19-87;

DESCRIPTION:    "Text buffer editing routines for se"
VERSION:        1.00;
SYSTEM:         MS-DOS;
FILENAME:       EDIT.C;
SEE-ALSO:       SE.C
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
*/

/*
     e/qed/ged/se screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    FUNCTIONS: movechar, moveline, dojump, jumpline,
               movepage, moveword, insertchar, replchar, deletechar,
               deleteword, linedelete, crdelete, crinsert, adjustc, sync


*/

/* Earlier versions had the capabiltiy of editing text containing embedded
 * tabs.  In that case the character index into text[] is less in magnitude
 * than cursorx.  Due partly to the addition of new functions, charn and
 * cursorx are used interchangably in this version.  This version converts
 * tabs to spaces at the time the file is read.  The capability of editing
 * tabs might be restored as an option in a future version, but probably a
 * better way is to do an automatic entab at disc write time for those who
 * want tabs.  The only advantage of embedded tabs is that they save disc
 * space.  They mess up everything else.  The keyboard tab key is a cursor
 * positioning command in this version.  It does not store anyting.     g.o.
 */

#include <stdio.h>
#include <ctype.h>
#include "ged.h"

/* move cursor by 'move' columns to the right return YES unless going off text
 */
movechar(move)
int  move;
{
    int  cp, len, result, j;

    cp = charn + move;
    result = YES;

    if ( cp < 0 )  {
        if (cline > 1) {
            charn = strlen(getline(cline-1));
            moveline(-1);
        }
    }

/* The "cursor right one char" command adds trailing spaces as needed to
 * provide free cursor movement.  New characters would be lost if preceeded
 * by a '\0'.
 */
    else if ( cp > ( len = strlen(text) ) )  {
        if(!trail && move == 1) {
            sync(cp);
            resetcursor();
            return YES;
        }
        else {
            charn = 0;
            result = moveline(1);
            return result;
        }
    }
    else {
        sync(cp);  /* move +/- one on same line */
        resetcursor();
        return result;
    }
}

/* calculate line number at the top and bottom of screen.  The lines in that
 * span are read from disc if not already in memory.
 *
 * It is convenient to be able to scroll
 * by page for a few pages then page back to the original line with the
 * cursor returning to its original position.  The program works
 * that way for page scrolls.
 */

calp()
{
    if (cursory < topline)
        cursory = topline;
    if ( (cursory - topline) > (cline - 1) )
        cursory = topline + (cline - 1);    /* near bof */
    pfirst = loc(cline + topline - cursory, 0);
    plast = loc(pfirst + SHEIGHT - topline, 0);
    if ( (cursory - topline) > (plast - pfirst) )
        calp(--cursory);  /* neaar eof */
    return;
}

/* Move cursor by '+/- move' lines, + is downward.
 * return YES if Ok, NO if going off text.
 *
 * Set 'cursory' to the preferred cursor y value before the call.  The
 * preferred location is not always be used.  There are conditions near
 * beginning and end of file when it cannot be used.  A scroll
 * placing the y cursor in the preferred positiion can be forced to
 * the extent possible by setting plast=-1 before the call.  The
 * preferred value will otherwise not be used if a one-line
 * scroll suffices, or if the new cursor position is already somewhere
 * on the existing display.  The minimization of scrolling is desirable
 * from a speed standpoint.  It also minimizes eye fatigue.
 *
 * The window movement is normally purely vertical, but there are
 * exceptions.  If 'charn' has recently changed then horizontal
 * display motion may result also.  If 'trail' is true then horizontal
 * motion will often result due to the differing line lengths if
 * the lines are long.  To cause horizontal adjstment on the new
 * line, set charn before the call to moveline().  It is unnecessary
 * to call sync().
 *
 * moveline() assumes that the existing physical display is correct and
 * current.  It can't be called to show the result of an editng operation.
 *
 * It is assumed that the scroll function is faster than a
 * rewrite, so scrolling is used when possible.  Good high-level portability
 * could be achieved by writing a low-level scroll function which might
 * actually do a rewrite when the hardware scroll function is missing.
 * The program is being restructured so that all hardware specific
 * display and terminal functions are in term.c.
 */

moveline(move)
int move;
{
    int line, i, oldoff, sav, ocline;

    puttext();

    ocline = cline;
    cline = loc(cline, move);  /* read from disc if necessary */
    move = cline - ocline;

    sav = curson(NO);
/* gettext adds trailing spaces if needed for free cursor motion */
    gettext(cline, charn);
    sync(charn);

    if (plast < 0)
        pfirst = -1;
    if (pfirst < 0)
        plast = -1;
    oldoff = lastoff;
    lastoff = calcoffset(charn);

    if (blocking && ( move != 1  && move != -1) )
        oldoff = -1;    /* force a complete rewrite if reverse field too complex */

    if (oldoff == lastoff && cline >= pfirst && cline <= plast ) {
/* the new cursor position is already on the screen */

        if (blocking)        /* the vacated line may become unmarked */
            putline(ocline, cursory, NULL);

        cursory = (cline - pfirst) + topline;
        calp();

        if (blocking)         /* the new line may become marked */
            putline(cline, cursory, NULL);

    }
/* scroll down and add one line at top */
/* the old cursor can be anywhere on the screen */
    else if (oldoff == lastoff && cline == pfirst -1) {
        if (blocking)  /* the vacated line may become unmarked */
            putline(ocline, cursory, NULL);

        scrolldown(topline);
        cursory = topline;
        putline(cline,topline,text);
        calp();
    }
/* scroll up and add one line at bottom */
    else if (oldoff == lastoff && cline == plast + 1) {
        if (blocking)  /* the vacated line may become unmarked */
            putline(ocline, cursory, NULL);

        scrollup(topline);
        cursory = SHEIGHT;
        putline(cline, SHEIGHT, text);
        calp();
    }
/* Vertical screen motion of 0 or  more than 1 line, or horiz scrolling. */
/* This case uses the preferred y cursor position if possible */
/* moves of 0 are used to re-establish valid environment */
    else  {
        calp();
        putpage();
    }
    putlineno(cline);  /* update line and column no. */
    resetpcursor();
    curson(sav);       /* ready to edit */
    return  YES;
}

dojump()
{
    int i;

    putmess( "[|+|/|-|]|line|, last |C|hange, |E|nd,  prior |J|ump, |M|ark;  |S|et mark");
    scans(ans,2);
    gotoxy(59,0);
    if ( (i = calcjmp()) > 0)
        jumpline(i - cline);
    return;
}
/* calculate jump to new line.
 * interpret the jump to current line (a NOP) as full screen refresh request.
 * Useful for debugging and from a remote location with transmission errors
 * or outages.
 */
calcjmp()
{
    int i, j, k, to;
    char far *jj;
    i = toupper(ans[0]);
    switch (i) {

    case '+':
        scans(ans,5);
        to = atoi( ans );
        if ( to == 0) {
            cleareop(0);
            putpage();
        }
        jmpto = cline + to;
        break;

    case '-':
        scans(ans,5);
        if ( (to = atoi( ans ) ) )
            jmpto = cline - to;
        break;

    case 'C':
        if (lastc < 1) {
            error("No lines have been changed");
            return cline;
        }
        cursory = topline + SHEIGHT/2;
        jmpto = lastc;  /* line last changed/deleted/inserted */
        break;

    case 'E':
        jmpto = lastl;
        cursory = SHEIGHT - (SHEIGHT - topline)/4;
        break;

    case 'J':
        cursory = topline + SHEIGHT/2;
        return jmpto;
        break;

    case 'M':
        if (linem1 == 0) {
            error("No cursor line was marked with S option");
            return cline;
        }
        else {
            cursory = topline + SHEIGHT/2;
            if (linem1 == cline) {
                if (linem2 > 0)
                    return linem2;
                else
                    return linem1;
            }
            if (linem2 == cline) {
                if (linem3 > 0)
                    return linem3;
                else
                    return linem1;
            }
            return linem1;
        }
        break;

    case 'S':
        linem3 = linem2;
        linem2 = linem1;
        linem1 = cline;   /* mark the current line */
        return cline;
        break;


    case 'W':
        cent();
        return -1;
        break;

    default:
        if ( ans[0] < ' ')
            return -1;

        if ( i >= '0' && i <= '9' ) {
            scans(ans+1,5);
            if ( to = atoi(ans)  ) {
                cursory = topline + SHEIGHT/2;
                jmpto = to;
            }
        }
        else {
            error("Bad parameter");
            return cline;
        }
    }
    return jmpto;
}

/* center window on cursor.  */
cent()
{
    int i,j,k;

    cursory = topline + (SHEIGHT-topline)/2;
    calp();
    offset = cursorx - SWIDTH/2;
    if (offset < 0)
        offset = 0;
/* in centering, don't move right beyond line ends in order to show as
   much text as possible */
    j = 0;
    for(i = pfirst; i <= plast; i++) {
        if (i == cline && altered)
            k = strlen(text);
        else
            k = strlen(getline(i));
        j = ( ( k > j) ? k : j );
    }
    if (j < (offset + SWIDTH) )
        offset =  j - SWIDTH;
    if (offset < 0)
        offset = 0;

    lastoff = -1;
    resetcursor();

    return;
}

/* move current line by move lines down,
 */
jumpline(move)
int move;
{

    puttext();

/* the old cursor position is mostly irrevlelant.  The following
 * avoids excessive horizontal scrolling.
 */
    charn = offset + (offset>0);
    moveline( move );

    return  YES;
}


/* move current line by a page down (dir==0) or up (-1)
 */
movepage(dir)
int dir;
{
    int move, i;

    puttext();

    move = SHEIGHT -topline +1 - PAGEOVERLAP;
    if (dir)
        move = -move;

/* the scroll distance is less than the page height, so a move of
 * 'move' downward from the top line would still be on the screen
 * and no scroll would occur.  force a scroll.
 */
    plast = -1;
    moveline(move);  /* use the existing y cursor */

/* the screen has been updated.  if the disc file is being used then
 * prepare the page containing the next screen for quick access.  In this
 * way the disc access usually occurs after the operator command rather
 * than before it's completion.
 */
    if (move > 0)
        i = plast + SHEIGHT - PAGEOVERLAP;
    else
        i = pfirst - SHEIGHT + PAGEOVERLAP;
    getline(i);

    return;
}


/* move 1 word to the right (move -ve: left)
 */
moveword(move)
int move;
{

    if ( charn + move < 0 )  {
        if (cline > 1) {
            puttext();
            charn = strlen(getline(cline-1));
            moveline(-1);
        }
        else {
            return;
        }
    }
    else if ( charn + move >= strlen(text) )  {
        puttext();
        charn = 0;
        moveline(1);
        if ( inword(text[0]) )
            return;
    }

    while (( move<0 || text[charn]) && inword(text[charn]) && (charn += move))
        ;
    while (( move<0 || text[charn]) && !inword(text[charn]) && (charn += move))
        ;
    if ( move < 0  &&  charn )  {
        while ( inword(text[charn])  &&  --charn )
            ;
        if ( charn || !inword(text[charn]) )  charn++;
    }
    sync( charn );
    resetcursor();
}

/* replace char at cursor position.
   The last character is at text[LLIM-2].  The latest possible
   '\0' is at text[LLIM-1]
 */
replchar(c)
char c;
{
    if ( charn >= LLIM-1 ) {
        error(" Line would be too long ");
        inbufp = 0;
    }
    else {
        if(text[charn] == '\0')
            text[charn+1] = '\0';
        text[charn] = c;
        altered = YES;
        sync( charn + 1 );
        rewrite( charn-1 );  /* updated charn */
    }
    return;
}

/* inserts 'c' at charn, moves cursor up one

   The last character is at text[LLIM-2].  The latest possible
   '\0' is at text[LLIM-1]
 */
insertchar(c)
char c;
{
    int  cp;

    cp = strlen(text);
    if ( cp >= LLIM)
        cerr(20);
    if ( charn > cp)
        cerr(21);

/* prevent line overflow due to trailing spaces */
    if (!trail && text[LLIM-2] == ' ')
        text[LLIM-2] = '\0';

    cp += 1;    /* 1 <=  cp  <= LLIM */
    if ( cp >= LLIM ) {
        error(" Line would be too long ");
/* empy the input buffer to prevent multiple error occurrences if the
   key is held down  */
        inbufp = 0;
    }
    else  {
        for ( ; cp > charn; cp-- )
            text[cp] = text[cp-1];
        text[charn] = c;
        altered = YES;
        sync( charn+1 );
        rewrite( charn-1 );   /* updated charn */
    }
}


/* deletes char before (dir=-1) or at (dir=0) cursor.
 * NOP if charn = 0.
 */
deletechar(dir)
int dir;
{
    char c;
    int  cp;

    cp = charn + dir;
    if ( cp < 0 )
        return;
    else if ( text[cp] == '\0' ) {  /* can't be dir = -1 */
        crdelete();  /* case of cline == lastl handeled by crdelete() */
    }
    else  {
        do  {
            c = text[cp] = text[cp+1];
            cp++;
        }
        while(c);

        altered = YES;
        sync( charn + dir );
        rewrite( charn );  /* updated charn */
    }
}

/* line delete */

linedelete()
{
    int i;

    puttext();
    if (lastl == 1) {
        altered = YES;
        text[0] = '\0';  /* keep a seed */
        sync(0);
        rewrite(0);
    }
    else {
        scrollup(cursory);      /* always clears bottom line */
        if (loc(plast,1) > plast )
            putline( plast+1, SHEIGHT, NULL );   /* add new line at bottom */
/* The screen is updated.  Now do the slow work */
        i = cline;
        if (cline == lastl) {
            cursory -=1;
            cline -= 1;
            putlineno(cline);
        }
        resetpcursor();
        deltp(i, 1);
        calp();
        gettext(cline, charn);
        sync(charn);
    }
/* do a kludged push on the text buffer.  see pop() */
    stkptr = histptr;
    stkcnt = histcnt;
    return;
}


/* Concatenate lines.  Following line is appended to the current line
 * at the cursor position.  The cursor position may be beyond the last
 * visible character if !trail.
 */
crdelete()
{
    int  len, err, *t;
    char textb[LLIM];

    puttext();
    if (text[0] == 0) {
        linedelete();  /* crdelete would work but linedelete looks better for undo */
    }
    else {
        err = 0;
        if (cline < loc(cline,1))
            strcpy( textb, getline(cline+1) );
        else
            return;   /* cline == lastl */

/* have to rewrite first if a right marker will be added */
        if (cursorx - offset == SWIDTH) {
            offset += 1;
            resetcursor();
        }
        len = strlen(text);
        if ( len + strlen(textb) + 1 > LLIM )  {
            textb[(LLIM-1)-len] = '\0';
            error1(" Line too long - cut short ");
            err = 1;
        }

        strcat( text, textb );
        altered = YES;
        puttext();
        putline(cline, cursory, text);
        if (cursory < SHEIGHT) {
            scrollup(cursory+1);             /* clears bottom line */
            if (loc(plast,1) > plast )
                putline( plast+1, SHEIGHT, NULL );   /* add new line at bottom */
        }
        resetcursor();
/* The screen is updated.  Now do the slow work */
        deltp( cline+1, 1 );
        calp();   /* lastl has changed, which can change plast */

/* hold off the status line if line truncated */
        while( (err) && chkbuf() == 0)
            ;
    }
    return;
}


/* deletes up to first occurrence of white space.
 * if no white space then delete to end of line
 * see inword() in ged7.c
 * deletes first space after word as part of the word.
 */
deleteword()
{
    int  pend, cp, in;
    char c;

    for ( in = inword( text[pend = charn] );
      ( c = text[pend] )  &&  ( in ? inword(c): !inword(c) );
      pend++ )
            ;
    if (text[pend] == ' ')
        pend++;
    for ( cp = charn; ( text[cp] = text[pend] ); pend++, cp++ )
        ;
    rewrite( charn );
    altered = YES;
}


/* line break.
 * In mode 0 the portion after the cursor is moved down one line, creating a
 * new line.  The cursor is positioned on the new line unless it was
 * initially in position 0, in which case it remains on the original line
 * in preparation for the entry of a new line.
 * However, if if the cursor is in column 1 of a null line
 * then a new null line is created and the cursor moves
 * down one.  These rules usually accomplish what is wanted with a mimimum
 * of keystrokes.
 *
 * In mode 1 (^N) the cursor always stays on the initial line.  ^N will
 * be used in a future version for sentence push/delete.
 *
 * The injection is done after the screen refresh because it is slow
 * for large documents
 *
 * cursory, cline, and scroll() have to be used with internal consistantly
 * if movline() is not used for linejumps.  calp() must be called if lastl
 * changes due to injection or deletion because cursory and cline have
 * relationships to pfirst and plast.
 */

crinsert(mode)
{
    char textb[LLIM], c;
    int  i, iline, charnb, ocharn, ncline, ocline, ch0;

/* the next steps produce the best reverse-field appearance with undo */
    if (charn != 0)
        altered = YES;

    puttext();
    curson(NO);
    ncline = ocline = cline;
    ocharn = charn;

    charnb = 0;
    if (autoin) {
        for ( ; text[charnb] == ' '; charnb++)
            textb[charnb] = ' ';
        if (charnb > charn)
            charnb = 0;
    }

    strcpy( &textb[charnb], &text[charn] );  /* can be a null line */
    ch0 = text[0];
    text[charn] = '\0';

    if (mode == 0 ) {
        sync(charnb);   /* cursor moves for <ret>, not for ^N
/* horizontal scroll may occur if offset not 0.  may have to rewrite old text first.*/
        if (offset > 0)
            resetcursor();
    }

    if (cursory == SHEIGHT) {
        cursory--;
        scrollup(topline); /* make room for the second half of the split line */
    }
    else
        scrolldown(cursory+1); /* make rooom for the second half */

    putline(cline, cursory, text);
    putline(cline+1, cursory+1, textb);
    if (mode == 0) {
        if (ocharn == 0 && ch0 != 0)
            i = 0;  /* a convenience feature */
        else
            i = 1;
        ncline += i;
        cline = ncline;   /* for consistancy check in resetcursor() */
/* calp() can't be called until after inject() */
        cursory += i;
        putlineno(ncline);
    }
    sync(charn);
    resetpcursor();
    curson(YES);
/* The screen is updated.  Now do the slow work */

/* undo looks better if charn==0 is made a special case */
    if (ocharn == 0) {
        inject (ocline-1, text);
    }
    else {
        cline = ocline;
        altered = YES;
        puttext();              /* left portion of line.  uses 'cline' */
        inject(cline,textb);    /* right portion */
        cline = ncline;
        gettext(cline, charn);         /* get the correct data in text[] */
    }
    calp();           /* lastl has increased */
    return;
}


/* set cursorx to col. cursor nearest to col. 'x' so that
   cursor isn't in the middle of a tab or off the end of the
   current line  +++ obsolete ?
 */
adjustc(x)
int x;
{
    char c;

    for( charn = 0, cursorx = 0;
     cursorx < x  && ( c = text[charn] );
     charn++, cursorx++ )
            if ( c == '\t' )
            cursorx += tabwidth-1-(cursorx % tabwidth);
}


/* put cursorx and charn onto character 'cp' of current line.
 */

sync(cp)
int cp;
{
    int i;

    if (cp < 0)
        cp = 0;
    if (cp > (LLIM-1) )
        cp = LLIM-1;   /* last possible '\0' is at text[LLIM-1] */
    pad(cp);
    for( charn = 0, cursorx = 0; charn < cp; cursorx++, charn++ ) {
        if ( text[charn] == '\t' )
            cursorx = cursorx+tabwidth-1-(cursorx % tabwidth);
    }
    return;
}

/* Add trailing spaces if needed.
 *
 * When a new line is selected it is extended with spaces if necessary
 * in order for the cursor to stay in its old x position.  The cursor
 * can move anywhere.  The spaces are removed before the line is stored.
 *
 * If trail is TRUE then the spaces are not added because in that case
 * trailing spaces in the input file or those added in editing are
 * permanently retained.  In this mode the cursor can't be moved beyond
 * the rightmost character of a line.
 */
pad(cp)
int cp;
{
    int i;

    if( !trail ) {
        i = trim(text);   /* 0  <=  i  <=  LLIM-1  */
        if( cp > i ) {
            for (  ; i < cp; i++)
                text[i] = ' ';
            text[cp] = '\0';
        }
    }
    return;
}


