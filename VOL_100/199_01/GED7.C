/*
Header:          CUG199;
Title:           Module 7 of ged editor;
Last Updated:    09/19/87;

Description:    "PURPOSE: low level terminal i/o functions";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged7.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 3.0;

References:
Endref;
*/

/*
e/qed/ged  screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981

Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
           March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

           May       1986:  qed converted to ged         (Mel Tearle)
           August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:      ged7.c

Functions: getkey, testkey, getlow, testlow, getscankey,
           inkey, putret, chkbuf, inword,
           makedim, makebright, makeother, beep, show_table
*/


#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


/* wait for a key to be pressed & return it,
 * translating to internal codes
 */
char getkey()
{
unsigned char c;

while ( !( c = testkey() ) );
return c;
}


/* if a key has been pressed, return it, else 0
 * see pcio.a for mods to scr_ci
 */
char testkey()
{
unsigned char c, *tranp;
int  i;

if ( !( c = inkey() ) )  return 0;

/* try and match translation keys
 * would appreciate someone explaining this to me - mt.
 */
if ( c == *(tranp = tran) )  {
     while ( !( c = inkey()) );
               c |= PARBIT;
}
/* synonym for move cursor left
 */
if ( c == BACKSP )  return LEFTKEY;

for ( i = 1, tranp++; i < NKEYS; i++, tranp++ )  {
      if ( c == *tranp )
           return ( unsigned char )i;
}
return c;
}


/* get a key, converting control and u/c keys to l/c,
 * translation of ESCKEY only
 */
char getlow()
{
unsigned char c;

while ( !(c = testlow()) );
return  c;
}


/* test for a key, convert upper
 * and control chars to lower case
 */
char testlow()
{
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
char getscankey()
{
unsigned char c;

while ( !( c = inkey()) );
  if ( tran[ESCKEY] == c)   return ( char )  ESCKEY_P;
  if ( tran[CR] == c)       return ( char )  CR_P;
  if ( tran[LEFTKEY] == c)  return ( char )  LEFTKEY_P;
  if ( tran[DELLEFT] == c)  return ( char )  DELLEFT_P;
  if ( tran[RETRIEVE] == c) return ( char )  RETRIEVE_P;
return c;
}


/* return last key pressed
 * here's where all the goodies come from
 * add chkbuf() to capture where desired
 */
char inkey()
{
unsigned char c;
int i;

if ( inbufp )  {
     c = inbuf[0];
     inbufp--;
     for (i = 0; i < inbufp; i++)
          inbuf[i] = inbuf[i+1];
     return c;
}

if ( ( c = scr_csts() ) == 0 )
            ;
return c;
}


/* type a CR/LF
 */
void putret()
{
scr_co( '\r' );
scr_co( '\n' );
}


/* if something in character que
 * and inbufp less than sizeof buffer, add to buffer
 */
void chkbuf()
{
unsigned char cc;

if ( ( cc = scr_csts() ) != 0  &&  inbufp < BUFFER )
       inbuf[inbufp++] = cc;
}


void show_time()
{
xgettime();
scr_cursoff();
scr_aputs( TIMEPOS, 0, timestr, DIM );
scr_curson();
}


/* return true if c is a letter, digit or punctuation
 * not very elegant but lots of control, used to delete a word
 */
int inword(c)
char c;
{
return  isalnum( c )  ||  index( "\"\\!@#$%^&*()_+-[]{};'`:~,./<>?|", c );
}


void makedim()
{
dim(); isdim = YES;
}


void makebright()
{
high(); isdim = NO;
}


void makeother()
{
if ( isdim )
      makebright();
else
      makedim();
}


/* display the proper table
 */
void show_table(table)
int table;
{

topline = window;

if ( !screen )  {
     pfirst  = loc( pfirst, ( window - 1 ) );
     plast   = loc( pfirst, SHEIGHT - topline );
}

switch( table )  {
  case  HELP :
        display_help();
        break;
  case  TABLE1 :
        display_table1();
        break;
  case  TABLE2 :
        display_table2();
        break;
  default :
        screen = NO;
        return;
}

puttext();
if ( !screen )  putpage();
screen = table;
}
