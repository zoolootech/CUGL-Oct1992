/*
Header:          CUG199;
Title:           Terminal specific module for ged editor;
Last Updated:    12/01/87;

Description:    "PURPOSE: terminal dependent screen control functions.
                 Uses DeSmet's PCIO.A screen code w/additions"

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged11.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 3.0;

References:
Endref;
*/

/*
e/qed/ged screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981

Modified:   Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
            March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

            May       1986:  qed converted to ged         (Mel Tearle)
            August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:       ged11.c

Functions:  delpage,
            norm, high, keytranslate, displayhelp

Note:       This file is very specific to IBM-PC like hardware.
            Mostly interrupt routines for time, pathnames, keyboard
            definitions, and help screen.
*/

#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


/* control key codes, here for convenience in defining terminal sequences
 */
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


/* defines the terminal key codes
 * which perform the editor commands
 */
void keytranslate()
{
/* each tran[xxxx]= should be set to the code emitted by the terminal
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
    tran[EOLKEY]    = CTRLP;   /*  7 cursor to end of line (P) */
    tran[BOLKEY]    = CTRLO;   /*  8 cursor to beginning of line (P) */
    tran[UPPAGE]    = CTRLR;   /*  9 scroll up a page (R) */
    tran[DOWNPAGE]  = CTRLC;   /* 10 scroll down a page (C) */
    tran[BOFKEY]    = CTRLU;   /* 11 cursor to beginning of file (U) */
    tran[HOMEKEY]   = CTRLB;   /* 12 cursor to end of file (B) */
    tran[DELLEFT]   = 0x7f;    /* 13 delete char to left of cursor (DEL) */
    tran[DELRIGHT]  = CTRLG;   /* 14 delete char under cursor (G) */
    tran[DELLNKEY]  = 0x1C;    /* 15 delete cursor line (^\) */
    tran[DELWDKEY]  = CTRLT;   /* 16 delete word to right of cursor (T) */
    tran[JUMPKEY]   = CTRLJ;   /* 17 jump to (J) */
    tran[CRSTILL]   = CTRLN;   /* 18 insert newline after cursor (N) */
    tran[QWIKKEY]   = CTRLQ;   /* 19 quit (Q) */
    tran[SCRLUPKEY] = CTRLW;   /* 20 scroll up thru text (W) */
    tran[TOPSCRKEY] = CTRLY;   /* 21 top of screen (Y) */
    tran[BOTSCRKEY] = CTRLV;   /* 22 bottom of screen (V) */
    tran[BLOCKKEY]  = CTRLK;   /* 23 block operations (K) */
    tran[SCRLDNKEY] = CTRLZ;   /* 24 scroll down thru text (Z) */
    tran[REPKEY]    = CTRLL;   /* 25 repeat last find/alter (L) */
    tran[HELPKEY]   = 0x1e;    /* 26 display help menu (^^) */
    tran[OOPSKEY]   = 0x1f;    /* 27 restore unedited line (^_) */
    tran[TAB]       = CTRLI;   /* 28 tab (I) */
    tran[RETRIEVE]  = CTRLL;   /* 29 retrieve (L) */
    tran[CR]        = 0x0d;    /* 30 return */
    tran[ESCKEY]    = 0x1b;    /* 31 escape, the magic key (ESC) */
}


/* interrupt routines follow */

#ifndef DS

union   REGS   r;
struct  SREGS  seg;

/* get current cursor position
 */
int getcursor()
{
r.x.ax = 0x0300;
r.x.bx = 0 << 8;
int86( 0x10, &r, &r );
return ( r.h.dl );
}


void curdrv(drv)
char drv[];
{
r.x.ax = 0x1900;               /* DOS interrupt 19 */
intdos( &r, &r );              /* gets current drive number */

drv[0] = ( char ) ( r.x.ax + 'a');  /* convert to symbolic drive name */
drv[1] = ':';
drv[2] = '\0';
}


/* getpath - get path to directory on default drive
 */
void getpath(path)
char path[];
{
strcat( path, "\\" );

r.x.ax = 0x4700;             /*   DOS interrupt 47 gets path string  */
r.x.dx = path[0] - '`';      /*   convert to index */
r.x.si = (unsigned)&path[3];
intdos( &r, &r );
}


/* this function originally used sprintf to format timestr, however
 * that added 4K of overhead to ged and was only used by this routine
 */
void xgettime()
{
unsigned  hour, min;
char str[4];

r.x.ax = 0x2C00;
intdos( &r, &r );

min  = r.x.cx  &  0x00ff;
hour = r.x.cx  >>  8;

zf_time( hour, str );   strcpy( timestr, str );   strcat( timestr, ":" );
zf_time( min, str );    strcat( timestr, str );
}


/* get status of parallel printer port - is the printer turned on?
 * report if error
 */
int prnstat()
{
r.x.ax = ( 0x02 << 8 );
r.x.dx = 0;                /* 0 = prn */
intdos( &r, &r );

if ( ( ( r.x.ax >> 8 ) & 0x80 ) ||
     ( ( r.x.ax >> 8 ) & 0x40 ) ||
     ( ( r.x.ax >> 8 ) & 0x10 ) )
      return( 1 );
else  {
     scr_co( '\07' );
     error( "error on printer " );
     return( 0 );
  }
}

#else

/* DeSmet C variables for 8086 registers and flags
 */
unsigned  _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
    char  _carryf, _zerof;


/* get current cursor position
 */
int getcursor()
{
_rax = 0x0300;
_rbx = 0 << 8;
_doint( 0x10 );
return ( _rdx & 0xff );
}


/* curdrv - get current default drive
 * from ed nather's ls.c
 */
void curdrv(drv)
char *drv;
{
int i;

i = _os( 0x19, 0 );       /* gets current drive number */

*drv++ = i + 'a';         /* convert to symbolic drive name */
*drv++ = ':';
return;
}


/* get path to directory on indicated drive
 * from ed nather's ls.c
 */
void getpath(path)
char *path;
{
extern int _showds();

strcat( path, "\\" );     /* append root file symbol to drive name */

_rax = 0x4700;            /* DOS interrupt 47 gets path string */
_rdx = *path - '`';       /* convert drive name to index */
_rsi = path + 3;          /* paste string after root symbol */
_rds = _showds();
_doint( DOSINT );
return;
}


/* this function originally used sprintf to format timestr, however
 * that added 4K of overhead to ged and was only used by this routine
 */
char xgettime()
{
unsigned  hour, min;
char str[4];

_rax = 0x2C00;
_doint( DOSINT );

min  = _rcx  &  0x00ff;
hour = _rcx  >>  8;

zf_time( hour, str );   strcpy( timestr, str );   strcat( timestr, ":" );
zf_time( min, str );    strcat( timestr, str );
}


/* get status of parallel printer port - is the printer turned on?
 * report if error
 */
int prnstat()
{
_rax = ( 0x02 << 8 );
_rdx = 0;                /* 0 = prn */
_doint( 0x017 );

if ( ( ( _rax >> 8 ) & 0x80 ) ||
     ( ( _rax >> 8 ) & 0x40 ) ||
     ( ( _rax >> 8 ) & 0x10 ) )
      return( 1 );
else  {
     scr_co( '\07' );
     error( "error on printer " );
     return( 0 );
  }
}

#endif


/* convert time to a 2 character zerofilled string
 * borrowed from Richard Threlkeld's itoa.c
 * uses strrev - DeSmet 3.0
 */
void zf_time(n,s)
int   n;
char s[];
{
int i = 0;

do  {
   s[i++] = ( char )( n % 10 + '0');
}  while ( ( n /= 10 ) > 0 );

while( i < 2 )  s[i++] = '0';
s[i] = '\0';
strrev(s);
}


/* the following code may do strange things if printed
 */

/* display the help menu
 * must be (helplines-1) lines of menu here
 */
void display_help()
{
scr_putstr( 0, 1,
"浜 Help 1 of 1 用様様様様様様様様冤様様様様様様様様冤様様様様様様用様様様様様様�", RSFW );
scr_putstr( 0, 2,
"�Quit/Query  ^KQ�Char left ^S,^H �Line left ^O,Home�Top of File ^U�Scroll Up ^W�", RSFW );
scr_putstr( 0, 3,
"�Quit/Exit   ^KX�Char rght ^D    �Line rght ^P,End �End of File ^B�Scroll Dn ^Z�", RSFW );
scr_putstr( 0, 4,
"�Read a File ^KR�Char delt ^G,Del�Line delt ^\\     �Delete EOL ^QY�Page Up   ^R�", RSFW );
scr_putstr( 0, 5,
"�Block Mark  ^KB�Word left ^A,F3 �Cursor up    ^E  �Find       ^QF�Page Down ^C�", RSFW );
scr_putstr( 0, 6,
"�  (Options)    �Word rght ^F,F4 �Cursor down  ^X  �Replace    ^QA�Screen Up ^Y�", RSFW );
scr_putstr( 0, 7,
"�Undelete    ^_ �Word delt ^T    �Jump to Line ^J  �Repeat Cmd  ^L�Screen Dn ^V�", RSFW );
scr_putstr( 0, 8,
"藩様様様様様様様様様様様様様様様溶様様様様様様様様溶様様様様� Esc to Restore 様�", RSFW );
}


void display_table1()
{
scr_putstr( 0, 1,
"浜 Graphics 1 of 2 用様様様様様様様様様様様様様様様様様様様冤様様様様様様様様様�", RSFW );
scr_putstr( 0, 2,
"� 218 � 194 � 191 � � 213 � 209 � 184 �  214 � 210 � 183 � � 201 � 203 � 187 � �", RSFW );
scr_putstr( 0, 3,
"� 195 � 197 � 180 � � 198 � 216 � 181 �  199 � 215 � 182 � � 204 � 206 � 185 � �", RSFW );
scr_putstr( 0, 4,
"� 192 � 193 � 217 � � 212 � 207 � 190 �  211 � 208 � 189 � � 200 � 202 � 188 � �", RSFW );
scr_putstr( 0, 5,
"� 176 � 177 � 178 � � 179 � 196 � 186 �  205 � 220 � 223 � � 219 � 221 � 222 � �", RSFW );
scr_putstr( 0, 6,
"把陳陳陳陳陳陳陳陳陳祖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳珍陳陳陳陳陳陳陳陳陳超", RSFW );
scr_putstr( 0, 7,
"�Enter Alt and Character's Decimal Value Using Numeric Keypad.  ^L Repeat Key. �", RSFW );
scr_putstr( 0, 8,
"藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様� Esc to Restore 様�", RSFW );
}


void display_table2()
{
scr_putstr( 0, 1,
"浜 Graphics 2 of 2 用様様様様様様様様様様様様様様様様様様様冤様様様様様様様様様�", RSFW );
scr_putstr( 0, 2,
"�  01   02   03  �  04   05   06    07   08   09 na�  10 na 11   12  �", RSFW );
scr_putstr( 0, 3,
"�  13   14   15  �  16   17   18    19   20   21  �  22   23   24  �", RSFW );
scr_putstr( 0, 4,
"�  25   26 na 27  �        28   29    30   31        � 155 � 156 � 157 � �", RSFW );
scr_putstr( 0, 5,
"� 171 � 172 � 173 � � 174 � 175 � 224 �  225 � 226 � 227 � � 228 � 229 � 230 � �", RSFW );
scr_putstr( 0, 6,
"� 231 � 232 � 233 � � 234 � 235 � 236 �  237 � 238 � 239 � � 240 � 241 � 242 � �", RSFW );
scr_putstr( 0, 7,
"� 243 � 244 � 245 � � 246 � 247 � 248 �  249 � 250 � 251 � � 252 � 253 � 254 � �", RSFW );
scr_putstr( 0, 8,
"藩様様様様様様様様様詫様様様様様様様様様様様様様様様様様様溶� Esc to Restore 様�", RSFW );
}


/* that's all */
