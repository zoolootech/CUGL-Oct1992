/*
Header:          CUG199;
Title:           Module 4 of ged editor;
Last Updated:    12/02/87;

Description:    "PURPOSE: perform find, alter and repeat commands";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged4.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 2.51;

References:
Endref
*/

/*
e/qed/ged  screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981

Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
           March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

           May       1986:  qed converted to ged         (Mel Tearle)
           August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:      ged4.c

Functions: info, findorrep, dofindrep, find

*/

#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif

/* quick key services
 */
void info()
{
int  line, xcursor;
char c;

xcursor = getcursor();
putmess( "F|ind, |A|lter, |R|eform, |Y|(DEL-EOL), |S|ervices" );

while ( (c = getlow()) != 'f'  &&  c != 'a'  &&
                     c != 'r'  &&  c != 'y'  &&
                     c != 's'  &&  c != ESCKEY );

if ( c == ESCKEY )  return;

switch (c)  {
  case 'f':
    replace = NO;
    findorrep();
    break;
  case 'a':
    replace = YES;
    findorrep();
    break;
  case 'y':
    scr_delete( ( cursorx > SWIDTH ) ? xcursor : cursorx, cursory );
    altered = YES;
    text[charn] = '\0';
    changed = YES;
    break;
  case 's':
    envir();
    show_fkeys();
    break;
  case 'r':
    puttext();
    if ( !text[0] )       /* empty line - skip it */
      moveline(1);
    else  {               /* make from -> to */
      from = to = line = cline;
      if ( cline != lastl )
      do  {
         gettext( ++line );
         to++;
      }  while ( cline <= lastl  &&  text[0] != '\0' );
      else to++;

    /* "to" must be 1 more than text to reform
     */
    reform( from, to );
    changed = YES;
    putpage();
    }
  break;
  }
}


/* find and replace
 */
void findorrep()
{
int  toend, global, i, count;
char c;

putmess( "Find|(^]=esc)|? " );

global = toend = NO;  nocheck = NO;

findir = 1;
count  = 0;

c = ( char )( scans( patt, FLIM ) );

if ( replace )  {
     if ( c == ESCKEY )  return;
     putmess( "Alter to|(^]=esc)|? " );
     c = ( char ) ( scans( changeto, FLIM ) );
}
else if ( !patt[0] )  return;

if ( c == CR )  {
     if ( replace )
          putmess
          ( "B|ack/|G|lobal/|T|o end (start)/|W|ithout asking/number |? " );
     else putmess
          ( "|Search |B|ackwards/number|? - ESC to halt " );
     if ( scans( opts, 5 ) == ESCKEY )   return;

     for ( i = 0; ( c = opts[i]); i++ )  {
       switch(tolower(c)) {
         case 'g':
           global = YES;
         case 't':
           toend = YES;
           break;
         case 'b':
           findir = -1;
           break;
         case 'w':
           nocheck = YES;
           break;
         }
       if ( c >= '0' && c <= '9' )  count = count*10+c-'0';
       }
    if ( !replace )  {
         global = NO;
         toend  = NO;
    }
    if ( count == 0 )  {
         if (toend)  count = MAXINT;
         else count = 1;
    }
    if ( global )  {
         findir = 1;
         moveline(1-cline);
         sync(0);
    }
    }
    else count = 1;
    dofindrep(count);
}


void dofindrep(count)
int count;
{
int cp, i, len;
char c;

puttext();
do  {
  count--;
  if ( find() == FAIL )  count = 0;
       else if ( replace )  {
       if ( nocheck ) c = 'y';
      else  {
          scr_curson();
          putmess( "|Replace |{|Y|/|N|}|? " );
          do  {
            scr_xy( REPPOS, 0 );
            for (i = 0; i < 1250; i++);
            resetcursor();
            for (i = 0; i < 1250; i++);
          }
          while ( ( c = testlow() ) != 'y' &&
                    c != 'n' && c != ESCKEY );
      }
      scr_delete( EMPOS, 0 );
      switch(c)  {
        case 'y' :
          if ( strlen(text)+(len=strlen(patt)) >= LLIM )  {
           error( "Line would be too long " );
           return;
          }
          for ( cp = charn; (text[cp] = text[cp+len]); cp++ );

          for ( cp = strlen(text), len = strlen(changeto);
                cp >= charn;
                cp-- )
                text[cp+len]=text[cp];
          for ( i = 0; (c=changeto[i]); i++ )  text[charn++] = c;
          altered = YES;
          puttext();
          rewrite( ++cp, cursorx );
          sync(charn);
          changed = YES;
        break;
       case ESCKEY:
         count = 0;
         error( "Search stopped " );
       case 'n' :
         movechar(findir);
         break;
       }
     }
  }
  while(count);
  inbufp = 0;
}


/* find 'patt', searching backwords ( findir==-1)
 * or forwards (findir==1) from current line
 * return FAIL or YES, (set current line to line containing pattern)
 * no wild cards
 */
int find()
{
int   i, fline, oldcharn, newcharn, interupt;
char  *s, pattch1, *p, *t, *getline();

if ( !replace || repeat )  movechar(findir);

fline       = cline;
oldcharn    = charn;
interupt    = NO;
pattch1     = patt[0];

/* searches forward beginning with current char if not tof
 */
if ( findir == 1 )
     while ( fline <= lastl )  {
       for ( s = getline(fline)+charn; *( p = s ); charn++, s++ )
         if ( *s == pattch1)  {
           for ( t = patt+1, p++; *t && *p == *t; p++, t++ );
           if ( !*t )  goto  foundit;
         }
       chkbuf();
       if ( !inbufp )  {
             fline++;
             charn = 0;
       }
       else
       if ( testkey() == ESCKEY )  {
            inbufp = 0;
            interupt = YES;
            goto  interrupted;
       }
    }
/* searches backwards
 */
    else
    while ( fline >= 1 )  {
      for ( s = getline(fline); charn >= 0; charn-- )
        if ( *( p = &s[charn]) == pattch1 )  {
         for ( t = patt+1, p++; *t && *p == *t; p++, t++ );
         if (!*t)  goto  foundit;
        }
     chkbuf();
     if ( !inbufp )
      charn = strlen( getline(--fline) )-1;
     else
       if ( testkey() == ESCKEY )  {
        inbufp = 0;
        interupt = YES;
        goto  interrupted;
       }
    }
interrupted:
  charn = oldcharn;
  if ( !replace || repeat )
       movechar( -findir );
  if ( interupt )
       error( "Search aborted " );
  else error( "Search fails " );
       return  FAIL;

foundit:
  newcharn = charn;
  moveline( fline - cline );
  sync( charn = newcharn );
  return  YES;
}





