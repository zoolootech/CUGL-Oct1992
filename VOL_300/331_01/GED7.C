/*
HEADER:         CUG199.07;
TITLE:          GED (nee QED) screen editor -- part 7;
DATE:           12/17/86;

DESCRIPTION:   "Low level terminal i/o functions for the GED editor.
                Putch, getch, etc.";
KEYWORDS:       putch, getch;
SYSTEM:         MS-DOS;
FILENAME:       GED7.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      DeSmet C;
*/

/*   e/qed/ged  screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle

    FILE:      ged7.c

    FUNCTIONS: getkey, testkey, getlow, testlow, getscankey,
               inkey, putret, dispch, putch, chkbuf, gettime,
               inword, uspr, beep, dohelp

    PURPOSE:   low level terminal i/o functions and additions

*/

#include <ctype.h>  /* for Microsoft only */
#include "ged.h"

char timestr[9],prevstr[9];

/* wait for a key to be pressed & return it,
 * translating to internal codes
 */
unsigned char getkey()
{
    unsigned char testkey();
    unsigned char c;

    while ( !(c = testkey()) )
        ;
    return c;
}


/* if a key has been pressed, return it, else 0
 * see pcio.a for mods to scr_ci
 */
unsigned char testkey()
{
    unsigned char inkey();
    unsigned char c, *tranp;
    int  i;

    if ( c = inkey() ) {

        if ( c == *(tranp = tran) )  {
            while ( !( c = inkey()) )
                ;
            c |= PARBIT;
        }
        for ( i = 1, tranp++; i < NKEYS; i++, tranp++ )  {
            if ( c == *tranp )
                return i;
        }
        if (c >= ' ')
            return c;
        else
            return 0;
    }
    else {
        show_time(0);
        return 0;
    }
}


/* get a key, converting control and u/c keys to l/c,
 * translation of ESCKEY only
 */
unsigned char getlow()
{
    unsigned char c;
    unsigned char testlow();

    while ( !(c = testlow()) );
    return  c;
}


/* test for a key, convert upper
 * and control chars to lower case
 */
unsigned char testlow()
{
    unsigned char inkey();
    unsigned char c;

    if ( !( c = inkey() ) )              return  0;
    if ( tran[ESCKEY] == c )             return  ESCKEY;
    if ( c >= F1KEY  &&  c  <= F10KEY )  return  c;

    if ( c < ' ') c = c + 96;

    return ( c >= 'A' && c <= 'Z' ? c + 32 : c );
}


/* get a key, translation of ESCKEY, CR, LEFTKEY,
 * DELLEFT, RETRIEVE only
 */
unsigned char getscankey()
{
    unsigned char inkey();
    unsigned char c;

    while ( !( c = inkey()) );
    if (tran[ESCKEY] == c)   return ESCKEY_P;
    if (tran[CR] == c)       return CR_P;
    if (tran[LEFTKEY] == c)  return LEFTKEY_P;
    if (tran[DELLEFT] == c)  return DELLEFT_P;
    if (tran[RETRIEVE] == c) return RETRIEVE_P;
    return c;
}


/* Dequeues and returns the next input character.  chkbuf() can be called
 * from anywhere to keep the input buffer current.  This buffer is
 * redundant with the BIOS input buffer, but is used to detect excessive
 * backlog.  Returns 0 if no key.  Never waits.
 */
unsigned char inkey()
{
    unsigned char c;
    int i;

    if (chkbuf()) {
        c = inbuf[0];
        inbufp--;
        for (i = 0; i < inbufp; i++)
            inbuf[i] = inbuf[i+1];
        return c;
    }
    else {
        return 0;
    }
}

/* Returns the next input character but leaves the character in the queue.
 * Allows a look-ahead capability without the commitment of processing
 * the next command.  Returns 0 if no key.  Never waits.
 */
unsigned char peekkey()
{
    if (chkbuf())
        return inbuf[0];
    else
        return 0;
}

/* Dequeue and return last key pressed.


/* type a CR/LF
 */
putret()
{
    putch('\r');
    putch('\n');
}


/* type a character, in different intensity
 * if non-printable turned off - mt.
 */
dispch(c)
char c;
{
    if ( c < ' ' )  {
/*  makeother();         */
/*  _os( DIRIO, c+64 );  */
/*  makeother();         */
    }
    else
        scr_co(c);
}


/* type a character to crt test for keyboard input
 */
putch(c)
char c;
{
    char cc;

    scr_co(c);
    chkbuf();
}


/*
 * Returns 0 if buffer is empty; otherwise returns the backlogged character
 * count.  First inputs and queues character if any.
 * show_time not used here only because it might slow some operations.
 */
int chkbuf()
{
    char cc;

    if ( ( cc = scr_csts() ) != 0  &&  inbufp < BUFFER )
        inbuf[inbufp++] = cc;
    return inbufp;
}




int gettime()
{
    unsigned  hours, mins, secs;

    rax = 0x2C00;
    syscall();

    mins  = rcx  &  0x00ff;
    hours = rcx  >>  8;
    secs = (rdx >> 8) & 0x00ff;
    sprintf( timestr, "%02d:%02d:%02d", hours, mins, secs );
    return secs;
}


/* refresh time field in status line if time has changed (change==0),
 * or unconditionally (change==1) */
show_time(change)
int change;
{
    int sav;  /* this variable must not be global */
    gettime();

    if (blankedmess || change || strcmp( timestr, prevstr ) != 0 )  {
        strcpy( prevstr, timestr );
        sav = curson(NO);
        scr_aputs( TIMEPOS, 0, timestr, ATTR3 );
        gotoxy(cursorx-offset,cursory);
        curson(sav);
    }
    return;
}


/* return true if c is a letter, digit or punctuation
 * not very elegant but lots of control, used to delete a word
 */
/* DeSmet version
 * inword(c)
 * char c;
 * {
 *   return  isalnum(c)  ||  index( "\"\\!@#$%^&*()_+-[]{};'`:~,./<>?|", c );
 * }
 */

/* Microsoft version */
inword(c)
char c;
{
    char *strchr();

/*   return  isalnum(c) || *strchr( "\"\\!@#$%^&*()_+-[]{};'`:~,./<>?|", c );*/
    return  isalnum(c) || *strchr( "\"\\!@#$%^&*()_+-[]{};'`:~./<>?|", c );
}

/* print 'n' as a number, return number of chars typed
 * useful in debugging
 */
int uspr(sn,ln,attri)
unsigned int sn,attri;
long int ln;
{
    int temp;
    long n;

    n = 0;
    if ( !ln )
        n = sn;
    else
        n = ln;

    if ( n < 10 )  {
        scr_putch( ( unsigned char )(n+'0'), attri );
        return 1;
    }
    temp = uspr( 0, n/10, attri );
    uspr( 0, n%10, attri );

    return  temp+1;
}


/* display help menu
 */
dohelp()
{
    int y;

    puttext();
    topline = 11;
    calp();
    cleareop(1);
    gotoxy(0,1);

/* display the help menu
 */
    putstr("LEFT:  char  <--, ^S     word ^A    line ^<--, ^U\n");
    putstr("RGHT:  char  -->, ^D     word ^F    line ^-->, ^]\n");
    putstr("UP:    window ^W    line ^E    page ^R, <PgUp>    file <Home>\n");
    putstr("DOWN:  window ^Z    line ^X    page ^C, <PgDn>    file <End>\n");
    putstr("DELETE: char ^G, <Del>, <BS>;  word ^T,   line  ^Y,  block ^K,  rpl ^V, <ins>\n");
    putstr("undo ^-   push line ^Y   pop ^P   copypop ^O   linejump ^J\n");
    putstr("Search/rpl: F3 up agn, F4 dn agn, ^L agn, ^QF,F2 new search, ^QA replace\n");
    putstr("<esc> cancel, ^K  block/output/read,  ^Q  find/rpl/files/paragraph\n");
    putstr("F1 help on/off  F6 cent. window.  F7 opts, F8 dir, F9 bye, F10 save\n");
    putstr("-------------------------------------------------------------------------------\n");

    putpage();
    return;
}



