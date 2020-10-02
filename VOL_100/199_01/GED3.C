/*
Header:          CUG199;
Title:           Module 3 of ged editor;
Last Updated:    11/22/87;

Description:    "PURPOSE: write text to screen";

Keywords:        e, editor, ged, qed, DeSmet, MSDOS;
Filename:        ged3.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 3.0;

Refrences:
Endref;
*/

/*
e/qed/ged screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981

Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
           March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

           May       1986:  qed converted to ged         (Mel Tearle)
           August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:      ged3.c

Functions: putline, rewrite, putstatusline, putlineno, uspr,
           putpage, putmess, unmess, putstr, rewrite, calcoffset,
           resetcursor, setstatusname
*/

#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


/* write a line to the screen - directly to screen memory
 */
void putline(line,y)
int  line, y;
{
char  vbuf[SWIDTH+2];
char  *getline();
int   bright, lastcol, off, lattr, loff;

char  *p;
register int   cp, i;

lastcol = SWIDTH;  off = loff = i = 0;  bright = YES;

#ifndef  DS
  memset( vbuf, ' ', SWIDTH+1 );
#else
 _setmem( vbuf, SWIDTH+1, ' ' );
#endif

vbuf[SWIDTH+1] = '\0';

/* if block option and line in range from -> to, then not bright
 */
if ( line <= lastl )  {
     if ( blocking )  {
      if (to)  bright = !( (line >= from)  &&  (line <= to) );
          else  bright = !( (line >= (from > cline ? cline : from) )  &&
          ( line <= (from > cline ? from  : cline)) );
  }
  p = getline(line);

  if ( ( line == cline && calcoffset() ) || ( blockscroll && offset ) )  {
         lastcol += offset - 1;
         off = offset;
         if ( !bright )  makebright();  else  makedim();
         loff = lattr = attr;
         i++;
  }
  if ( !bright )  makedim();  else  makebright();

  for ( cp = 0; *p  &&  cp  < lastcol; p++ )  {
    if ( cp++ >= off )   {
         if ( *p == '\t' )
               do  {
                 if ( cp >= off  &&  cp < lastcol )
                      vbuf[i++] = ' ';
               }
               while ( ++cp % tabwidth );
          else
                 if ( *p != 10  &&  *p != 26  )
                      vbuf[i++] = *p;
    }
  }

  scr_putstr( 0, y, vbuf, attr );

  /* take care of offset indicators
   */
  if ( loff )  scr_aputch( 0, y, '', lattr );

  if ( *(p)  &&  ( cp >= SWIDTH ) ) {
     if ( bright )  makedim();  else   makebright();
     scr_aputch( SWIDTH, y, '', attr );
  }
  makebright();
  }
}


/* rewrites current line from char 'cp', col 'x', onwards
 * no re-writes on dim lines
 * ¯¯ note: rewrite (0, 0) will force an update of the current line ®®
 */
void rewrite(cp,x)
int cp, x;
{
int   i, j, k, begmark;
char  vbuf[SWIDTH+2];
char  c;

i = j = k = 0;

begmark = ( calcoffset() > 0 );

i = x - offset + begmark;

if ( i > 0 ) k = i;

if ( !x  &&  begmark )  {
     i++;  k++;
     scr_aputch( 0, cursory, '', DIM );
}

#ifndef DS
  memset( vbuf, ' ', ( SWIDTH+1 ) - k );
#else
 _setmem( vbuf, (SWIDTH+1) - k, ' ' );
#endif

vbuf[(SWIDTH+1)-k] = '\0';

while( ( x < SWIDTH+offset-begmark ) && ( ( c = text[cp++] ) != '\0' ) )  {
  if ( c == '\t' )  {
       for ( i = tabwidth-x%tabwidth; i>0 && x<SWIDTH+offset-begmark; x++, i--)
         if ( x >= offset )
              vbuf[j++] = ' ';
  }
  else if ( x++ >= offset  &&  c != 10  &&  c != 26  )
                   vbuf[j++] = c;
}
scr_putstr( k, cursory, vbuf, attr );

if ( c )
     scr_aputch( SWIDTH, cursory, '' , DIM );
}


void putstatusline(line)
int line;
{
scr_delete( 0, 0 );   putlineno( line );   scr_cursoff();

if ( caps_on() )  scr_aputs( CAPLK, 0, " Caps ", REVS );

scr_aputs( FNPOS, 0, pathname, DIM );

if ( overtype )   scr_aputs( OVERT, 0, " Overtype ", REVS );

show_time();
}


/* uses scr_aputs to move the cursor else uspr won't work right
 */
void putlineno(line)
int line;
{
int i;
static int prevline, prevcursor;

if ( blankedmess ) prevline = prevcursor = 0;

if ( prevline != line || line == 1 )  {
     scr_cursoff();
     scr_aputs( LNPOS, 0, "Line ", DIM );
     for ( i = 5-1-uspr( line, 0L ); i > 0; i-- )  scr_co( ' ' );
     prevline = line;
  }

if ( prevcursor != cursorx || cursorx == 0 )  {
     scr_cursoff();
     scr_aputs( COLPOS, 0, "Col ", DIM );
     for ( i = 4-1-uspr( cursorx, 0L ); i > 0; i-- )  scr_co( ' ' );
     prevcursor = cursorx;
  }
}


/* print 'n' as a number, return number of chars typed
 * useful in debugging
 */
int uspr(sn,ln)
int sn;
long ln;
{
int temp;
long n;

n = 0;
if ( !ln ) n = sn;
     else n = ln;

if ( n < 10 )  {
     scr_putch( ( char ) n + '0', DIM );
     return 1;
}
temp = uspr( 0, n/10 );
uspr( 0, n%10 );

return  temp+1;
}


/* display page - with current centered
 */
void putpage()
{
int  y, line;
char c;

pfirst = loc( cline, ( topline - SHEIGHT)/2 );
plast  = loc( pfirst, SHEIGHT - topline );

for ( line = pfirst, y = topline; line <= plast; line++, y++ )  {
      if ( cline == line )  {
           cursory = y;
           adjustc( cursorx );
      }
      putline( line, y );
      if ( inbufp && ( cline != 1 ) && ( cline != lastl ) )
      if ( ( ( c = inbuf[0] ) == tran[DOWNPAGE] )
               || ( c == tran[UPPAGE] ) )  return;
}
if ( y <= SHEIGHT )   scr_cls( y );
}


/* uses putstr, a special case string writer
 */
void putmess(message)
char *message;
{

scr_delete(0,0);
putstr( message );
blankedmess =  YES;

}


/* restore screen from message or window
 */
void unmess()
{
int l, i, newtop, diff;

newtop = ( screen ? window : 1 );

if ( newtop > window ) newtop = window;

if ( !( diff = topline - newtop ) )  return;

if ( diff < PAGEOVERLAP && pfirst - diff < 1 )  {
  /*    if ( blankedmess == newtop )  */
     if ( newtop  == 1 )
          return;

/*   for ( l = newtop; l < topline; l++ )
           scr_delete( 0, l );  */

     for ( l = newtop; l < topline;  )
           scr_delete( 0, l++ );

/*   blankedmess = newtop;  */
     blankedmess = YES;
     return;
}
blankedmess = NO;

if ( ( pfirst -= diff ) < 1 )  {
       topline = newtop;
       putpage();
}
else  {
  for ( l = newtop, i = 0; l < topline; l++, i++ )
        putline( pfirst + i, l );
  topline = newtop;
 }
}


/* writes all messages to screen replaced putch with scr_putch
 */
void putstr(s)
char *s;
{
int  on;
char c;

on = YES;   high();

while( *(s) )  {
  c = *s;
  switch(c)  {
    case BRIDIM :
      if ( on ) dim(); else high();
      on = !on;
      break;
    case '\n':  {
      putret();
      break;
    }
    default :
      scr_putch( c, attr );
  }
  s++;
}
high();
}


int calcoffset()
{
for ( offset = 0;
      cursorx >= SWIDTH+offset-(offset>0);
      offset += OFFWIDTH );
return  offset;
}


/* does a rewrite of screen if offsets change
 */
void resetcursor()
{
int line, y;

/* compare offsets */
if ( lastoff != calcoffset() )  {
/* ok to horizontal scroll, write everything but current line
 */
   if ( blockscroll )  {
    for ( line = pfirst, y = topline; line <= plast; line++, y++ )
          if ( line != cline )  putline( line, y );
   }
   scr_xy( 0, cursory );   rewrite( 0, 0 );   lastoff = offset;
}
scr_xy( cursorx-offset + ( offset > 0 ), cursory );
}


/* make full pathname for 'putstatusline' - truncate if too long
 */
char setstatusname()
{
char *np, c;
char temp[FILELEN];

int  i , p;

pathname[0] = '\0';

if ( filename[0] == '\0' )  return( '\0');

np = filename;

while( *(np) ) np++;             /* find end */
                                 /* find last '\',if any  */
while( ( ( c = *(--np)) != '\\')  &&  c != ':'  &&  ( np != filename ) )  {
  if ( c == '.' )  {             /* save any extension */
       if ( *(np+1) == '\0' )
            *(np) = '\0';
       strcpy( defext, np );
  }
}

if ( !( *np ) )
     strcpy( temp, filename );
else
    strcpy( temp, ++np );

#ifndef  DS
  memset( pathname, '\0', FILELEN  );
#else
 _setmem( pathname, FILELEN, '\0' );
#endif

curdrv( pathname );
getpath( pathname );

if ( ( strlen( pathname ) ) > 3 )  strcat( pathname,  "\\" );

strcat( pathname, temp );

if ( ( p = strlen( pathname ) ) > PATHLEN )  {
       strcpy( temp, pathname );
       for( i = 0; i <= PATHLEN; i++ )
            pathname[i] = temp[--p];

    pathname[i++] = '\0';
    strrev( pathname );
    pathname[0] = '*';
  }
else  {
   p = strlen( pathname );
   for ( i = 0; i < ( PATHLEN - p )/2; i++ )
         temp[i] = ' ';

   temp[i++] = '\0';
   strcat( temp, pathname );
   strcpy( pathname, temp );
  }
}

/* that's all */
