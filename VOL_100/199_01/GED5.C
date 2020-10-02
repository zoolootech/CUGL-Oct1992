 /*
Header:          CUG199;
Title:           Module 5 of ged editor;
Last Updated:    12/02/87;

Description:    "PURPOSE: read and write files";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged5.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 3.0;

Refrences:
Endref;
*/

/*
e/qed/ged  screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981

Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
           March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

           May       1986:  qed converted to ged         (Mel Tearle)
           August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:      ged5.c

Functions: readfile, writefile, exists, checkexists, scans, retag,
           format
*/


#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif

#define  GS   0x1d
#define  ESC  0x1b

/* read text into current file from 'filename' with ^KR
 * if middle of line then insert text after cursor
 * else if beginning of line insert above cursor
 */
int readfile(name)
char *name;
{
int  c, i, j, k, line, first, res;
char textb[LLIM];

if ( ffopen( name, ( struct iobuffer * ) fbuf ) == FAIL )  {
     strcat( name, defext );
     if ( ffopen( name, ( struct iobuffer * ) fbuf ) == FAIL )  {
          error( " Can't find file " );
          name[0] = '\0';
          return( FAIL );
     }
}
first = YES;
i     = charn;
line  = cline;

for ( j = charn, k = 0; ( ( textb[k] = text[j] ) != '\0'); j++, k++ );
  do  {
    while ( i < LLIM  &&
      ( ( c = egetc( ( struct iobuffer * ) fbuf ) ) != DFAIL )
              &&  (c != '\n')  &&  ( c != ENDFILE ) )
      text[i++] = ( char )c;

    if ( text[i-1] == '\r' )  i--;
    text[i] = '\0';

    if ( c == ENDFILE )
         for ( k = 0;
             ( ( text[i] = textb[k] ) != '\0' ) &&  i < LLIM;
                 i++, k++ );

    if ( first )  {
         altered = YES;
         puttext();
         first = NO;
         res = YES;
    }
    else  {
      res = writ_txb( line, text );
      line++;
    }
    i = 0;
  } while ( c != DFAIL  &&  res  != FAIL  &&  c != ENDFILE );

ffclose( ( struct iobuffer * ) fbuf );
gettext( cline );
return YES;
}


int writefile(from,to,name,nametoprint,exiting)
int  from, to, exiting;
char *name, *nametoprint;
{
int  l, copying, copysecstart, copybytestart;
char c, *t, *getline();
struct iobuffer *textbuf;

puttext();
if ( name[0] <= ' ' )  {
     error("Bad name ");
     return  FAIL;
}

if ( fcreat( name, ( struct iobuffer * ) fbuf ) == FAIL )  {
     error( "Can't create file " );
     return(FAIL);
}

putmess( "S|aving: " );
putstr( nametoprint );
copying = NO;

if ( exiting )  to = lastread;
     else if ( to == lastl )  to = loc( lastl, 0 );

for ( l = from; l <= to; )  {
      t = getline( l++ );
      while ( *(t) )  {
        if ( eputc( *t, ( struct iobuffer * ) fbuf ) == FAIL )
             goto diskfull;
        t++;
      }

      if ( l <= to )  {
           if ( eputc( '\r', ( struct iobuffer * ) fbuf ) == FAIL )
                goto diskfull;
           if ( eputc( '\n', ( struct iobuffer * ) fbuf ) == FAIL )
                goto diskfull;
      }
}

if ( exiting && ( ( lastl == UNKNOWN ) || !goteof ) )  {
     copybytestart = SECSIZ-( textbuf->_nleft + SECSIZ )%SECSIZ;
     if ( eputc( '\r', ( struct iobuffer * ) fbuf ) == FAIL )  goto diskfull;
     if ( eputc( '\n', ( struct iobuffer * ) fbuf ) == FAIL )  goto diskfull;
     copying = YES;
     while ( ( c = ( char ) egetc( textbuf ) ) != ENDFILE
               &&    c != ( char ) DFAIL )
     if ( eputc( c, ( struct iobuffer * ) fbuf ) == FAIL )  goto  diskfull;
}

if ( eputc( ENDFILE, ( struct iobuffer * ) fbuf ) == FAIL )  goto  diskfull;
if ( dflush( ( struct iobuffer * ) fbuf ) == FAIL )  goto  diskfull;
if ( ffclose( ( struct iobuffer * ) fbuf ) == FAIL )  {
     error( " Can't close file " );
     goto  reposition;
}

if ( copying )  ffclose( textbuf );
return  YES;

diskfull:
  error(" Disk full ");
reposition:
  if ( copying )  {
       ffclose( ( struct iobuffer * ) fbuf );
       funlink( name );
       textbuf->_nleft = 0;
       while ( copybytestart-- )  egetc( textbuf );
   }
return FAIL;
}


int exists(name)
char *name;
{
char c;

c = 'y';
if ( checkexists( name ) )  {
     putmess(" OK to replace ");
     putstr( name );
     putstr(" ? ");
     scr_co( ( c = getlow() ) );
     putret();
  }
return c == 'y';
}


/* return YES if file 'name' exists, else NO
 */
int checkexists(name)
char *name;
{
int  fd;
    
/* if (dskcheck(setjmp(dskerr)) != 0 || (fd=open(name,0)) == FAIL) return NO; */
/* no setjmp just yet */
if ( ( fd = open( name, 0 ) ) == FAIL )
       return  NO;
close( fd );
return  YES;
}


/* used to input filename, etc in message routine
 */
int scans(answer,maxlen)
char *answer;
int maxlen;
{
unsigned char c;
int  n, i;

maxlen--;
n = 0;
while ( n < maxlen )  {
  switch( ( c = getscankey() ) )  {
    case BACKSP :
    case DELLEFT_P :
      if ( n )  {
        scr_co( BACKSP );
        scr_co( ' ' );
        scr_co( BACKSP );
        n--;
        answer--;
      }
      break;
    case CR_P :
    case ESCKEY_P :
      n = maxlen;
      break;
    case LEFTKEY_P :    /* pass this key code thru */
      c = tran[LEFTKEY];
      goto passthru;
    case RETRIEVE_P :
      if ( *answer )  {
           while ( *answer )  {
             scr_co( *answer );
             answer++;
             n++;
           }
      break;
      }
      else  {
        c = tran[RETRIEVE];       /* if no answer, display the key code */
        goto passthru;
      }
    case GS :                     /* kludge to permit ESC in find/alter */
      c = ESC;
      default :
passthru:
     if ( c > 127 || c < 31 )  break;
     scr_co(c);
     *answer = c;
     answer++;
     n++;
     break;
   }
}
*answer = '\0';
putret();                   /* set PARBIT of c back to 0 to */
return ( c & ~PARBIT );     /* maintain compatibility with other calls */
}


void format(name)
char *name;
{
char tempname[FILELEN], *n, *tn;

if ( !*name )  return;

if ( *(name+1) == ':' )
       strcpy( tempname, name );
  else  {
    tempname[0] = ( char ) curdsk + 'A';
    tempname[1] = ':';
    name[FILELEN] = '\0';
    strcpy( &tempname[2], name );
}

for (  n = name, tn = tempname; ( *n = toupper( *tn ) ); n++, tn++ )
       if ( *n == '.' )  break;

while( *n = tolower( *tn ) )  {
        n++;
        tn++;
/*      ;  */
  }
}


/* puts a new suffix on a file name
 */
void retag(name,tag)
char *name, *tag;
{
for ( ; *name && *name != '.'; name++ );
if ( !*name )  *name = '.';
for ( ++name; ( *name = *tag ); name++, tag++ );
}


/* that's all */

