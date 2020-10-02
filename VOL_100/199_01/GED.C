/*
Header:           CUG199;
Title:            Module 0 of ged editor;
Last Updated:     12/06/87;

Description:     "PURPOSE: initialise; process commands";

Keywords:         e, editor, qed, ged, DeSmet, MSDOS;
Filename:         ged.c;
Warnings:        "O file must be present during link of ged";

Authors:          G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:        DeSmet 3.0;

References:
Endref;
*/

/*
e/qed/ged screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 -  August-December 1981

Modified:   Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
            March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

            May       1986:  qed converted to ged         (Mel Tearle)
            August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:       ged.c

Functions:  main, initialise, edit, finish, dispose, xit,
            askforfile, seldisk
*/

#include "ged.h"

#ifdef MSC
jmp_buf  mark;
#endif

void main(argc,argv)
char **argv;
int argc;
{
int argn;
int i;
char *dig;

/* --------------------- Default option settings ---------------------*/

initjmp =    1;     /* jmp to line 1 */
autoin  =  YES;     /* auto indent  [YES/NO] */
backup  =  YES;     /* make ".BAK" file [YES/NO] */


/***********************************************************************/
/*** DON'T CHANGE *** DON'T CHANGE *** DON'T CHANGE *** DON'T CHANGE ***/
/***********************************************************************/

   readall =  YES;     /* read as much text as possible up to memory */

/***********************************************************************/
/*** DON'T CHANGE *** DON'T CHANGE *** DON'T CHANGE *** DON'T CHANGE ***/
/***********************************************************************/


trail   =   NO;     /* don't strip trailing blanks [YES/NO] */

tabwidth    =   4;  /* tab stops every n cols [number] */
blockscroll = YES;  /* don't horizontal scroll whole page, just current line */
pagingdisk  =   0;  /* create buffer file on this disk -
                       set to either 0 (for currently logged-in disk)
                       or to desired disk letter  (eg 'B') */

rtmarg    =  REFORM;   /* default for reformat only - no wordwrap */
defext[0] =    '\0';   /* default extension */

window = WINDOW;

/* ------------------ End of default option settings -------------------- */


if ( ( _os( VERNO, 0 ) & 0x00ff ) < 2 )  {
       putstr( "Ged Error: Must use MSDOS vers 2.x with 'ged'" );
       exit(0);
}


inbufp = 0;
filename[0] = name[0] = '\0';

argn = 0;
while ( ++argn < argc )
  if ( *argv[argn] == '-' )  {
        dig = argv[argn]+1;
        switch( toupper(*dig) )  {
           case 'A' :
                autoin = ( char ) !autoin;
                break;
           case 'B' :
                backup = ( char ) !backup;
                break;
           case 'Q' :
                readall = ( char ) !readall;
                break;
           case 'H' :
                blockscroll = ( char ) !blockscroll;
                break;
           case 'T' :
                tabwidth = 0;
                while (*++dig) tabwidth = tabwidth*10+*dig-'0';
                break;
           case 'S' :
                trail = ( char ) !trail;
                break;
           case 'D' :
                pagingdisk = toupper(*(dig+1));
                if ( pagingdisk >= 'A' && pagingdisk <= 'P' )
                     break;
           default  :
                if ( isdigit( *dig ) )  {
                     initjmp = atoi( (argv[argn]+1 ) );
                     break;
                }
                putstr( "Ged Error: Illegal option: " );
                putstr( argv[argn] );
                exit(1);
                }
  }
  else strcpy( filename[0] ? name : filename, argv[argn] );

ans[0] = patt[0] = changeto[0] = opts[0] = '\0';


/* ----------------------  start here  -------------------- */

keytranslate();

do  {
  initialise();
  edit();
}
while (YES);
}

/* -----------------------  end main   ------------------- */


void initialise()
{
int i;

cursorx  = charn   = offset = lastoff = from = 0;
histptr  = histcnt = ncommand = to = 0;

cursory  = cline = topline  = lastread = lastl = findir = jmpto = 1;

changed  = blocking = isdim = repeat = NO;
replace  = blankedmess = NO;
screen   = NO;

if ( overtype )  ;  else  NO;

altered   = goteof = YES;
errmess   = '\0';
lastgraph = NULL;

pfirst   = -100;    text[0]  = '\0';


curdsk = _os( CURDSK, 0 );


initvm();
scr_setup();
scr_cursoff();

if ( filename[0] )  {
     scr_clr();

scr_putstr( 11, 10,
   "ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออป", RSFW );
scr_putstr( 11, 11,
   "บ   qed editor version 4.6b  MICROLOGY 1983 - JWH 1985    บ", RSFW );
scr_putstr( 11, 12,
   "บ        m/ged version 2.00  Mel Tearle 1987              บ", RSFW );
scr_putstr( 11, 13,
   "บ                    ฏ ged - 12/06/87 ฎ                   บ", RSFW );
scr_putstr( 11, 14,
   "ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ", RSFW );
scr_delete( 0,24 );


     /* open a file, if it fails then try again to
      * open it using filename plus default extension
      */
     while ( opentext( filename ) == FAIL )  {
             askforfile();
             if ( !filename[0] )  goto  newfile;
     }
     lastl = UNKNOWN;  lastread = 0;  goteof = NO;

     if ( name[0] )  {
          strcpy( filename, name );
          name[0] = '\0';
     }
}       /* end - if filename */

newfile:
  errmess = NULL;

  format( filename );


 /* read as much text to fill memory or end of file
  */
 if ( readall )
      for ( i = 1; (! ( usage[slotsinmem-1] ) && ( !( goteof ) ) ); i += 100 )
            readtext(i);

 gettext(1);
 scr_clr();

 if ( initjmp > 2)  {              /* (2-1) is a bad jump from line 1 */
      jumpline( initjmp-cline );   /* not possible to do init jump to 2*/
      initjmp = 0;
 }
 else
     putpage();

setstatusname();
putstatusline( cline );

show_fkeys();
}                  /* end - initialise  */


/* command processor
 */
void edit()
{
unsigned char c;

int  inc, i, to, oldcrx;

#ifdef  MSC
  setjmp( mark );
#else
  setjmp(0);
#endif

while ( YES )  {
  goodline = cline;     /* restore environment */
  storehist = YES;      /* makesure we're saving history */
  unmess();             /* check if topline has changed */

  if ( blankedmess )  {
       putstatusline( cline );
       blankedmess = NO;
  }
  else
       putlineno( cline );

  resetcursor();
  scr_curson();
  c = getkey();

  if ( ( ncommand++ % 32 ) == 0 )   show_time();

  if ( errmess != NULL )  errmess =  NULL;
    switch(c)  {
      case UPPAGE :
           movepage(-1);
           break;
      case DOWNPAGE :
           movepage(0);
           break;
      case DOWNKEY :
           moveline(1);
           break;
      case UPKEY :
           moveline(-1);
           break;
      case SCRLDNKEY :
           oldcrx = cursorx;
           to = plast - cline;
           moveline(to);
           inc = ( ( moveline(1) == YES ) ? 1 : 0 );
           cursorx = oldcrx;
           moveline( ( ( cline - to ) <= pfirst )
                     ? ( topline - cursory ) : ( -to - inc ) );
           break;
      case SCRLUPKEY :
           oldcrx = cursorx;   /* save current cursorx */
           to = topline - cursory;
           moveline(to);
           inc = ( ( moveline(-1) == YES ) ? 1 : 0 );
           cursorx = oldcrx;
           moveline( ( ( cline - to ) >= plast )
                     ? ( SHEIGHT - cursory ) : ( -to + inc ) );
           break;
      case DELLNKEY :
           changed = YES;
           text[0] = '\0';
           crdelete( ( cline == lastl ? -1 : 0 ) );
           break;
      case BLOCKKEY :
        switch ( to = blockops() )  {
          case YES :  return;
                 /*   break;  */
          case 'x' :
          case 'q' :  if ( finish(to) == YES )  return;
                      break;
          case 'r' :
                      putmess( "F|ile to read? " );
                      if ( ( scans( name, FILELEN ) ) == ESCKEY )  break;
                      if ( *(name) )  {
                           readfile( name );
                           changed = YES;
                           putpage();
                      }
                      break;
        }
        break;
      case QWIKKEY :    /* all ^Q functions */
           lastgraph = NULL;
           info();
           break;
      case BOLKEY :
           sync(0);
           break;
      case EOLKEY :
           sync( strlen( text ) );
           break;
      case HOMEKEY :    /* last line of file */
           if ( jumpline( lastl - cline ) )  {
                sync( strlen( text ) );
           }
           break;
      case BOFKEY :     /* first line of file */
           if ( jumpline( 1 - cline ) )  sync( 0 );
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
      case DELLEFT :
           changed = YES;
           deletechar(-1);
           break;
      case DELRIGHT :
           deletechar(0);
           changed = YES;
           break;
      case DELWDKEY :
           changed = YES;
           deleteword();
           break;
      case JUMPKEY :
           makedim();
           putmess( "J|ump ([+/-]n,|RET|=crnt) ? " );
           scans(ans,6);
           makebright();
           if ( !*ans )  jmpto = cline;
              else
                calcjmp();
           break;
      case TOPSCRKEY :
           moveline( ( topline-cursory ) );
           break;
      case BOTSCRKEY :
           moveline( ( SHEIGHT-cursory ) );
           break;
      case REPKEY :
           if ( lastgraph )  {
                changed = YES;
                insertchar( lastgraph );
           }
           else  {
                repeat = YES;
                dofindrep(1);
                repeat = NO;
           }
           break;
      case HELPKEY :
      case F1KEY :             /* help menu */
         switch ( screen )  {
           case HELP :
             screen = NO;
             break;
           default :
             show_table( HELP );
           }
           break;
      case CR :
           changed = YES;
           crinsert(0);
           break;
      case CRSTILL :
           changed = YES;
           crinsert(-1);
           break;
      case OOPSKEY :
           undo();
           break;
      case TAB :
           changed = YES;
           insertchar('\t');
           break;
      case ESCKEY :
           if ( screen )  {
                screen = NO;
                lastgraph = NULL;
                puttext();
           }
           else {
             resetcursor();
             while ( !( c = inkey() ) );
               if ( c < ' ' ) ;                   /* c already CTRL */
                  else
                    if ( c < '@' ) break;         /* c not CTRL char */
                        else                      /* c either upper or lower */
                        c -= ( ( char ) ( ( c < '`' ) ? 64 : 96 ) );
           }
           break;
      case ALTKEY :
           if ( screen )  {
                lastgraph = get_graphic();
                changed = YES;
                insertchar( lastgraph );
           }
           else
                get_graphic();
           break;
      case INSKEY :
           if ( overtype  )  {
                overtype = NO;
                scr_aputs( OVERT, 0, "          ", HIGH );
           }
           else {
                overtype = YES;
                scr_aputs( OVERT, 0, " Overtype ", REVS );
           }
           break;
      case CAPSLKON :
           scr_aputs( CAPLK, 0, " Caps ", REVS );
           break;
      case CAPSLKOFF :
           scr_aputs( CAPLK, 0, "      ", HIGH );
           break;
      case F2KEY :                     /* update time */
           show_time();
           break;
      case GREYMINUS :
      case F3KEY :                     /* move cursor word left */
           moveword(-1);
           break;
      case GREYPLUS :
      case F4KEY :                     /* move cursor word right */
           moveword(1);
           break;
      case F5KEY :
           to = 'k';
           finish(to);
           break;
      case F6KEY :
           #ifdef DS
             getenv( "LS", envstr );
           #else
             envstr = getenv( "LS" );
           #endif

           run( "ls ", envstr, 1 );
           break;
      case F7KEY :
           #ifdef DS
             getenv( "MORE", envstr );
           #else
             envstr = getenv( "MORE" );
           #endif

           run( "more ", envstr, 1 );
           break;
      case F8KEY :
           #ifdef DS
             getenv( "GREP", envstr );
           #else
             envstr = getenv( "GREP" );
           #endif

           run( "grep ", envstr, 1 );
           break;
      case F9KEY :
         switch ( screen )  {
           case  TABLE1 :
             show_table( TABLE2 );
             break;
            default :
             show_table( TABLE1 );
           }
           break;
      case F10KEY :
           envir();
           show_fkeys();
           break;
      default :
           changed = YES;
           insertchar(c);
           break;
      }
  }
}


/* No to return to current file or don't return,
 * but exit if finished altogther
 * return Yes to edit another file
 */
int finish(o)
int o;
{
int abandon;
char c, tempname[FILELEN], namebak[FILELEN];

if ( o != 'x' &&  o != 'k' )  {
   putmess
     ( "W|rite edited text to file, |A|bandon all edits, |Esc|ape to return " );
   while ( ( c = getlow() ) != 'w' && c != 'a' && c != ESCKEY );
   if ( c == ESCKEY )
        return  NO;
   abandon = c == 'a';
}

if ( o == 'k' )  c = 'w';

if ( ( c == 'w' ) || ( o == 'x' )  )  {
   if ( ( !filename[0] ) )  {
      putmess( "F|ile to write to? " );
      if ( ( scans( filename, FILELEN ) ) == ESCKEY )
             return NO;

      format( filename );

      if ( filename[0] <= ' '  ||  ( !backup && !exists( filename ) ) )  {
           filename[0] = '\0';
           return  NO;
      }
   }
   if ( backup )  {   /* delete old bak file */
        strcpy( namebak, filename );
        retag(  namebak, "BAK" );
        if ( checkexists( namebak ) )
        if ( funlink( namebak ) == FAIL )  return  NO;
   }
   /* keep old name in 'filename'
    */
   strcpy( tempname, filename );
   /* new file called 'filename.@@@ ' - or whatever you like
    */
   retag( tempname, "@@@" );
   if ( writefile( 1, lastl, tempname, filename, YES ) == FAIL )
        return  NO;
   /* check original file still exists -
    * may have been deleted or renamed by user
    */
   if ( checkexists( filename ) )  {
      if ( backup )  {
       /* orig. file becomes '.bak'
        */
      if ( frename( filename, namebak ) == FAIL )
           goto failed;
   }
   else  {
      /* delete orig file
       */
       if ( funlink(filename) == FAIL )  {
  failed:    /* if can't delete/rename old file, change
              * new name to '.@@@'
              */
           strcpy( filename, tempname );
           goto nowrite;
           }
        }
      }
     /* new file goes from '@@@' to orig name
      */
      frename( tempname, filename );
      changed = NO;     /* file now not changed */
      }
    if ( o == 'x' )  {
        if ( pagefd != NOFILE )  {
             close( pagefd );
             funlink( pagingfile );
        }
        xit();
    };

nowrite:

if ( o != 'k' )  {
    putmess( "|e|X|it to DOS, |A|nother file, |Esc|ape to return " );
    while ( ( c = getlow() ) != 'x' && c != ESCKEY && c != 'a' );
}
else
   c = ESCKEY;

  switch(c)  {

    case 'x' :
      if ( changed )
         if ( !dispose() )  return  NO;
      ffclose( ( struct iobuffer * ) textbuf );
      if ( pagefd != NOFILE )  {
           close( pagefd );
           funlink( pagingfile );
      }
      xit();

    case 'a' :
      if ( changed )
         if ( !dispose() )  return  NO;
      ffclose( ( struct iobuffer * ) textbuf );
      if ( pagefd != NOFILE )  {
           close( pagefd );
           funlink( pagingfile );
      }
      askforfile();
      return  YES;

    case ESCKEY :
      if ( !abandon )  {
          scr_xy( WAITPOS, 0 );
          opentext( filename );
          lastl    = UNKNOWN;
          lastread = 0;
          goteof   = NO;
          initvm();
          gettext( cline );
          errmess  = NULL;
          putstatusline( cline );
      }
  }
return  NO;
}


/* check to dispose of modified file
 */
int dispose()
{
putmess( "A|bandon modified file?| (y/n) ");

if ( getlow() == 'y' )  return  YES;
return  NO;
}


/* quit editor and leave cursor at bottom of screen
 */
void xit()
{
scr_delete( 0, 24 );
scr_xy( 0, 23 );
exit( 0 );
}


/* get another file to edit into 'filename'
 */
int askforfile()
{
int drive;

while( YES )  {
  scr_clr();
  dirdone:
    if ( errmess != NULL )  {
         scr_xy( EMPOS, 0 );
         putstr( errmess );
    }
  scr_putstr( 0, 0,"File to edit: ", HIGH );

  scr_putstr( 0, 24,
"   ESC to exit  '>' for directory  <CR> new file  A:-P: change default disk     ",
REVS );
  scr_curson();
  scr_xy( 14, 0 );

  if ( scans( filename, FILELEN ) == ESCKEY )  xit();

  scr_delete( EMPOS, 0 );  errmess = NULL;

  if ( filename[0] == '>' )  {
       #ifdef DS
         getenv( "LS", envstr );
       #else
         envstr = getenv( "LS" );
       #endif

       run( "ls ", envstr, 1 );
       scr_delete( 0, 0 );
       goto  dirdone;
  }

  if ( strlen(filename) == 2 &&  filename[1] == ':' )  {
     if ( ( drive = toupper( filename[0]) - 'A' ) >= 0 && drive < 16 )  {
                                    {
          if ( seldisk( drive ) )  continue;
                }
                curdsk = drive;
         }
     }
     else  {
       name[0] = '\0';
       return  YES;
     }
  }
}


int seldisk(drive)
int drive;
{

_os( SELDSK, drive );

return 0;
}


/* displays message, returns to a known good screen
 */
void fatalerror(message)
char  *message;
{
error( message );
moveline( goodline-cline );

#ifdef  MSC
  longjmp( mark, 1 );
#else
  longjmp( 0, 1 );
#endif
}
