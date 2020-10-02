/*
Header:          CUG199;
Title:           Module 8 of ged editor;
Last Updated:    12/01/87;

Description:    "PURPOSE: get and put text lines into and out of storage";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged8.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compiler:        DeSmet 2.51;	s


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

File:      ged8.c

Functions: loc, gettext, getline, writ_txb, deltp, puttext,
           readtext, opentext, balloc, error, addhistory
*/


#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


/* returns line + move, adjusted to be within text
 */
int loc(line,move)
int line, move;
{
int y;

if ( ( y = line + move ) < 1 )  y = 1;
if ( lastl == UNKNOWN  &&  y > lastread )  readtext( y );
if ( y > lastl )  return lastl;

return y;
}


/* makes 'line' the current line
 */
void gettext(line)
int line;
{
char *getline();

strcpy( text, getline( line ) );
cline = line;
}


/* returns address of text of 'line'
 * and updates page usage
 */
char *getline(line)
int line;
{
int  slot;

line = loc( line, 0 );

if ( ( slot = pageloc[ tp[line].page] ) <= 0 )
       slot = swappin( tp[line].page );

if ( usage[slot] > FREE )
     usage[slot] = ++pclock;

return  slotaddr[slot] + tp[line].moffset;
}


/* inserts 'txt' after 'line'
 */
int writ_txb( line, txt )
int  line;
char *txt;
{
int  l, p, balloc();
struct  addr  anaddr;

addhistory( line, HISTINSERT );

if ( ( char * ) &tp[lastread+2] >= slotaddr[tppages] )  {
  /* need another slot to store tp's in
   */
  if ( ( tppages + 2  ) == slotsinmem )  {
         error( " Too many lines of text " );
         return  FAIL;
  }
  if ( usage[tppages] == NOTAVAIL )  /* ie page is being alloc'ed into */
       allocp = PAGESIZE+1;          /* force alloc into new page */

  if ( usage[tppages] != FREE )
       swapout(tppages);             /* changed */

  usage[tppages++] = NOTAVAIL;
}

#ifndef  DS
  if ( ++line < ++lastread )
       memcpy( &tp[line+1], &tp[line], (lastread-line)*sizeof(anaddr) );
#else
  if ( ++line < ++lastread )
       _move( (lastread-line)*sizeof(anaddr), &tp[line], &tp[line+1] );
#endif

tp[line].moffset = p = balloc( 1 + strlen( txt ) );
tp[line].page = newpage;
strcpy( npaddr+p, txt );

if ( lastl != UNKNOWN )  lastl++;

return  line;
}


/* delete line by shifting pointers
 */
void deltp(from)
int from;
{
struct  addr  anaddr;

addhistory( from, HISTDELETE );    /* add for undo */

#ifdef  MSC
   memmove( &tp[from], &tp[from+1], ( lastread - from )*sizeof(anaddr) );
#else
  _move( ( lastread - from )*sizeof( anaddr ), &tp[from+1], &tp[from] );
#endif

if ( lastl != UNKNOWN )  lastl--;
lastread--;
if ( lastl < 1 )  lastl = 1;
if ( lastread < 1 )  lastread = 1;
}


/* replaces cline's text if it has been altered
 */
void puttext()
{
int p, cp, balloc();

if ( altered )  {
     addhistory( cline, HISTREPLACE );   /* add for undo */
     altered = NO;
     if ( !trail )  {
          for ( cp = strlen( text )-1;
                cp >= 0  &&  isspace( text[cp] ); cp-- )
                text[cp] = '\0';
     }
     tp[cline].moffset = p = balloc( 1 + strlen( text ) );
     tp[cline].page = newpage;
     strcpy( npaddr+p, text );
   }
}


/* Reads file being edited into virtual memory
 * until 'line' is read, or eof.
 * If eof, sets lastl to number of last line read.
 * File is assumed to be already open
 */
void readtext(line)
int line;
{
char txt[LLIM], *t;
int  i, c, l;

storehist = NO;

while ( lastread < line )  {
  for ( i = 0, t = &txt[0]; i < LLIM  &&
        ( ( c = egetc( ( struct iobuffer *)textbuf) ) >= ' ' ||
        c != '\n' && c != DFAIL  &&  c != ENDFILE ); i++ )  {
        if ( c )  {
            *t = ( char ) c;
             t++;
        }
        else i--;
        }
  if ( txt[i-1] == '\r' )
       t--;
  *t = '\0';

  if ( ( l = writ_txb( lastread, txt ) ) == FAIL )
         goto  toomuch;
  else
    if ( ( lastread = l )%100 == 0 )
           putlineno( lastread );

    if ( ( goteof = ( char ) ( c == ENDFILE ) ) || c == DFAIL )  goto eof;
}
goto ok;

eof:
  ffclose( ( struct iobuffer *) textbuf );
toomuch:
  lastl = lastread;
ok:
  storehist = YES;
}


/* open the file being edited for reading
 */
int opentext(name)
char *name;
{
if ( ffopen( name, ( struct iobuffer * ) textbuf ) == FAIL )  {

/* attempt to open with default extension added
 */
  strcat( name, defext );

  if ( ffopen( name, ( struct iobuffer * ) textbuf ) == FAIL )  {
       error( " Can't open file " );
       name[0] = '\0';
       lastl = 1;
       return  FAIL;
  }
}
return  YES;
}


/* allocate and return the offset in newpage of a vector size n
 */
int balloc(n)
unsigned n;
{
int slot;

if ( ( allocp + n ) >= PAGESIZE )  {

/* no room in current newpage; get another
 */
if ( pageloc[newpage] > 0 )  usage[pageloc[newpage]] = INUSE;
     pageloc[++newpage] = slot = freememslot();
     usage[slot] = NOTAVAIL;
     allocp = 0;
     npaddr = slotaddr[slot];
}
allocp += n;
return  allocp-n;
}


void error(message)
char *message;
{
if ( errmess != NULL )  return;

scr_delete( 0, 0 );
scr_cursoff();
scr_aputs( 0, 0, message, REVS );  scr_ci();
blankedmess = YES;
scr_curson();
resetcursor();
inbufp = 0;
}


void addhistory(l,type)
int l;
int type;
{
struct  histstep  *step;

if ( !storehist )  return;

step = &history[histptr];

step->histp.page = tp[l].page;
step->histp.moffset = tp[l].moffset;

step->histline = l;
step->histtype = type;
step->histcomm = ncommand;

if ( ++histptr == HISTLEN )  histptr = 0;
if ( histcnt < HISTLEN )  histcnt++;
}


/* that's all */
