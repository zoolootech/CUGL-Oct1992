/*
HEADER:         CUG999.03;
TITLE:          GED (nee QED) screen editor -- part 3;
DATE:           5/15/87;

DESCRIPTION:   "Screen output functions.

KEYWORDS:       screen, output;
SYSTEM:         MS-DOS;
FILENAME:       paint.c;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn.
COMPILERS:      Microsoft 4.0
*/

/*
     e/qed/ged/se screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle

    FILE:      paint.c

    FUNCTIONS: putline, rewrite, putoffset, putstatusline, putlineno,
               putpage, putmess, unmess, putstr, calcoffset,
               resetcursor, setstatusname, error, error1, cerr, wait

    PURPOSE:   write text to screen
*/


#include <stdio.h>
#include "ged.h"


/* writes a line to the screen.   Uses the text address pointer in the third
 * argument instead of getline if the third argument is not NULL.
 *
 * On most systems the physical cursor will end at the end of the new
 * line.  It is not restored here because groups of lines are usually
 * written and it is more efficiet to repositon the cursor after the
 * group.
 */
putline(line,y,adr)
int  line, y;
char *adr;
{
    unsigned char  vbuf[SWIDTH+1];
    char *getline();
    int   bright, lastcol, off, i, j, x, sav;

    unsigned char  *p, c;
    int   cp, low, high;

    sav = curson(NO);
    vbuf[0] = '\0';


/* if block option and line in range  then reverse field */
    if ( blocking ) {

        if (vbord2 == 0)
            j = cline;
        else
            j = vbord2;

        if (vbord1 < j)
            i = vbord1;
        else {
            i = j;
            j = vbord1;
        }
        bright = (line >= i)  &&  (line <= j);
    }
    else {
        bright = NO;
    }

    if (adr == NULL) {
        if (altered && line == cline)
            cerr(32);  /* displayed data is obsolete */
        p = getline(line);
    }
    else
        p = adr;

    if (offset > 0) {
        lastcol = SWIDTH + offset;
        off = offset;
        if ( bright )  /* the markers are the opposite of the main color */
            colornorm();
        else
            colorblock();
        scr_aputch( 0, y, '<', ATTR2 );
        x = 1;
    }
    else {
        lastcol = SWIDTH;
        off = 0;
        x = 0;
    }

    if ( bright )
        colorblock();
    else
        colornorm();

    i = 0;
    for ( cp = 0; (*p)  &&  (cp < (lastcol)); p++, cp++ ) {
        if ( (off == 0) || (cp > off) )  {
            if ( *p >= 0 )      /* change limit to block control characters */
                vbuf[i++] = *p;
            else {
                if ( *p == '\t' ) {
                    do  {
                        if ( (off == 0) || (cp > off)  &&  (cp < (lastcol+x)) )
                            vbuf[i++] = ' ';
                    }
                    while ( (++cp % tabwidth) != 0 );
                    cp--;
                }
            }
        }
    }
    vbuf[i] = '\0';
    scr_putstr( x, y, vbuf, attr );

    if ( x + i < SWIDTH) {
        scr_delete( x + i, y );
    }
    else {
/*        if ( bright )
            colornorm();
        else
            colorblock();
  */
        c = *p;
        if (( c != '\0') && (cp == (lastcol))) {
            if ( *(++p) )
                scr_aputch( SWIDTH, y, '>', ATTR2 );
            else
                scr_aputch( SWIDTH, y, c, attr );
        }
        else {
            scr_aputch( SWIDTH, y, ' ', attr );
        }
    }
    colornorm();
    curson(sav);
    return;
}


/* rewrites current line from char 'cp', col 'x', onwards.  cp is the
 * character index, >= 0.  x is the virtual x cursor location.
 *
 * This routine could be called instead of rewrite to show line editing
 * if it is known that the new cursor will be on-screen.  The performance
 * improvement in doing so is insignificant.
 *
 * putline and this routine should be modified to have a greater commonality
 * than they do.
 */

rewrite1(cp,x)
int cp, x;
{
    unsigned char  vbuf[SWIDTH+1];
    int   h, i, j, k, begmark, sav;
    unsigned char  c;

    sav = curson(NO);
    vbuf[0] = '\0';
    h = i = j = k = 0;

    begmark = ( offset > 0 );
    if (begmark == 0)
        k = x;
    else {
        k = x - offset;
        if (k < 1)
            k = 1;
        scr_putstr( 0, cursory, "<", ATTR2 );
    }
    while ( x < SWIDTH+offset  &&  ( c = text[cp] ) )  {
        if ( c == '\t' )  {
            for ( i = tabwidth-x%tabwidth; i>0 && x<SWIDTH+offset-begmark; x++, i--)
                if ( x >= offset )
                    vbuf[j++] = ' ';
        }
/* change following limit to block control characters */
        else if ( (offset == 0) || ((x > offset)  &&  (c >= 0) ))
            vbuf[j++] = c;
        cp++;
        x++;
    }
    if (blocking)
        colorblock();
    else
        colornorm();

    vbuf[j] = '\0';
    scr_putstr( k, cursory, vbuf, attr );
    i = k + strlen(vbuf);
    if (i <= SWIDTH)
        scr_delete( i, cursory );

    c = text[cp];
    if ( (c > 0) && (x == SWIDTH+offset) ) {
        if(text[cp+1] == '\0' )
            scr_aputch( SWIDTH, cursory, c, attr );
        else
            scr_aputch( SWIDTH, cursory, '>' , ATTR2 );
    }
    curson(sav);
    return;
}


putoffset()
{
    gotoxy( 0, 0 );  /*?*/
    gotoxy( (offset>0), cursory );
}


putstatusline(line)
int line;
{
    int sav;
    char buf[50];

    sav = curson(NO);
    putlineno( line );
    strcpy(buf,justfile);
    strcat(buf,"                             ");
    buf[29]=0;   /* clear up to first time digit */
    scr_aputs( FNPOS, 0, buf, ATTR3 );

    show_time(1);
    curson(sav);
    blankedmess = NO;
    if (sav)
        resetpcursor();
    return;
}


/* uses scr_aputs to move the cursor else uspr won't work right.  call
 * resetpcursor after this call
 */
putlineno(line)
int line;
{
    int i, sav;

    if ( !displaypos )
        return;
    sav = curson(NO);
    scr_aputs( 0, 0, "       Line ", ATTR3 );

    displine = line;
    for ( i=5-uspr(displine, 0L, ATTR3); i > 0; i-- )
        scr_putch( ' ',ATTR3);

    scr_aputs( COLPOS-1, 0, " Col ", ATTR3 );

    dispcol = cursorx+1;
    for ( i=4-uspr(dispcol, 0L, ATTR3); i > 0; i-- )
        scr_putch( ' ',ATTR3 );
    if (charep) {
        scr_aputs(25,0,"   ",ATTR3);
        scr_aputs(28,0,"Over",ATTR2);
/* adjust space count to just before filename field */
        scr_aputs(32,0,"           ",ATTR3);
    }
    else
/* adjust space count to just before filename field */
        scr_aputs(25,0,"                  ",ATTR3);
    curson(sav);
    if (sav)
       resetpcursor();
    return;
}




/* exit help mode
 */
unmess()
{
    if (topline == 1)
        return;
    topline = 1;
    calp();
    putpage();
    return;
}

/* Write message to top line.  Uses all of line and clears the last
 * part.
 */
 putmess(s)
 char *s;
{
    int i;
    i = putmess1(s,0,SWIDTH+1);
    gotoxy(i+1,0);     /* position cursor for keyboard input */
    return;
}

/* write underlined message in status line.  Characters between || are
 * written highlighted.  xloc is the x cursor position of the first char.
 * width is the field width.  the trailing part if the field is cleared
 * if the message does not fill it.
 */
putmess1( s, xloc, width )
char *s;
int xloc, width;
{

    char c;
    int  i, cnt, norm;

    cnt = 0;
    gotoxy(abs(xloc), 0);
    norm = 0;
    color3();
    while( (c = *s++) ) {
        switch(c) {
        case '|':
            ( norm = ~norm ) ? color4() : color3();
            break;
        case '\n':
            break;
        default :
            scr_putch( c, attr );
            cnt += 1;
        }
    }
    color3();

    i = cnt;
    while (i++ < width)
        scr_putch( ' ', attr);
    colornorm();
    blankedmess = YES;
    return cnt;
}

/* Writes prompting/status messages to screen.  Characters between || are
 * written highlighted.
 */
int putstr(s)
char *s;
{
    int  norm;
    char c;

    norm = 0;
    colornorm();
    while( (c = *s++) ) {
        switch(c) {
        case '|':
            ( norm = ~norm ) ? color1() : colornorm();
            break;
        case '\n':
            putret();
            break;
        default :
            scr_putch( c, attr );
        }
    }
    colornorm();
    return;

}
/* insure that cursor is on screen with horizontal changes.  The argument is
 * the new cursorx that must be visible.  The algorithm uses hysteresis so
 * that horizontal scrolling only occurs when necessary.  If horizontal
 * scrolling gets behind the keyboard then the algorithm will jump ahead
 * twice as much as it finds itself behind.  The adaptive feature makes it
 * practical to scroll in increments of 1 on most display devices.
 */
calcoffset(x)
int x;
{
    int i, j;
    i = offset;

    if (x <= offset) {
        i = (x-1)/OFFWIDTH;
        i = i*OFFWIDTH;
        if (i<0)
            i=0;
    }
    j = strlen(text) > (offset+SWIDTH+1);  /*  1 if right marker needed */

    if (x > offset + SWIDTH -j) {
        i = x  - SWIDTH + j;
        i = i + (OFFWIDTH - 1);
        i = i - (i % OFFWIDTH);
    }

/* scroll horizontally in larger chunks if the program gets behind the
   keyboard
 */
    j = chkbuf();
    if (j > 0)
        while ( (j = chkbuf()) != chkbuf())  /* fill the char buffer */
            ;
    j = 2*j;  /* normally zero */
    if(j>20)
        j=20;
    if (i<offset)
        j=-j;
    if(i != offset)
        i=i+j;
    offset=i;
    if(offset<0)
        offset=0;
    return offset;
}

/* Display page.  Called only when a complete rewrite is necessary.
 * The help area and status line, as defined by topline, are unaffected,
 * except that the line and column displays are updated.
 */
putpage()
{
    int  i, j, y, line, sav;
    char c;
    sav = curson(NO);

/* The line number has normally changed when putpage is called */
    putlineno(cline);
    rw1();  /* calculate and hold offset */
    rw2();

    resetpcursor();
    curson(sav);
    return;
}


/* Page update.  Called after editing operations which affect screen
 * data and/or horizontal cursor position, but not involving a change
 * in cline.  The screen must already have been written,
 * as only changes are written.  The text buffer must contain
 * valid data.  Horizontal scrolling is performed as needed.
 *
 * Only the current line is scrolled horizontally if blockscroll is false.
 * That option is useful for slow remote terminals.  Operations which move the
 * cursor offscreen vertically will nevertheless result in a full
 * horizontal scroll when the entire page is eventually rewritten if
 * blockscroll is false.  The PC is fast enough that there is little
 * advantage to partial line updates.
 *
 * cp is the text[cp] index of the first character to be written.
 *
 * Call sync() first
 */

rewrite(cp)
int cp;
{
    if ( rw1() ) {
        if (blockscroll)
            rw2();         /* all text lines */
        else
            putline(cline, cursory, text);  /* all of cline */
    }
    else {
        rewrite1( cp, cursorx + (cp - charn) );  /* part of cline */
    }
    resetpcursor();  /* always onscreen at this point */
    return;
}

/* local function */
rw2()
{
    int line, y;

/* cline is put out in sequence with the rest for a good visual effect */
    for ( line = pfirst, y = topline; y <= SHEIGHT; line++, y++ ) {
        if ( line == cline ) {
            putline (line, y, text);
            if (y != cursory)
                cerr(33);
        }
        else if (line <= plast) {
            putline( line, y, NULL );
        }
        else {
            cleareol(0,y);
        }
    }
    return;
}


/* resetcursor() is used like rewrite, but is called when the screen change
 * is only a horizontal cursor position change.  The change may be
 * horizontally off-screen, in which case the entire screen is rewritten.
 * Call sync() first.
 *
 * moveline() handles vertical off-screen cursor movements, which may
 * also involve horizontal motion.
 */

resetcursor()
{

    if ( rw1() ) {
        if (blockscroll)
            rw2();         /* all text lines */
        else
            putline(cline, cursory, text);  /* all of cline */
    }
    resetpcursor();
    return;
}


/* reset physical cursor is a less general form of resetcursor.  It is used to
 * restore the cursor to the editing location when it is known that the
 * position is somewhere on the existing physical display.  That is
 * generally not true if the result of any operator command is being shown.
 */

resetpcursor()
{
    gotoxy( cursorx-offset, cursory );
    return;
}

/* local function */
rw1()
{
/* must insure screen current by putpage or otherwise if cline changes */
    if (displine != cline)
        cerr(34);  /* the text display may be wrong if no horz scroll occurs */

    if (dispcol != cursorx + 1) {
/* The column number has changed */
        putlineno(cline);
    }
/* compare offsets */
    if ( lastoff != calcoffset(cursorx) )  {
        lastoff = offset;
        return YES;  /* rewrite needed */
    }
    else {
        return NO;
    }
}



/* copy just filename w/o path for 'putstatusline' */

setstatusname()
{
    char *np, c;

    if ( filename[0] == '\0' )  {
        justfile[0] = '\0';
        return;
    }
    np = filename;
    while( *(np++) )     /* find end */
        ;
/* find last '\',if any  */
    while( ((c = *(--np)) != '\\')  &&  c != ':'  &&  (np != filename) )  {
        if ( c == '.' )  {      /* save any extension */
            if ( *(np+1) == '\0' )
                *(np) = '\0';
            strncpy(defext,np,4);  /* ".xxx\0"  */
        }
    }

    if ( !(*np) )  strcpy( justfile, filename );
    else  {
        memcpy(justfile, filename, 2 );
        strcpy( &justfile[2], ++np);
    }
}

/* Show an error message and wait.  The message kept up until the next
 * keystroke, then the key is used normally.  The purpose of the wait
 * is to prevent the status line from overwriting the message.  Use
 * error1 when the hold is not wanted.
 */
error(message)
char *message;
{
    int i;

    error1(message);
    i = chkbuf();
    while (chkbuf() == i)
        ;
    return;
}

/* Show an error and wait 3.0 to 4.0 seconds, or until a key is entered.
 */
errort(msg)
char *msg;
{

    error1(msg);
    wait(3);
    return;
}
/* Wait 'sec' seconds, or until a key is entered.  The amount of
 * delay is independent of the CPU speed.  The delay may be up to
 * one second more than specified.
 */
wait(sec)
int sec;
{
    int i, j, k, ck0;

    ck0 = chkbuf();

    i = gettime();
    for ( j = 0; j <= sec; j++) {
        while (i == (k = gettime())) {
            if (chkbuf() != ck0) {
                return;
            }
        }
        i = k;
    }
    return;
}

/* show an error message and return immediately.  used only when the
 * caller holds off the status line.  A call to hstart can be used
 * first to specify the first column number.  This call is good
 * for only one message.  The starting column is zero if the call is
 * not made.
 */

int bgin = 0;

hstart(col)
int col;
{
    bgin = col;
    return;
}
error1(message)
char *message;
{
    gotoxy(bgin,0);
    scr_aputs( bgin, 0, message, colorerr());
    cleareol( bgin+strlen(message), 0 );
    bgin = 0;
    blankedmess = YES;
    resetpcursor();
    colornorm();
    curson(YES);   /* always ready for operator input after an error */
    return;
}

/* Show an error message due to a coding error.
 */
cerr(errnum)
int(errnum);
{
    char buf[25];

    sprintf(buf, "Coding error %d", errnum);
    hstart(27);
    error(buf);
    return;
}
