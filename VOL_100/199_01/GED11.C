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
"ÉÍ Help 1 of 1 ÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÍÍ»", RSFW );
scr_putstr( 0, 2,
"ºQuit/Query  ^KQ³Char left ^S,^H ³Line left ^O,Home³Top of File ^U³Scroll Up ^Wº", RSFW );
scr_putstr( 0, 3,
"ºQuit/Exit   ^KX³Char rght ^D    ³Line rght ^P,End ³End of File ^B³Scroll Dn ^Zº", RSFW );
scr_putstr( 0, 4,
"ºRead a File ^KR³Char delt ^G,Del³Line delt ^\\     ³Delete EOL ^QY³Page Up   ^Rº", RSFW );
scr_putstr( 0, 5,
"ºBlock Mark  ^KB³Word left ^A,F3 ³Cursor up    ^E  ³Find       ^QF³Page Down ^Cº", RSFW );
scr_putstr( 0, 6,
"º  (Options)    ³Word rght ^F,F4 ³Cursor down  ^X  ³Replace    ^QA³Screen Up ^Yº", RSFW );
scr_putstr( 0, 7,
"ºUndelete    ^_ ³Word delt ^T    ³Jump to Line ^J  ³Repeat Cmd  ^L³Screen Dn ^Vº", RSFW );
scr_putstr( 0, 8,
"ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÏÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÏÍÍÍÍÍÍÍÍÍ Esc to Restore ÍÍ¼", RSFW );
}


void display_table1()
{
scr_putstr( 0, 1,
"ÉÍ Graphics 1 of 2 ÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»", RSFW );
scr_putstr( 0, 2,
"º 218 Ú 194 Â 191 ¿ ³ 213 Õ 209 Ñ 184 ¸  214 Ö 210 Ò 183 · ³ 201 É 203 Ë 187 » º", RSFW );
scr_putstr( 0, 3,
"º 195 Ã 197 Å 180 ´ ³ 198 Æ 216 Ø 181 µ  199 Ç 215 × 182 ¶ ³ 204 Ì 206 Î 185 ¹ º", RSFW );
scr_putstr( 0, 4,
"º 192 À 193 Á 217 Ù ³ 212 Ô 207 Ï 190 ¾  211 Ó 208 Ð 189 ½ ³ 200 È 202 Ê 188 ¼ º", RSFW );
scr_putstr( 0, 5,
"º 176 ° 177 ± 178 ² ³ 179 ³ 196 Ä 186 º  205 Í 220 Ü 223 ß ³ 219 Û 221 Ý 222 Þ º", RSFW );
scr_putstr( 0, 6,
"ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶", RSFW );
scr_putstr( 0, 7,
"ºEnter Alt and Character's Decimal Value Using Numeric Keypad.  ^L Repeat Key. º", RSFW );
scr_putstr( 0, 8,
"ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ Esc to Restore ÍÍ¼", RSFW );
}


void display_table2()
{
scr_putstr( 0, 1,
"ÉÍ Graphics 2 of 2 ÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»", RSFW );
scr_putstr( 0, 2,
"º  01   02   03  ³  04   05   06    07   08   09 na³  10 na 11   12  º", RSFW );
scr_putstr( 0, 3,
"º  13   14   15  ³  16   17   18    19   20   21  ³  22   23   24  º", RSFW );
scr_putstr( 0, 4,
"º  25   26 na 27  ³        28   29    30   31        ³ 155 › 156 œ 157  º", RSFW );
scr_putstr( 0, 5,
"º 171 « 172 ¬ 173 ­ ³ 174 ® 175 ¯ 224 à  225 á 226 â 227 ã ³ 228 ä 229 å 230 æ º", RSFW );
scr_putstr( 0, 6,
"º 231 ç 232 è 233 é ³ 234 ê 235 ë 236 ì  237 í 238 î 239 ï ³ 240 ð 241 ñ 242 ò º", RSFW );
scr_putstr( 0, 7,
"º 243 ó 244 ô 245 õ ³ 246 ö 247 ÷ 248 ø  249 ù 250 ú 251 û ³ 252 ü 253 ý 254 þ º", RSFW );
scr_putstr( 0, 8,
"ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÏÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÏÍ Esc to Restore ÍÍ¼", RSFW );
}


/* that's all */
