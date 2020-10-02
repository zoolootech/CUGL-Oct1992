/*
Header:          CUG199;
Title:           Module 6 of ged editor;
Last Updated:    12/01/87;

Description:    "PURPOSE: performs block commands and printing functions";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged6.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 2.51, 3.01;

References:
Endref:
*/

/*
e/qed/ged  screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981

Modified:   Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
            March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

            May       1986:  qed converted to ged         (Mel Tearle)
            August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:       ged6.c

Functions:  blockpos, blockops, putpart, listfile
            reform, firstwhite
*/


#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


int blockpos(oldpos)
int  oldpos;
{
char  c;
int   to;

if ( oldpos )  putstr( "| or |P|rev." );
     do  {
       resetcursor();
       switch ( (c = getkey()) )  {
     case DOWNKEY :
       moveline(1);
       break;
     case UPKEY :
       moveline(-1);
       break;
     case LEFTKEY :
       movechar(-1);
       break;
     case RIGHTKEY :
       movechar(1);
       break;
     case LEFTWKEY :
       moveword(-1);
       break;
     case RIGHTWKEY :
       moveword(1);
       break;
     case BOLKEY :
       sync(0);
       break;
     case EOLKEY :
       sync( strlen( text ) );
       break;
     case UPPAGE :
       movepage(-1);
       break;
     case DOWNPAGE :
       movepage(0);
       break;
     case HOMEKEY :
       if ( jumpline(lastl-cline) )  sync( strlen( text ) );
       break;
     case BOFKEY :
       if ( jumpline(1-cline) )  sync(0);
       break;
     case JUMPKEY :
       putmess( "Jump to? " );
       scans( ans, 6 );
       if ( ( to = atoi(ans) ) )
              jumpline( to-cline );
       break;
     case QWIKKEY :
       info();
       break;
     case REPKEY :
       repeat = YES;
       dofindrep(1);
       repeat = NO;
       break;
     case 'p' :
     case 'P' :
       if ( oldpos )  return  PREV;
       break;
     case ESCKEY :
       return  FAIL;
     default :
     ;
     }
       }
while ( c != CR );
return  cline;
}


/* 'K'- key block options
 * some mods to print routine - mt.
 */
int blockops()
{
int   oldcline, oldcharn, oldto, oldfrom, op;
int   l, ll, line, *t, shifts, shiftx, cp;
char  *txt, c, shift[LLIM];

puttext();

oldcline = cline;
oldcharn = charn;
oldfrom  = from;
oldto    = to;

putmess
  ( "Q|uit, |X|it/save, |R|ead file, |B|lock moves, |J|ump repeat? " );

while ( ( op = getlow() ) != 'q'  &&  op != 'x'  &&
                       op != 'r'  &&  op != 'b'  &&
                       op != 'j'  &&  op != ESCKEY );

/* if ( op == ESCKEY  )  return; */
   if ( op == ESCKEY  )  return NO;

scr_co( op );
  switch (op)  {
    case 'q':
      return('q');
    case 'x':
      return('x');
    case 'r':
      return('r');
    case 'j':
      if ( jmpto > 1 )
           jumpline( jmpto-cline );
      else
           calcjmp();
      return  (NO);
   case F7KEY:
      return  (NO);
    default:
      putmess(
        "Block: W|rite, |P|rint, |S|hift, |M|ove, |C|opy, |D|elete? " );

      while ( ( op = getlow()) != 'w'  &&  op != 'p'  &&
                            op != 's'  &&  op != 'm'  &&
                            op != 'c'  &&  op != 'd'  &&
                            op !=  ESCKEY );

   /* if ( op == ESCKEY  )  return; */
      if ( op == ESCKEY  )  return NO;

      switch (op)  {
        case 'w':
        putstr("Write");
        break;
      case 'p':
        putstr("Print");
        break;
      case 's':
        putstr("Shift");
        break;
      case 'm':
        putstr("Move");
        break;
      case 'c':
        putstr("Copy");
        break;
      case 'd':
        putstr("Delete");
        break;
    }
    from = cline;
    to = 0;
    blocking = YES;

    putmess
      ( "|Put cursor on line |end|ing block and press [return]" );

    if ( ( to = blockpos( oldto ) ) == FAIL )  {
           to = cline;
           goto abort;
    }

    if ( to == PREV )  {
         moveline( oldfrom-cline );
         from = cline;
         moveline( oldto-cline );
         to = cline;
    }

    if ( to < from )  {
         l = to;
         to = from;
         from = l;
    }

    switch (op)  {
      case 'w':
        putmess( "File to write to? " );
        if ( scans( name, FILELEN ) != ESCKEY )
           if ( exists( name ) )
                writefile( from, to, name, name, NO );
      break;

    case 'p':
      if ( !prnstat() )  goto abort;      /* add 12/13/86 */
      scr_delete( 0, 0 );
      putstatusline( from );
      listfile( from, to );
      putmess( "Printer: L|ine  |P|age  |Q|uit  " );

      while ( ( c = toupper( getkey() ) ) != 'Q' )  {
        if ( c == 'P' )
             _os( LSTOUT, 0x0c );
        else if ( c == 'L' )  {
            _os( LSTOUT, '\n');
            _os( LSTOUT, '\r');
         }
      }
      break;
    case 's':
      putmess
        ( "Delete/insert spaces/tabs| to shift line, and press [return]" );

      moveline( from - cline );
      sync(0);
      resetcursor();
      shifts = 0;
      while ( ( c = getkey() ) != CR )  {
    switch (c)  {
      case DELRIGHT:
        if ( text[0] == ' ' || text[0] == '\t' )
        deletechar(0);
        break;
      case ' ':
        insertchar(' ');
        break;
      case TAB:
        insertchar('\t');
        break;
      case ESCKEY:
        goto abort;
      }
      shift[shifts++] = c;
      sync(0);
      resetcursor();
    }
    puttext();
    for ( l = from+1; l <= to; l++ )  {
      gettext(l);
      for ( shiftx = 0; shiftx < shifts; shiftx++ )  {
      switch( ( c = shift[shiftx] ) )  {
        case DELRIGHT:
          if ( *( txt = &text[0] ) == ' ' ||  *txt == '\t' )
               while ( ( *txt = *(txt+1) ) ) txt++;
          break;
        case ' ':
        case TAB:
          if ( ( cp = strlen( text ) ) < (LLIM-1) )  {
                 for ( ; cp >= 0; cp-- )
                       text[cp+1] = text[cp];
                 text[0] = ( char ) ( ( c == ' ' )  ? ' ' : '\t');
        }
        break;
        }
        }
        altered = YES;
        puttext();
        }
        changed = YES;
      break;

    case 'd':
      for ( l = from; l <= to; l++ )  deltp( from );
      gettext( loc ( from, -1 ) );

      /* charn = adjustc( oldcharn ); */
      adjustc( oldcharn );

      from = to = 0;
      changed = YES;
      break;

    case 'm':
    case 'c':
      putmess
      ( "|Put cursor on |line under which  block is to go |and press [return]" );
      if ( (cline = line = blockpos(0)) == FAIL )  {
      cline = oldcline;
      break;
      }
      for ( l = from; l <= to; l++ )  {
    if ( (line = writ_txb( line, getline(l)) ) == FAIL )  break;
    if ( op == 'm' )  {
         deltp( ( l < line ? l : l + 1 ) );
    if ( to < line )  {
          to--;
          l--;
          line--;
          cline--;
     }
    }
    else {
    if (to >= line)  to++;
    if (l >= line)   l++;
    if (l == cline)  l = line;
    }
    }
    from = cline + 1;
    to = line;
    changed = YES;
    break;
    }
abort:
    blocking = NO;
    switch (op)  {
      case 'w':
      case 'p':
      case 's':
    cline = oldcline;
    charn = oldcharn;
    putpart(from,to);
    break;
      case 'd':
     /* putpart(cline,cline+SHEIGHT); */
    putpart( from, to );
    break;
      case 'm':
      case 'c':
    putpage();
    break;
    }
    return NO;
    }   /* outer case */
}       /* func blockops */


void putpart(start,fin)
int  start, fin;
{
int l, y;

if ( start < pfirst )  putpage();
else
  for ( l = start, y = topline+(start-pfirst);
        l <= fin && y <= SHEIGHT;
        l++, y++)  {
        if ( l == cline )  cursory = y;
        putline(l,y);
  }
}


/* printer routine
 */
void listfile(from,to)
int  from, to;
{
int  cp, i, k, l;
char *t;

k = 0;
puttext();
  for ( l = from; l <= to; l++, k++ )  {
      scr_cursoff();
      if ( l%10 == 0  && l > 1 )  putlineno(l);
      if ( k >= PAGELEN - NLINES )  {
           for ( i = k; i < PAGELEN; i++ )  {
                 _os( LSTOUT, '\r' );
                 _os( LSTOUT, '\n' );
           }
           k = 0;
      }
      for ( cp = 0, t = getline(l); *t; t++ )
      if ( *(t) == '\t')
      for ( i = tabwidth-cp%tabwidth; i>0 ; cp++, i-- )
             _os( LSTOUT, ' ' );
      else  {
        if ( *(t) > 0x19)    /* send control char */
             _os( LSTOUT, *t);
      else  {                /* for OKI & CITOH */
        switch (*t)  {
          case (0x08):
          case (0x0a):
          case (0x0c):
          case (0x0d):
          case (0x18):
           _os( LSTOUT, *t );
          break;
        default:  {
           _os( LSTOUT, '^' );
           _os( LSTOUT, *t+64 );
        }
     }
    }
    cp++;
    }
    _os( LSTOUT, '\r');
    _os( LSTOUT, '\n');

    if ( testkey() == ESCKEY )  {
         error( "Listing aborted " );
         return;
    }
  }
}


/* reformat a block to right margin
 * supplied by J.W. Haefner
 */
void reform(from,to)      /* reformat lines to right margin */
int from;
int to;
{
int curline;         /* current line to reform */
int inline;          /* line to writ_txb a new line after */
int cpos;            /* current line posititon */

char rbuf[LLIM], *rbufp;    /* buffer to hold reformed line */
char *txtp;                 /* pointer to global text */
char *posp;                 /* position in reformed line */
unsigned int diffn;         /* difference of 2 pointers */
char achar;
char *firstwhite();         /* see following function */

inline  = to;
rbuf[0] = '\0';       /* zero reform buffer */
rbufp   = rbuf;       /* set pointer to beginning of reform buffer */
txtp    = text;       /* set pointer to external text buffer */
cpos    = 0;

for ( curline = from; curline <= to; curline++ )  {
  gettext( curline );         /* get current line - store it in text */
    if ( !(*(txtp)) )  {                 /* if text ptr -> to nothing */
       if ( *(rbufp ) )  {               /* if buffer ptr -> to something */
            *rbufp = '\0';               /* tag buffer with end char */
            writ_txb( inline++, rbuf );  /* add new reformatted line */
            rbuf[0] = '\0';              /* set reform buffer to empty */
            rbufp = rbuf;                /* set reform ptr -> to rbuf */
            cpos = 0;                    /* set char counter to zero */
       }
    writ_txb( inline++, text );          /* else write empty line */
    }

/* needs to handle lines less then rmargin */

  while ( *(txtp) )  {
    achar = *txtp;
    if ( ( cpos <= rtmarg ) || ( ( achar == ' ') || (achar == '\t') ) )  {
                          {
           *rbufp = achar;
            rbufp++;
        }
        if ( achar == '\t' )
             cpos = cpos+tabwidth-1-( cpos%tabwidth );
        else cpos++;
        txtp++;
        if ( ( curline == to )  &&  !(*(txtp)) )  goto  lastref;
    }

    if ( ( cpos >= rtmarg ) )  {
       if ( ( posp = firstwhite( rbuf, cpos, rbufp ) ) == rbuf )
              posp = rbufp;
       #ifndef  DS
         diffn  = *rbufp - *posp;
         rbufp  = rbufp - diffn;
         txtp   = txtp  - diffn;
       #else
         diffn  = rbufp - posp;
         rbufp  = rbufp - diffn;
         txtp   = txtp  - diffn;
       #endif
lastref:
       *rbufp = '\0';
       writ_txb( inline++, rbuf );    /* add new reformatted lines */
       rbuf[0] = '\0';
       rbufp = rbuf;
       cpos = 0;
    }               /* endif */
  }                 /* endwhile */

if ( cpos > 0  &&  text[0] != ' ' )  {       /* and one for good luck */
     *rbufp = ' ';
     rbufp++;
     cpos++;
}
txtp = text;
}                   /* endfor */

/* delete old unformatted lines
 */
for ( cpos = from; cpos <= to; cpos++ )  deltp( from );
blocking = NO;
gettext( loc ( from, -1 ) );
sync(0);
resetcursor();
}


/* find first white space to break line on
 */
char *firstwhite(s,cp,tp)
char s[];                        /* string to search */
int  cp;                         /* cursor pos and string (text) pos */
char *tp;
{
int  cpos;
char *posp, *lastnwht;
char c;

/* original
    for (cpos=cp,posp=tp;posp>=s && (((c=(*posp & 0x7f) != ' ') && c!='\t')
    || cpos>rtmarg);cpos--)
    if (c!='\t') posp--;
    else
       if (!((cpos-1)%tabwidth)) posp--;
    return (++posp);
*/

lastnwht = tp;
posp = tp - 1;

while ( ( ( ( c = *(posp) ) != ' ' && c != '\t' ) ||
                   cp > rtmarg ) && posp >= s )  {
   switch(c)  {
     case '\t':
       if ( !(cp-1)%tabwidth )  posp--;
       break;
     case ' ':
       posp--;
       break;
     default :
       lastnwht = posp--;
       break;
   }           /* endswitch */
   cp--;
}              /* endwhile */
return( lastnwht );
}
