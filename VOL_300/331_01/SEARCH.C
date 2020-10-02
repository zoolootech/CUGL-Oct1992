/*
HEADER:         CUG999.04;
TITLE:          GED (nee QED) screen editor -- part 4;
DATE:           10/08/86;

DESCRIPTION:    "Find, alter, and repeat commands for the GED editor.";
KEYWORDS:       find, alter, repeat, pattern matching, search, replace;
FILENAME:       GED4.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      DeSmet C;
*/

/*
     e/qed/ged  screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle

    FILE:      ged4.c

    FUNCTIONS: info, findorrep, dofindrep, find

    PURPOSE:   perform find, alter and repeat commands

*/


#include <stdio.h>
#include "ged.h"

/* ^Q quick key services */

char spatt[81];
char changeto[81];

info()
{
    unsigned char c;

    puttext();
    putmess("|F|ind, |A|lter, |K|ontext/files, |P|aragraph");
    while ( (c = getlow()) != 'a' && c != 'f' && c != 'k' &&
        c != 'p' && c != ESCKEY );
    if (c == ESCKEY)
        return;
    switch (c)  {
    case 'f':
        findorrep(0);
        break;
    case 'a':
        findorrep(1);
        break;
/* the wordstar ^QY clear to end of line command.  disabled here because
 * this version uses ^Y to delete from cursor to end of line
 *
 *  case 'y':
 *      cleareol(cursorx,cursory);
 *      altered = YES;
 *      text[charn] = '\0';
 *      break;
 */
    case 'k':
        envir();
        break;
    case 'p':
        putmess("(F1 = dot help)  Delete dot commands while reforming?  (Y/N)");
        while ( (c = getlow()) != 'y' && c != 'n' && c != ESCKEY && c != F1KEY);
        if (c == ESCKEY)
            return;
        if (c == F1KEY) {
            helpdot();
            return;
        }
        if (c == 'y')
            roff(0);
        else
            roff(1);
        break;
    }
}


/* initiate string search or search and replace.  Called by F2 key
 * with mode = 0 or from above.
 * F2 str F3  search back
 * F2 str F4  search fwd
 * F2 str1 F2 str2 F3 replace backward
 * F2 str1 F2 str2 F4 replace forward
 * F3 & F4 & ^L  resume
 */
findorrep(mode)
int mode;
{
    int i, cnt;
    int c;
    char tpat[80];
    puttext();

    putmess("|Find|? ");
    c = scans(tpat,80);
    if ( c == ESCKEY || !tpat[0] )
        return;
    strcpy(spatt, tpat);  /* move valid string to permanent storage */
    if (c == F2KEY)
        replace = YES;
    else
        replace = mode;

    if ( replace )  {
        putmess("Alter to? ");
        c = scans(changeto,80);  /* null replacemente allowed */
        if (!ctrl) {
            if (c == ESCKEY)
                return;     /* no escape if <esc> can be embedded */
            for (i = 0; (changeto[i]); i++) {
                if (changeto[i] < ' ') {
                    error("Use -C option to embed ctrl codes (F7)");
                    return;
                }
            }
        }
    }
    else {
        changeto[0] = '\0';
    }

/* Global is the only option because this would be an awkward way to
 * replace one word.
 * To do the entire document it is necessary to use the <home> key first
 * to jump to line 1.
 * It is planned to add a wrap at end of file option, probably as the default.
 */

/*defaults */
    nocheck = NO;
    cnt  = 0;
    findir = 1;

    if ( c == F3KEY) {
        findir = -1;
    }
    else if (c == F4KEY) {
        findir = 1;
    }
/* string terminated by <ret> */
    else {
        if ( replace )
            putmess("|B|ackward,  |W|ithout asking, |count| ");
        else
            putmess("|<ret>|, |B|ackwards, |count| ");
        if ( scans(opts,5) == ESCKEY )
            return;

        for ( i = 0; ( c = opts[i]); i++ )  {
            switch(tolower(c)) {
            case 'b':
                findir = -1;
                break;
            case 'w':
                nocheck = YES;
                break;
            default:
                if ( c >= '0' && c <= '9' )
                    cnt = cnt*10+c-'0';
            }
        }
    }
    if ( cnt == 0 )  {
        if (replace)
            cnt = -1; /* unlimited */
        else
            cnt = 1;
    }
    dofindrep(cnt,findir);
}



/* Resume a search or search and replace
 * Called by F4, F5, ^L, and from above.
 * All searchs and search/replace operations wrap at beginning and end of file.
 * The first possible match is the first char beyond
 * the cursor position.  The last possible match is at the initial
 * cursor position.
 */
int ncline, count, oldlen, newlen;

dofindrep(count1,dir)
int count1,dir;
{
    int i, j, l2, s0, ocharn, ocline, ncharn;
    char c;
    char buf[81+35];


    puttext();
    count = count1;
    findir = dir;  /* F2 and F3 keys permanently change directin */
    oldlen = strlen(spatt);
    ocline = cline;
    ncline = cline;
    ocharn = charn;
    ncharn = charn;
    if (findir > 0) {
        newlen = 1;    /* don't start at the cursor */

        putmess1("    ", 28, 5);
        while (i = find(charn+newlen, cline, LLIM, lastl)) {
            if (i < 0)
                return;
            if (!dorep())
                return;
            ncline = cline;
            ncharn = charn;
        }
        putmess1("EOF", 28, 5);
        wait(1);
        puttext();
        charn = newlen = 0;
        cline = 1;
        plast = -1;
        gettext(cline, 0);
        while (i = find(charn+newlen, cline, ocharn, ocline)) {
            if (i < 0)
                return;
            s0 = strlen(text);
            if (!dorep())
                return;
            if (cline == ocline)
                ocharn += strlen(text) - s0;
            ncline = cline;
            ncharn = charn;
        }
    }
    else {
        s0 = strlen(text);
        while (i = find(charn-1, cline, 0, cline)) {
            if (i < 0)
                return;
            if (!dorep())
                return;
            ncline = cline;
            ncharn = charn;
        }
        ocharn += strlen(text) - s0; /* terminal point changes with replacemets */
        charn = 0;
        putmess1("    ", 28, 5);
        while (i = find(charn-1, cline, 0, 1)) {
            if (i < 0)
                return;
            if (!dorep())
                return;
            ncline = cline;
            ncharn = charn;
        }
        puttext();
        putmess1("BOF", 28, 5);
        wait(1);
        cline = lastl;
        plast = -1;
        gettext(cline, 0);
        charn=strlen(text);
        while (i = find(charn-1, cline, ocharn, ocline)) {
            if (i < 0) {
                return;
            }
            if (!dorep())
                return;
            ncline = cline;
            ncharn = charn;
        }
    }
    strcpy(buf,"Search for '");
    strcat(buf,spatt);
    strcat(buf,"' fails");
    error1(buf);
    wait(2);
    putstatusline(ncline);
    charn = ncharn;
    moveline(ncline-cline);
    curson(YES);
    return;
}

int dorep()
{
    int i, j;
    char c;

    if (!replace)
        return 0;


    if ( nocheck )
        c = 'y';
    else  {
        blankedmess = YES;
        putlineno(cline);
        putmess1("Replace  |<esc>|/|Y|/|N| ?", 34, 37);
        do {
            gotoxy(55,0);  /* position just after prompt */
            for (i=0; i < 3000; i++);
            resetpcursor();
            for (i=0; i < 3000; i++);
        }
        while (chkbuf() == 0);

        c = testlow();
    }
    switch(c)  {
    case 'y' :
        newlen = strlen(changeto);   /* null replacement allowed */
        if ( strlen(text) + newlen - oldlen + 1 > LLIM )  {
            error(" Line would be too long ");
            return;
        }
/* delete the old word */
        for ( j = charn; (text[j] = text[j + oldlen]); j++ )
            ;

/* make room. move '\0' also, for strlen+1 moves */
        j = strlen(text);
        for (  ; j >= charn;  j-- )
            text[j+newlen]=text[j];

        j = charn;
        for ( i = 0; (c=changeto[i]); i++ )
            text[j++] = c;

/* the line is not stored immediately because there may be other changes
                   in it */
        altered = YES;
        rewrite( charn );
        sync(charn);
        break;
    default:   /* no */
        newlen = 1;
        break;
    case ESCKEY :
/* move off the string start so it will be picked up on a resume */
        movechar(-findir);
        error1(" Search stopped.  ^L, F3, F4 to resume. ");
        wait(1);
        return 0;
        break;
    }
    count--;
    if (count == -2)
        count = -1;   /* -1 = unlimited count */
    if (count = 0)
        return 0;
    else
        return 1;
}

/* find 'spatt', searching backwards ( findir==-1)
  or forwards (findir==1).  The search starts at text[cp].
  If cp is -1 and the search is backward the search begins at the
  end of the preceeding line.  If forward and off the end of the
  line the search begins on the following line.  Lines between line1
  and line2 are searched.

  Returns 1 if found, 0 if not found, -1 if aborted.

  No wild cards in this version.  When the wildcard option is added
  a different and slower search algorithm should be used, retaining
  this code if no wild cards are specified.  In that way performance
  is not lost for the normal case.

  On a 5 MHz IBM PC this version searches text of typical line length
  at 30,000 characters per second, provided that all of it is in RAM.
  This performance is important for large documents.
 */
int find(cp, line1, cp2, line2)      /* local function */
int cp, line1, cp2, line2;
{
    unsigned char testkey();
    int   i, j, k, m, fline, oldcharn, interupt, linecount;
    char  *s, pattch1, *p, *t, *padr, *getline();
    char *strstr();
    long jj;

/* the cursor is on the line being searched, which is not on the screen */
    curson(NO);
    fline = line1;
    oldcharn = charn;
    interupt = NO;
    linecount = cline % 100;
    pattch1 = spatt[0];

    if ( findir == 1 ) {
        if ( cp >= strlen(text) ) {
            fline++;
            cp = 0;
        }
/* first search from the cursor+1 to line end of the first line, then all
 * of the following lines.  The library routines are used where
 * possible because they are often faster.
 */

        while ( fline <= line2 )  {
/* return null if spatt not in line */
            if (fline == cline)
                p = text;
            else
                p = getline(fline);

            s = strstr(p+cp, spatt);
            if (!(s == NULL)) {
                cp = s-p;
                if (fline == line2 && cp > cp2)
                    goto fail;
                else
                    goto  foundit;
            }

/* Check for input key occasionally.  The search operation is slowed
 * excessively if this test is made on every line.  The test
 * is made on every replacement, though.
 */
            if ( (fline % 200) == 0 || fline == cline) {
                if (chkbuf() != 0) {
                    inbufp = 0;    /* discard the keystroke */
                    interupt = YES;
                    goto  interrupted;
                }
            }
            fline++;
            cp=0;  /* cp not always set by above loop */
        }
    }
/* search backward */
    else {
        if (cp < 0)
            fline--;

        while ( fline >= line2 )  {

/* do a fast forward test to find if the string is in the line,
 * then search all or a portion of the line backward.
 */
            if (fline == cline)
                padr = text;
            else
                padr = getline(fline);
            if (cp < 0)
                cp = strlen(padr) - 1;    /* -1 <= cp */
            t = strstr(padr, spatt);
            if (!(t == NULL)) {
                for ( s = padr; cp >= 0; cp--) {
                    if ( *( p = s+cp) == pattch1 )  {
                        for ( t = &spatt[1], p++; *t && (*p == *t); p++, t++ )
                            ;
                        if (*t == '\0') {
                            if (fline == line2 && cp < cp2)
                                goto fail;
                            else
                                goto  foundit;
                        }
                    }
                }
            }
            if (fline % 200 == 0 || fline == cline) {
                if (chkbuf() != 0) {
                    inbufp = 0;   /* discard the keystroke */
                    interupt = YES;
                    goto  interrupted;
                }
            }
            cp = -1;
            fline--;  /* do next line */
        }
/* line loop falls through.  not found. */
    }
fail:;
    return 0;

interrupted:;
    error1("Search aborted");
    wait(1);
    putstatusline(ncline);
    moveline(ncline-cline);
    curson(YES);
    return -1;

/* only one copy of the text buffer is stored below by moveline, regardless of
 * the number of changes in it.
 */

foundit:;
/* position the screen horizontally so both the new and old strings are
 * visible in context.
 */
    i = strlen(changeto);   /* newlen not current */
    k = (i > newlen) ? i : newlen;
    m = cp + k;

    if (cp < charn) {
        if (cp < SWIDTH-k)
            calcoffset(0);
        else
            calcoffset(cp-10);
    }
    else {
/* favor no offet.  if offset required anyway then show a little more */
        if (m > SWIDTH)
            m += 10;
        calcoffset(m);
    }

    if (fline != cline || plast < 0) {  /* scroll vertically if necessary */
        charn = 0;   /* required by a consistancy check in puttext() */
        puttext();     /* store the changes */
        if (fline > plast+2 || fline < pfirst - 2)
            cursory = topline + (SHEIGHT-topline)/2;  /* re-optimize cursor position if new text area */
        charn = cp;
        moveline(fline-cline);
    }
    else {
        sync(cp);  /* won't cause horz scroll */
        rewrite(0);
    }
    putstatusline(cline);
    blankedmess = NO;
    curson(YES);
    return  1;
}



