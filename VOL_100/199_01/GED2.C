/*
Header:          CUG199;
Title:           Module 2 of ged editor;
Last Updated:    09/19/87;

Description:    "PURPOSE: perform text changing commands";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged2.c;
Warnings:       "O file must be present during link of ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 3.0;

References:
Endref;
*/

/*
e/qed/ged screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 -  August-December 1981

Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
           March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

           May       1986:  qed converted to ged         (Mel Tearle)
           August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:      ged2.c

Functions: movechar, moveline, scroll, calcjmp, jumpline,
           movepage, moveword, insertchar, deletechar, crdelete,
           deleteword, crinsert, undo, adjustc, sync
*/


#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


/* move cursor by 'move' columns to the right
 * return YES unless going off text
 */
int movechar(move)
int  move;
{
int  cp, len, result;

cp = charn + move;
result = YES;

if ( cp < 0 )  {
     result = moveline(-1);
     if ( result )  {
          adjustc( LLIM );
          movechar( cp + 1 );
     }
     else sync(0);
}
else if ( cp > ( len = strlen(text) ) )  {
          result = moveline(1);
          if ( result )  {
               sync(0);
               movechar( cp-len-1 );
          }
          else adjustc( LLIM );
}
else sync(cp);
return  result;
}


/* move cursor by 'move' lines down
 * return YES if Ok, NO if going off text
 */
int moveline(move)
int move;
{
int line;

puttext();
if ( ( move < 0 ? -move : move ) > SHEIGHT )  scr_xy( WAITPOS, 0 );
line = cline;

if ( ( cline = loc( cline, move ) ) != line )  {
       gettext( cline );
       if ( cline < pfirst || cline > plast )  {
            if ( move == 1 || move == -1 )  scroll( move );
                 else putpage();
       }
       else  {
         if ( ( !blocking )  &&  ( !offset ) )  {
                cursory += cline - line;
                adjustc( cursorx );
         }
         else  {
            putline( line, cursory );
            cursory += cline - line;
            adjustc( cursorx );
            putline( cline, cursory );
         }
       }
  return  YES;
  }
else return  NO;
}


/* scroll up ( move EQ 1 ) or down 1 line
 */
void scroll(move)
int  move;
{

if ( move == 1 )
     scr_scrup( 1, topline, 0, SHEIGHT, SWIDTH );
  else
     scr_scrdn( 1, topline, 0, SHEIGHT, SWIDTH );

adjustc( cursorx );
putline( cline, cursory );

if ( plast - pfirst == ( SHEIGHT - topline) )  plast += move;
pfirst += move;
}


/* calc number of lines to jump
 */
void calcjmp()
{
int to;

/* reset jmpto for repeat incrmtl jumps
 */
jmpto = 1;

if ( ans[0] == '+' )
     if ( ( to = atoi( ans+1 ) ) )  jumpline( to );
if ( ans[0] == '-' )
     if ( (to = atoi( ans ) ) )     jumpline( to );
if ( ans[0] >= '0' )  {
     if ( (to = atoi( ans ) ) )     jumpline( to-cline );
           jmpto = to;
   }
}


/* move current line by move lines down,
 * checking for interrupt from user
 * (if interrupted, do nothing, and return NO)
 */
int jumpline(move)
int move;
{
int  line, dest;
char testkey();

puttext();
dest = cline + move;
dest -= dest % 100;

if ( dest > lastread )  {
     scr_xy( WAITPOS, 0 );
     line = cline;
     while ( line < dest && loc( line, 100 ) != line )  {
             line += 100;
             if ( testkey() == ESCKEY )  {
                  error( " Interrupted" );
                  return  NO;
             }
     }
}
moveline( move );
return  YES;
}


/* move current line by a page down ( dir == 0 ) or up (-1)
 */
void movepage(dir)
int dir;
{
int move, line;

scr_cursoff();
puttext();
move = ( SHEIGHT-topline )/2 - PAGEOVERLAP;

if (dir)  move = pfirst - cline - move;
else move = plast - cline + move;

if ( ( cline = loc( ( line = cline ), move ) ) != line )  {
       gettext( cline );
       putpage();
}
scr_curson();
}


/* move 1 word to the right (move -ve: left)
 */
void moveword(move)
int move;
{
if ( charn + move < 0 &&  cline > 1 )  {
     moveline(-1);
     charn = strlen( text );
}
else if ( charn + move >= strlen( text ) )  {
          moveline(1);
          sync(0);
          if ( inword( text[0] ) )  return;
}

while ( ( move < 0 || text[charn] ) && inword( text[charn] )
                                    && ( charn += move ) );
while ( ( move < 0 || text[charn] ) && !inword( text[charn] )
                                    && ( charn += move ) );
if ( move < 0  &&  charn )  {
     while ( inword(text[charn] )   &&  --charn );
     if ( charn || !inword( text[charn] ) )
          charn++;
}
sync( charn );
}


/* if not overtype then insert 'c' at charn, move cursor up one
 * else overwrite character under cursor
 */
void insertchar(c)
char c;
{
int  cp;

if ( ( cp = strlen( text ) + 1 ) >= ( LLIM - 1 ) )
       error( " Line too long " );
else  {
  if ( !overtype  ||  ( overtype  &&  ( charn == ( cp - 1 ) ) ) )
       for ( ; cp >= charn; cp-- )
             text[cp+1] = text[cp];

  text[charn] = c;
  altered = YES;
  rewrite( charn, cursorx );
  sync( charn+1 );
  }
}


/* deletes char before (dir=-1) or at (dir=0) cursor
 */
void deletechar(dir)
int dir;
{
char c;
int  cp;

cp = charn + dir;
if ( cp < 0 )  {
     if ( cline > 1 )  crdelete(-1);
}
else if ( text[cp] == '\0' )  {
     if ( cline < lastl )  crdelete(0);
}
else  {
  do  {
     c = text[cp] = text[cp+1];
     cp++;
  }
  while(c);

  altered = YES;
  sync( charn + dir );
  if ( calcoffset() != lastoff )
       rewrite( 0, 0 );
  else
       rewrite( charn, cursorx );
  }
}


/* delete a [CR] before (dir==-1) or at (dir==0) cursor
 */
void crdelete(dir)
int dir;
{
int  delline, len, *t;
char textb[LLIM];

altered = YES;
if ( dir == 0 )  {
     delline = cline + 1;
     strcpy( textb, getline( delline ) );
     cursory++;
}
else  {
  delline = cline;
  strcpy( textb, text );
  if ( cline > 1 )  gettext( cline-1 );
  else  puttext();
}
sync( ( len = strlen(text) ) );

if ( len + strlen(textb) >= LLIM )  {
     textb[LLIM-len] = '\0';
     error( " Line too long - cut short " );
}

strcat( text, textb );
puttext();
deltp( delline );

if ( delline > plast || delline <= pfirst )  putpage();
else  {
    scr_scrup( 1, cursory--, 0, SHEIGHT, SWIDTH );
    rewrite( 0, 0 );

    if ( plast <= lastl && lastl-pfirst > SHEIGHT - topline )
         putline( plast, SHEIGHT );
    else  plast = lastl;
  }
}


/* deletes up to first occurrence of white space
 * if no white space then delete to end of line
 * see inword() in ged7.c
 */
void deleteword()
{
int  pend, cp, in;
char c;

for ( in = inword( text[pend = charn] );
      ( c = text[pend] )  &&  ( in ? inword(c): !inword(c) );
        pend++ )
        ;
for  ( cp = charn; ( text[cp] = text[pend] ); pend++, cp++ )
        ;
rewrite( charn, cursorx );
altered = YES;
}


/* insert a [CR] behind ( dir=0 ) or in front of (dir=-1) cursor
 * dir is 0
 */
void crinsert(dir)
{
char textb[LLIM], c;
int  charnb;

charnb = 0;
if ( autoin  &&  !isspace( text[charn] ) )
     while( isspace( ( c = text[charnb] ) ) )  textb[charnb++] = c;

strcpy( &textb[charnb], &text[charn] );
text[charn] = '\0';
altered = YES;

if ( dir == 0 )  {
     puttext();
     strcpy( text, textb );
     altered = YES;
     sync( charnb );
     if ( ( cline = writ_txb( cline, textb ) ) == FAIL )  return;
}
else if ( writ_txb( cline, textb ) == FAIL )  return;

if ( cursory >= SHEIGHT )  {
     puttext();
     putpage();
}
else  {
  scr_scrdn( 1, cursory+1, 0, SHEIGHT, SWIDTH );

  if ( dir == 0 )
       putline( cline-1, cursory++ );
  else
       putline( cline+1, cursory+1 );

  if ( plast - pfirst < SHEIGHT - topline )  plast++;

  rewrite( 0, 0 );
  }
}


/* undo edit on current line and then from history
 */
void undo()
{
int  onpage;     /* flag for changes to current screen */
int  l, slot;

char textb[LLIM];

if ( altered )  {
     if ( cline <= lastl )  gettext( cline );  else text[0] = '\0';
     altered = NO;
     adjustc( cursorx );
     rewrite( 0, 0 );
}
else  {
  if ( histcnt == 0 )  {
       error( " Nothing to undo " );
       return;
  }
  onpage = NO;
  do  {
    if ( --histptr < 0 )  histptr = ( HISTLEN - 1 );
    histcnt--;

    l = history[histptr].histline;

    onpage = onpage || ( l >= pfirst && l <= plast );
    storehist = NO;
    switch ( history[histptr].histtype )  {
      case HISTINSERT:
        deltp( l+1 );
        break;
      case HISTDELETE:
        if ( ( slot = pageloc[history[histptr].histp.page] ) <= 0 )
               slot = swappin( history[histptr].histp.page );
        strcpy( textb, slotaddr[slot] + history[histptr].histp.moffset );
        writ_txb( l-1, textb );
        break;
      case HISTREPLACE:
        tp[l].page    = history[histptr].histp.page;
        tp[l].moffset = history[histptr].histp.moffset;
        break;
    }
  }

  /* until command changes
   */
  while ( histcnt  &&  history[histptr].histcomm ==
          history[( histptr-1 < 0 ? HISTLEN-1 : histptr-1) ].histcomm );

  if ( l < pfirst || l > plast )
           cline = l;
  putpage();
  }
if ( cline <= lastl )  gettext( cline );
}


/* set cursorx to col. cursor nearest to col. 'x' so that
 * cursor isn't in the middle of a tab or off the end of the
 * current line
 */
void adjustc(x)
int x;
{
char c;

for( charn = 0, cursorx = 0;
     cursorx < x  && ( ( c = text[charn] ) != '\0');
     charn++, cursorx++ )
     if ( c == '\t' )
          cursorx += tabwidth -1 -( cursorx % tabwidth );
}


/* put cursorx and charn onto character 'cp' of current line
 */
void sync(cp)
int cp;
{
for( charn = 0, cursorx = 0;
     charn < cp;
     cursorx++, charn++ )
     if ( text[charn] == '\t' )
          cursorx = cursorx + tabwidth-1-( cursorx % tabwidth );
}


/* that's all */
