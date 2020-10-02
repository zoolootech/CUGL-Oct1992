/*
HEADER:         CUG999.12;
TITLE:          Terminal Drivers;
DATE:           10/03/86;

DESCRIPTION:   "PC Clone I/O routines for GED editor.
                Contains calls to assembly routines in file pcio.a.";
SYSTEM:         MS-DOS;
FILENAME:       TERM.C;
WARNINGS:      "O file must be present during link for ged";
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      DeSmet C;
*/

/*
     e/qed/ged screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986;   converted to ged - Mel Tearle

    FILE:      term.c

    FUNCTIONS: terminit, gotoxy, curson, cleareol, cleareop,
               keytranslate, scrollup, scrolldown,
               colornorm, color1, colorblock, colorerr

    PURPOSE:   terminal dependent screen control functions

This file in principle contains the functions which must be modified or
rewritten for converting the program to a different terminal or system.
There are several exceptions.  Some modules make direct calls to functions
beginning with scr_ in in pcio.asm.  Those routines conceptually belong
here, but the convesion is not complete.

The keyboard input functions would ideally consolidate all the terminal or
system specific code here.  The input processing is in a state of disarray
in this version.  There is some obsolete code and concepts still in the
system.  Consolidation is needed.  There are also places where characters
should be redefined as integers in order to not compromise the 256-symbol
character set.

*/

#include <dos.h>
#include "ged.h"

/* control key codes, here for convenience in defining terminal sequences */

#define CTRLA   0x01    /*  SOH */
#define CTRLB   0x02    /*  STX */
#define CTRLC   0x03    /*  ETX */
#define CTRLD   0x04    /*  EOT */
#define CTRLE   0x05    /*  ENQ */
#define CTRLF   0x06    /*  ACK */
#define CTRLG   0x07    /*  BEL */
#define CTRLH   0x08    /*  BS  */
#define CTRLI   0x09    /*  HT  */
#define CTRLJ   0x0a    /*  LF  */
#define CTRLK   0x0b    /*  VT  */
#define CTRLL   0x0c    /*  FF  */
#define CTRLM   0x0d    /*  CR  */
#define CTRLN   0x0e    /*  SO  */
#define CTRLO   0x0f    /*  SI  */
#define CTRLP   0x10    /*  DLE */
#define CTRLQ   0x11    /*  DC1 */
#define CTRLR   0x12    /*  DC2 */
#define CTRLS   0x13    /*  DC3 */
#define CTRLT   0x14    /*  DC4 */
#define CTRLU   0x15    /*  NAK */
#define CTRLV   0x16    /*  SYN */
#define CTRLW   0x17    /*  ETB */
#define CTRLX   0x18    /*  CAN */
#define CTRLY   0x19    /*  EM  */
#define CTRLZ   0x1a    /*  SUB */

/* --------------------- start here -------------------------- */

/*
 *  This function is compiler dependent and must be replaced entirely for
 *  compiling with a different compiler.  The calls to this function
 *  are compiler independent.
 *
 *  Screen output and keyboard input do not pass through this function.
 *  The DOS calls are too slow for screen output.
 *
 *  Some of the calls to this function will work only in the small
 *  data memory model.
 */


syscall()
{
    int intdos();
    union REGS inregs, outregs;

/* set up the hardware registers for passing values to the system */
    inregs.x.ax = rax;
    inregs.x.bx = rbx;
    inregs.x.cx = rcx;
    inregs.x.dx = rdx;
    inregs.x.si = rsi;
    intdos(&inregs, &outregs);

/* return the register results to the caller */
    rax = outregs.x.ax;
    rbx = outregs.x.bx;
    rcx = outregs.x.cx;
    rdx = outregs.x.dx;
    rsi = outregs.x.si;
    carryf = outregs.x.cflag;
    return;
}




/* sets internal location for start of display address among other things
 */
terminit()
{
    scr_setup();
}



/* move cursor to column x, line y. top left is (0,0)
 */
gotoxy(x,y)
int x,y;
{
    scr_rowcol( y, x);
    return;
}

/* cursor on/off
   cursor is turned on if argument is 1; off if 0.  The cursor state that
   existed before the call is returned so that it can be saved by the
   caller for later restoration.
   YES = 1; NO = 0.
 */
curson(onoff)
int onoff;
{
    static int old = -1;
    int i;

    if (old == onoff)
        return old;  /* no change */

    i = old;
    if(onoff) {
        scr_curson();
        old = 1;
     }
     else {
        scr_cursoff();
        old = 0;
    }
    return i;
}

/* write a string and clear from string end to end of line.  x and y are
 * physical coordinates.
 */
writeline(x, y, msg, attrib)
int x, y, attrib;
char *msg;
{
    scr_putstr(x, y, msg, attrib);
    scr_delete(x + strlen(msg), y);
    return;
}


/* clear to end of line.  x and y are physical cursor addresses (not
 * the variable cursorx).  0 <= x <= SWIDTH,  0 <= y <= SHEIGHT.
 */
cleareol(x,y)
int x, y;
{
    scr_rowcol( y, x);
    scr_clrl();
    return;
}


/* clear line y and all following lines.  Leave cursor at the start of
 * line y.  y is the physical cursor value, 0 <= y <= SHEIGHT.
 */
cleareop(y)
int y;
{
    gotoxy(0, y);
    scr_cls();
    gotoxy(0, y);
    return;
}

/* scroll up 1 line.  Only one-line scrolls are currently supported.  More
 * could be accomodated, but there is no real incentive to.  The argument is
 * the physical cursor y position, 0 <= top <= SHEIGHT.  Lins from there on
 * down are scrolled up one line.  Scrolling is much faster than rewriting.
 * Clears the bottom line if the argument is the bottom line.
 */
scrollup(top)
int  top;
{
    if (top < SHEIGHT)
        scr_scrup( 1, top, 0, SHEIGHT, SWIDTH );
    else
        cleareol(0, SHEIGHT);
    return;
}

scrolldown(top)
int  top;
{
    if (top < SHEIGHT)
        scr_scrdn( 1, top, 0, SHEIGHT, SWIDTH );
    return;
}

/* set screen character attribute to normal.
 * This control if implemented as a function so that the function can
 * send control characters to the display if the program is ported to
 * a system with a remote terminal.  The overall program still needs quite
 * a bit of work on attribute control.  One function with arguments seems
 * a better way in retrospect.  Some rethinking on attribute control is needed.
 */
colornorm()
{
    attr = ATTR0;
    return attr;
}

/* set screen character attribute to highlighted (abnormal).  See
   description in the .h header file. */

color1()
{
    attr = ATTR1;
    return attr;
}

/* Set screen character attribute to the form used for marking blocks.
 * See the header file.
 */
colorblock()
{
    attr = ATTR2;
    return attr;
}
color3()
{
    attr = ATTR3;  /* underlined, normal intensity */
    return attr;
}
color4()
{
    attr = ATTR4;  /* underlined and highlighted */
    return attr;
}


/* Set the screen character attribute to the form used for coloring error
 * messages
 */
colorerr()
{
    return colorblock();
}



/*----------------------INPUT FROM KEYBOARD-----------------------------*/


/* defines the terminal key codes
 * which perform the editor commands
 */
keytranslate()
{
/*
    Keyboard translation has become fragmented.  This table should be
    restructured to allow unrestricted byte value multikey
    commands.  There should also be a place in the table for at
    least one alternate but equivalent key sequence.  The translation
    now done in pcio.asm should probably be moved here.  Code for
    the parity bit described below still exists but is mostly inactive.
    The keyboard characters seen at the top level should
    be convrted to integer type to provide more value flexibility.
    The Microsoft 4.0 convention of signed bytes also complicates
    the use of byte size characters for control purposes.
    The value restrictions are now at the byte limit.      G.O.

 */

/*
    Each tran[xxxx]= should be set to the code emitted by the terminal
    key which will perform the indicated action.  The recommended (control)
    key assignments are shown in round brackets.
        Some terminals precede their codes by a lead-in character
    (eg the Hazeltine uses the tilde).  This char should be assigned to
    tran[LEADIN].  If there is no lead-in character, set tran[LEADIN] to zero.
        'e' will ignore the leadin character if tran[LEADIN] is non-zero,
    but will set the parity bit on the next character. All other chars from the
    keyboard will already have any parity bits removed as they are read in.  Thus
    codes with lead-ins should be entered in the table below as code+0x80, or
    more clearly, as code+PARBIT.
        For example, suppose that one is coding the Hazeltine 1420, which
    generates a tilde,^L (0x0d) sequence when the cursor up key is pressd.  To
    recognise this sequence, set tran[LEADIN] to tilde (0x7e) and set
    tran[UPKEY] to 0x0d+PARBIT.
    */
    tran[LEADIN]    = 0;       /*  0 lead-in character, 0 if not used */
    tran[DOWNKEY]   = CTRLX;   /*  1 cursor down */
    tran[UPKEY]     = CTRLE;   /*  2 cursor up */
    tran[LEFTKEY]   = CTRLS;   /*  3 cursor left */
    tran[RIGHTKEY]  = CTRLD;   /*  4 cursor right */
    tran[RIGHTWKEY] = CTRLF;   /*  5 cursor right one word (F) */
    tran[LEFTWKEY]  = CTRLA;   /*  6 cursor left one word (A) */
    tran[EOLKEY]    = 0x1D;    /*   7 cursor to end of line ^]) */
    tran[BOLKEY]    = CTRLU;   /*  8 cursor to beginning of line (U) */
    tran[UPPAGE]    = CTRLR;   /*  9 scroll up a page (R) */
    tran[DOWNPAGE]  = CTRLC;   /* 10 scroll down a page (C) */
    tran[BOFKEY]    = 220;     /* 11 cursor to beginning of file (see pcio.asm) */
    tran[HOMEKEY]   = 221;     /* 12 cursor to end of file (see pcio.asm) */
    tran[DELLEFT]   = 0x08;    /* 13 delete char to left of cursor */
    tran[DELRIGHT]  = CTRLG;   /* 14 delete char under cursor (G) */
    tran[DELLNKEY]  = CTRLY;   /* 15 delete remainder of line (^Y) */
    tran[DELWDKEY]  = CTRLT;   /* 16 delete word to right of cursor (T) */
    tran[JUMPKEY]   = CTRLJ;   /* 17 jump to (^J) */
    tran[CRSTILL]   = CTRLN;   /* 18 line break (N) */
    tran[QWIKKEY]   = CTRLQ;   /* 19 quit (^Q) */
    tran[WINDUP]    = CTRLW;   /* 20 window up one line ^(W) */
    tran[POP]       = CTRLP;   /* 21 pop one line */
    tran[CPOP]      = CTRLO;   /* 22 copy and pop */
    tran[BLOCKKEY]  = CTRLK;   /* 23 block operations (K) */
    tran[WINDDOWN]  = CTRLZ;   /* 24 window down one line (^Z) */
    tran[REPKEY]    = CTRLL;   /* 25 repeat last find/alter (L) */
    tran[INSKEY]    = CTRLV;   /* 26 character insert/replace */
    tran[OOPSKEY]   = 0x1f;    /* 27 restore unedited line (^_) */
    tran[TAB]       = CTRLI;   /* 28 tab (I) */
    tran[RETRIEVE]  = CTRLL;   /* 29 retrieve (L) */
    tran[CR]        = 0x0d;    /* 30 return */
    tran[ESCKEY]    = 0x1b;    /* 31 escape, the magic key (ESC) */
}


/* that's all */










