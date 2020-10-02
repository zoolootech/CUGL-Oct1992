/*
Header:          CUG199;
Title:           Module 1 of ged editor;
Last Updated:    12/02/87;

Description:    "PURPOSE:  environment; change files";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged1.c;
Warnings:       "O file must be present for link of ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 3.0;

References:
Endref;
*/

/*
e/qed/ged  screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 -  August-December 1981

Modified:   Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
            March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

            May       1986:  qed converted to ged         (Mel Tearle)
            August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:       qed1.c

Functions:  system, dirmsk, getcurdir, files, putonoff
            getnumb, getlow
*/


#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


/* execute a program from within ged and do clean-up for screen
 */
void run(display,program,flag)
char *display;
char *program;
int   flag;
{
int  i = 1;
char *s[6];  /* six options */

scr_delete( 0, 0 );  putstr( display );

if ( dirmsk() == YES )  {
     scr_delete( 0, 0 );
     goto  quit;
}

scr_delete( 0, 24 );  scr_cls(1);

#ifndef DS
  s[i] = strtok( patt, " " );
  while ( s[i++] != NULL )  {
          s[i] = strtok( NULL, " " );
  }
  spawnv( P_WAIT, program, s );
#else
  exec( program, patt );
#endif

getkey();
scr_delete( 0, 24 );  scr_clr();

quit:
  topline = 1;
  if ( screen )  show_table( screen );
  if ( flag  )  {
       blankedmess = YES;
       putpage();
       putstatusline( cline );
       show_fkeys();
  }
}


/* display function key definitions at bottom of screen
 */
void show_fkeys()
{
scr_putstr( 0, 24,
"F1Help ÛF2Time ÛF3Left ÛF4Rght ÛF5Save ÛF6Ls ÛF7More ÛF8Grep ÛF9Graph ÛF10Reset ",
REVS );
}


/* returns dir mask using patt for directory search
 */
char dirmsk()
{
char *pattp;

pattp = patt;
if ( scans( patt, FILELEN ) == ESCKEY )  return  YES;
if ( !*pattp )  pattp = "*.*";
return NO;
}


/* display or re-set ged's startup globals
 */
void envir()
{
putonoff( autoin );

puttext();
scr_clr();
blankedmess = YES;

putstatusline( cline );
scr_delete( 0, 24);
show_envir();

do {
  switch( getlow() )  {
      case 'a':
        autoin = ( char ) !autoin;
        show_envir();
        break;
      case 'b':
        backup = ( char ) !backup;
        show_envir();
        break;
      case 'h':
        blockscroll = ( char ) !blockscroll;
        show_envir();
        break;
      case 'r':
        scr_xy( 0, 18 ); putstr( "|Enter new |R|ight Margin: " );
        if ( !( rtmarg = getnumb() ) )   rtmarg = REFORM;
        show_envir();
        break;
      case 't':
        scr_xy( 0, 18 ); putstr( "|Enter new |tab width|: " );
        if ( !( tabwidth = getnumb() ) )  tabwidth = 0;
        show_envir();
        break;
      case 's':
        trail = ( char ) !trail;
        show_envir();
        break;
      case 'f':
        if ( files() )  goto escape;
        break;
      case 'q':
      case ESCKEY:
      case F10KEY:
        goto escape;
      }
}
while (YES);

escape:
  scr_clr();
  topline = 1;
  if ( screen )  show_table( screen );
  putpage();
  blankedmess = YES;
  putstatusline( cline );
}


void show_envir()
{
struct addr anaddr;

  scr_xy(0,4);
  putstr("Edit context:\n\n");
  putstr("  |enter |F| for file context\n\n");
  putstr("A|uto indent:                    ");
  putonoff(autoin);
  putstr("|make |B|ackup of original file:   ");
  putonoff(backup);
  putstr("T|ab stops every                  ");
  uspr(tabwidth,0L);
  putstr("\nS|trip trailing blanks,tabs      ");
  putonoff(!trail);
  putstr("H|orizontal scroll whole page    ");
  putonoff(blockscroll);
  putstr("R|ight Margin                    ");
  uspr(rtmarg,0L);
  putstr("\n|Memory allows for no more than ");
  uspr( ((slotsinmem-2)*(PAGESIZE/(sizeof(anaddr)))) ,0L);
  putstr("| lines of text");

  scr_xy( 0, 16 ); putstr("To change, enter capital letter, or enter |Q| to return: ");
  scr_delete( 0, 18 );
}


int files()
{
char oldname[15], newname[15], dc, c;
char curdir[64], *curdirp, *tp;
int  dir;

do  {
   scr_clr();  putstatusline( cline );   scr_delete( 0,24 );
newcomm: 
  dir = curdsk;
  errmess = NULL;
  scr_xy( 0, 1 );
  putstr( "Q| to return to Edit context  |D| to delete a file\n" );
  putstr( "R| to rename a file           |P| to change/show path \n" );
  putstr( "C| to change the name of the edited file " );
  putstr( filename );
  c = getlow();
  switch (c)  {
    case ESCKEY:
      return  YES;
    case 'q':
      return  NO;
    case 'p':
      tp = curdirp = curdir;
      *curdirp = 0;
      scr_delete( 6, 4 );
      putstr( "Drive: " );

/*  if ( ( c = getscankey() ) == CR || c == ESCKEY )
                 dir++;
 */
      if ( ( c = getkey() ) == CR )
             dir++;
      else dir = ( toupper(c)-'@' );        /*A=1,B=2...*/

      #ifdef  DS
        if ( getdir( dir, curdirp ) == FAIL )  goto newcomm;
      #else
        if ( getcwd( curdirp, FILELEN ) == NULL )  goto newcomm;
      #endif

      scr_delete( 6, 4 );
      putstr( "|Current directory: |" );

      #ifndef  DS
        putstr( curdirp );
      #else
        scr_co( ( dir + '@' ) );
        putstr( ":\\" );
        putstr( curdirp );
      #endif

      putstr( "|  Enter new directory: |" );
      scans( curdir, FILELEN );

      if ( !( *curdirp ) )  goto  newcomm;      /* drive specified? */

/*    while( ( dc = *(tp++) ) && dc != ':' );
 *     && dc != ':' )  tp++;
 */
      while( *(tp) )  {
        if ( *tp == ':' ) break;
        dc = *tp;
        tp++;
      }

      if ( dc )  {
           if ( ( dir = toupper( *(tp-2))-'A') < 0  ||  dir > 16 )  {
            /*if (seldisk(dir)) continue;
              curdsk = dir; */
              *curdirp = ( char ) curdsk + 'A';
           }
      }
      chdir( curdirp );
      scr_delete( 6, 4 );
      goto  newcomm;
    default:
            if ( c == 'r' || c == 'd' || c == 'c' )  {
                 scr_delete( 6, 4 );
                 putstr( "Name of file: " );
                 scans( oldname, FILELEN );
                 if ( !oldname[0] )  goto  newcomm;
                 format( oldname );
             }
            switch (c)  {
              case 'd':
                if ( funlink( oldname ) == FAIL )  {
                     error( "Can't delete" );
                     goto  newcomm;
                }
                break;
              case 'r' :
                scr_delete( 6, 5 );
                putstr( "New name:     " );
                scans( newname, 15 );
                format( newname );
                if ( !newname[0] || frename( oldname, newname ) == FAIL )  {
                     error( "Can't rename" );
                     goto newcomm;
                }
                break;
              case 'c'  :
                strcpy( filename, oldname );
                setstatusname();
                putstatusline( cline );
              default:
                goto  newcomm;
              }
           }

        } 
while (YES);
}


void putonoff(flag)
int  flag;
{
putstr( (flag) ? "ON\n" : "OFF\n" );
}


int getnumb()
{
int i, n;
char c, numb[5];

scans( numb, 5 );
n = 0;
for ( i = 0; i < 5; i++ )  {
      c = numb[i];
     if ( c >= '0' && c <= '9' )  n = n*10 + c - '0';
}
return n;
}


/* that's all */
