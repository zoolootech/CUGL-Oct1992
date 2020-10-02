/*
HEADER:         CUG999.08;
DATE:           5/15/87;

DESCRIPTION:    "Text storage and manipulation routines for the GED editor.
                 Virtual memory interface";
KEYWORDS:       text storage, memory management, virtual storage, paging;
SYSTEM:         MS-DOS;
FILENAME:       VIRT2.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      Microsoft 4.0;
*/

/*
     e/qed/ged/se  screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle


    FUNCTIONS: loc, gettext, getline, inject, deltp, puttext,
               readtext, opentext, balloc,
               addhistory, trim

    PURPOSE:   get and put text lines into and out of storage




   The far and huge pointer definitions can be removed by changing
   the preprocessor directives in ged.h


*/

#include <stdio.h>
#include <ctype.h>
#include "ged.h"


int untrims;


/* returns line + move, adjusted to be within text.
 */
loc(line,move)
int line, move;
{
    int y, sav, i;

    if(charep) {
        i = move;
        if(i < 0)
            i = -i;
        if (i > 10) {
            charep = 0;  /* exit character replace mode if line changes a lot*/
            blankedmess = YES;  /* and change the header status indication */
        }
    }

    if ( ( y = line+move ) < 1 )
        y = 1;
    if ( y > lastl )
        return lastl;
    else
        return y;
}


/* makes 'line' the current line.
   Lines which appear on the screen cause the virtual memory page containing
   the line to be marked as recently used.  The global search operations do
   not call this routine.
 */
gettext(line, cp)
int line, cp;
{
    int i;
    char *getline();

    if (altered)
        cerr(80);   /* the text buffer was not stored with puttext */

    strcpy( text, getline( line ) );
    pad(cp);  /* add trailing spaces if cursor beyond end of line */

    cline = line;

    if (clock < (MAXINT-1) )
        clock++;
/* don't lower the priority of newpage */
    i = virtslot[ tp[line].page ];
    if (usage[i] < clock)
        usage[i] = clock;


    text[LLIM-1] = '\0';   /* for diagnostic checks only */
    return;
}

/* returns small memory model address of text of 'line' and updates
 * page usage.  The buffers used by getline and gettext have to be
 * different.  The extra text move for gettext has no significant effect
 * on program timing.  getline is used for the string search operatios
 * and needs to be fast.
 *
 * getline has to be used with caution because the page pointed to can
 * be swapped out by subsequent activites, invalidating the pointer.  That
 * problem does not exist in this version because the line is copied to
 * a local buffer to satisfy the mixed memory model requirements.
 * In this version the pointer is invalidated by a subsequent gettext().
 *
 * Requires that strings not cross 64 k boundaries.
 */

char glbuf[LLIM];  /* this buffer is shared by getline, gethist, & gettext */

char *getline(line)
int line;
{
    char FAR *hgetline();
    register char FAR *hptr;
    register char *s;

    hptr=hgetline(line);
    s=&glbuf[0];
/* The following is equivalent to the movesf() call.  movesf is in pcio.asm
 *  while (*s++ = *hptr++)
 *      ;
 */
    movesf(s,hptr);
    return &glbuf[0];
}

/* used by undo */
char *gethist(page,offset)
int page,offset;
{
    char HUGE *hptr;
    char *s;
    if(virtslot[page] < 0)
        swappin(page);
    hptr = slotaddr[virtslot[page]]+offset;
    s = &glbuf[0];
    while (*s++ = *hptr++)
        ;
    return &glbuf[0];
}
/* returns far address of text of 'line'
 * and updates page usage.  Requires that 2<<16 % pagesize == 0 to
 * avoid crosssing a 64 k boundary.
 * the huge pointers are recast to far for consistancy and effieciency.
 */
/*
char FAR *hgetline(line)
int line;
{
    int pg;
    line = loc(line,0);
    pg = tp[line].page;
    if ( virtslot[pg] < 0 )
        swappin(pg);
    return  (char FAR *) slotaddr[virtslot[pg]] + tp[line].moffset;
}
*/
char FAR *hgetline(line)
int line;
{
    int pg;
    int i,j;

    line = loc(line,0);
    pg = tp[line].page;
    if ( virtslot[pg] < 0 )
        swappin(pg);

    i =  tp[line].moffset;
    if(i > 0) {
        j = *((char FAR *) slotaddr[virtslot[pg]] + tp[line].moffset -1);
        if(j != 0)
            cerr(84);
    }
    return  (char FAR *) slotaddr[virtslot[pg]] + tp[line].moffset;
}


/* Inserts 'txt' after 'line', moving following pointer array up.  Line 1
 * is injected at 0.
 *
 * See also comment in deltp.
 */
inject(line,txt)
int  line;
char *txt;
{
    int  l, balloc();
    int i, j, trims;
    char *s;
    char FAR *h;
    long FAR *ht;
    long FAR *hf;
    long int ii;


    trims = trim(txt);

    if (lastl > 16383)
        goto tomany;

    ii = (long) (lastl + 1) * sizeof(*tp);
    if ( (ii/PAGESIZE) >= tpslots) {
/* need another slot to store tp's in */
        if ( tpslots == slotsinmem-2)
            goto tomany;
        if ( usage[tpslots] > 0 )
            swapout(tpslots);  /* bump for tp, which can't be swapped out */
        usage[tpslots++] = -1;
    }

    addhistory( line+1, line+1, HISTINSERT );

    if(line < lastl) {
        ht = (long int FAR *) &tp[lastl+1];
        hf = (long int FAR *) &tp[lastl];
        j = lastl-line;
        for (i = 0; i < j; i++)
            *ht-- = *hf--;
    }
    lastl++;

    tp[line+1].moffset = balloc(1+trims);  /* increments newpage if necessary */
    tp[line+1].page = newpage;
    h = slotaddr[virtslot[newpage]] + tp[line+1].moffset;
    s = &txt[0];
    while (*h++ = *s++)
        ;
    stale(newpage);
    untrim();

/* keep the default jump location on the same physical line */
    if ( line <= jmpto)
        jmpto++;
/* keep the marked jump locations on the same physical line */
    if ( line <= linem1)
        linem1++;
    if ( line <= linem2)
        linem2++;
    if ( line <= linem3)
        linem3++;
/* rember the last change for the jump command. never needs adjustment. */
    lastc = line+1;

    return  line+1;

tomany:;
    error(" Too many lines for RAM size.  Line lost. ");
    return  FAIL;
}


/* delete line by shifting pointers

   The tp structures must have the same size as a long integer for
   this routine to work.  Execution time is excessive for very large
   documents if the shortcut is not used.
 */
deltp(dline, cnt)
int dline, cnt;
{
    int i, j, lastls;
    long FAR *ht;
    long FAR *hf;

    lastls = lastl;
    for (i = dline; i < dline+cnt; i++) {
        addhistory( i, dline, HISTDELETE );    /* save for undo */
        lastl--;
        if ( lastl < 1 )
            lastl = 1;
        if (i < jmpto)
            jmpto--;

        if (i < linem1)
            linem1--;
        if (i < linem2)
            linem2--;
        if (i < linem3)
            linem3--;

        lastc = i;
    }
    ht = (long FAR *) &tp[dline];
    hf = (long FAR *) &tp[dline+cnt];
    j = lastls - dline;
    for (i = 0; i < j; i++)
        *ht++ = *hf++;

    return;
}

/* replaces cline's text if it has been altered.  the new text goes to
 * a newly allocated region.  the old text remains as is for the undo.
 */
puttext()
{
    int balloc();
    char *s;
    char FAR *h;
    int tsize;


    if (text[LLIM-1] != '\0')
        cerr(81);

    if ( altered )  {
        tsize = trim(text);   /* string restored before exit */
        if (charn > untrims)
            cerr(82);         /* trailing spaces lost */
        addhistory( cline, cline, HISTREPLACE );   /* add for undo */
        altered = NO;

        tp[cline].moffset = balloc(1+tsize);   /* increments newpage if necessary */
        tp[cline].page = newpage;
        h = slotaddr[virtslot[newpage]] + tp[cline].moffset;
        s = &text[0];
        while (*h++ = *s++)
            ;
        stale(newpage);
        untrim();
/* remember the last change for the jump command */
        lastc = cline;
    }
    return;
}

/* mark the disc copy of the page obsolete and recycle the disc slot */
stale(page)
int page;
{
    int i;

    i = virtslot[page];
    if ( i < 0 || i >= slotsinmem)
        cerr(83);       /* table corrupt */
    if (auxloc[i] < 0) {
        dskslots[1 -auxloc[i]] = 0;
        auxloc[i] = 0;
    }
    return;
}

/* allocate and return the offset in newpage of a vector size n.
 */
int balloc(n)
unsigned n;
{

    if (virtslot[newpage] < 0)
        swappin(newpage);
    if ( allocp  + n >= PAGESIZE )  {

/* no room in current newpage; get another.
 */
        usage[virtslot[newpage]] = clock;  /* normal priority */
        virtslot[++newpage] = freememslot();
        usage[virtslot[newpage]] = MAXINT;  /* highest possible priority for residency */
        allocp = 0;
    }
    allocp += n;
    return  allocp-n;
}

addhistory(is,willbe,type)
int is, willbe;
int type;
{
    if ( !storehist )  return;

    history[histptr].histp.page    = tp[is].page;
    history[histptr].histp.moffset = tp[is].moffset;

    history[histptr].histline = willbe;
    history[histptr].histtype = type;
    history[histptr].histcomm = ncommand;

    histptr++;
    if ( histptr == HISTLEN )  histptr = 0;
    if ( histcnt < HISTLEN )   histcnt++;
    return;
}

/* eliminate trailing blanks and tabs if trail is false */
/* returns the string length after trimming */
char *untriml;
char untrimc;

int trim(string)
char *string;
{
    int i, l;

    untrims = strlen(string);
    if ( (!trail) && (untrims > 0) )  {
        for ( i = untrims-1; (i >= 0)  &&
        ( (string[i]==' ') || (string[i]=='\t') ); i-- )
                ;
        l = ++i;
    }
    else {
        l = untrims;
    }
    untriml = &string[l];
    untrimc = *untriml;
    *untriml = '\0';
    return l;
}
/* restore the string to its original state */
untrim()
{
    *untriml = untrimc;
    return;
}



/* Reads file being edited into virtual memory until 'line' is read, or eof.
 * If eof, sets lastl to number of last line read.  File is assumed to be
 * already open.
 *
 * ENDFILE is the ^Z end of file marker.  DFAIL is from egetc(), not from the
 * library.  These values are internal and do not depend on the compiler used.
 */
readtext(line)
int line;
{
    struct iobuffer  *iobuf;
    int nbytes;
    register unsigned char *t;
    register int c;
    static unsigned char *nextp;
    static int nleft;

    char txt[LLIM]; /* latest char at txt[LLIM-2], latest '\0' at txt[LLIM-1] */
    char buf[80], *mpt1, *mpt2;
    int  i, j, l;
    static unsigned char *tend;
    int once1, once2, once3;

    storehist = NO;
    goteof = NO;
    if (line == 1)
        once1 = once2 = once3 = NO;

/* use local variables to speed execution.  they are restored after
 * use to insure compatibality with egetc().  these i/o functions are used
 * similarly to the library functions but they are local.
 * (later timing studies show little if any improvement with local variables.)
 */
    iobuf = textbuf;
    nextp = iobuf -> nextp;
    nleft = iobuf -> nleft;

    while ( lastl < line && !(goteof) )  {
        t = &txt[0];
        tend = &txt[LLIM-1];
/* This section needs to be fast for the frequent tests.
 * Each machine instruction adds over a second to the load
 * time for a 500 kb document on a 5 MHz PC.
 */
        do {

skip:;
/*  following same as egetc().  inline to minimize execution time */

            if ( nleft-- ) {
                if ( (*t = (charmask & *nextp++) ) > 0x1F) {   /* char overlain later if wrong */
                    goto agn;
                }
                else {
                    --nextp;
                    c = *nextp++;
                }
            }
            else if ( (nbytes = read(iobuf -> fdes, iobuf -> buff, NSECTS*SECSIZ)) == -1 )  {
                nleft++;
                c = DFAIL;
            }
            else if ( nbytes == 0 )  {
                nleft++;
                c = ENDFILE;
            }
            else {
                nleft = nbytes ;
                nextp = iobuf -> buff;
                goto skip;

            }
/* the standard EOL is 0x0D 0x0A. */
            if (c == '\r') {
                goto eol;
            }
            else if (c == '\n') {
                goto skip;
            }
            else if (c == '\t') {
/* Detab.  Unconditional here, will be made the default option in later versions. */
                if (!once3) {
                    writeline(0,3,"Input is being detabbed.  Invoke with -Tn option to change tab.",ATTR2);
                    once3 = YES;
                }
                i = tabwidth - ((t - txt) % tabwidth);   /* 1 <= i <= tabwidth */
                for (j= 1; j <= i && !(t == (tend)); j++)
                    *t++ = ' ';
                t--;
            }
            else if (c == ENDCHAR) {
                goteof = YES;
                goto eol;   /* end of file is ^Z */
            }
            else if (c == ENDFILE) {
                goteof = YES;  /* end of file given by directory file size */
                goto eol;
            }
/* Retain control codes if the invocation option -C was specified,
 * otherwise convert them to '?'
 */
            else if ( c >= 0) {
                mpt1 = "File contains ASCII codes below 0x20";
                if (ctrl) {
                    *t = c;
                    mpt2 = "";
                }
                else {
                    *t = '?';
                     mpt2 = ". Converted to '?'. Use -C option to retain";
                }
                if (!once1) {
                    strcpy(buf,mpt1);
                    strcat(buf,mpt2);
                    writeline(0,1,buf,ATTR2);
                    once1 = YES;
                }
            }
            else {
                error("Disc Error");  /* wait for keystroke then edit whatever is there */
                goteof = YES;
                goto eol;
            }
agn:;
        }
        while ( !(++t == tend) );
/* ----------------------------------------*/
/* A line of maximal length followed by 0D 0A will result in the loop
 * falling through when there is no actual overflow.
 * No character is discarded on a line split,
 * and the handling of this rare case does not slow the loop.  This
 * algorithm limits the line length to LLIM-2 in most cases.
 */

        if (!once2) {
            sprintf(buf, "Line %d was too long, line(s) split.", line);
            writeline(0,2,buf,ATTR2);
            once2 = YES;
        }
eol:;
        *t = '\0';
        l = 0;
        if ( !goteof || !(t == &txt[0]) || (line == 1) ) {
            l = inject(lastl,txt);
        }
        if ( l == FAIL ) {
            goteof = YES;     /* at line limit */
        }
        else {
            if ( lastl % 100 == 0 ) {
                putlineno(lastl);
                displine = cline;  /* no change in text display */
                putallo();
            }
        }
    }
    iobuf -> nextp = nextp;
    iobuf -> nleft = nleft;

    storehist = YES;
    blankedmess = YES;
    if (goteof) {
        fclose(textbuf);
        if((once1 || once2 || once3) && (lastl < 5000) )
            wait(1);  /* wait so that the err msg can be seen on a fast system */
    }
    return;
}


/* open the file being edited for reading
 */
opentext(name)
char *name;
{
    int fopen1();
    int buf[FILELEN+20];
    strcpy(buf,name);
    if ( fopen1(name, textbuf) == FAIL)  {

/* attempt to open with default extension added */
        strcat(buf,".c");
        if ( fopen1(buf, textbuf) == FAIL )  {
/* try second default */
            strcpy(buf,name);
            strcat(buf,".doc");
            if (fopen1(buf, textbuf) == FAIL ) {
                strcpy(buf," Can't open file ");
                strncat(buf,name,FILELEN-1);
                error1(buf);
                name[0] = '\0';
                lastl   = 1;
                return  FAIL;
            }
        }
    }
    strcpy(name,buf);
    return  YES;
}
