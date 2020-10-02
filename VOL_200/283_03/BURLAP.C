

/* burlap.c : 11/29/88, 12/11/88, et seq., d.c.oshel */

#include "burlap.h"
#include <fcntl.h>

#define COLORFILE "BURLAP.CFG"

#define ONCE 0  /* does new_record() continue looping? */
#define MANY 1

char far *burlap_screen = (char far *) NULL;
int delimiter = ',';
char *parameter_file_name;
char *screen_file_name;
char *macro_file_name;

/*=============================================*/
/*          Definitions of Fields Used         */
/*=============================================*/

FIELDS LastName, FirstName, Company, Address1, Address2,
       City, State, ZipCode, Salutation, HomePhone, WorkPhone,
       Comment, NameLine, CoLine, A1Line, A2Line, CityLine;



/*=============================================*/
/*               Field Validaters              */
/*=============================================*/


/* concatenate FirstName, LastName and place the result into NameLine
   */
static int concatnames( char *p, int x, int y, int len )
{
    char *q;
    int i;

    memset( p, SPC, len );
    q = strdup( FirstName );
    strip_blanks(q);
    for ( i = 0; *q && i < len; i++, q++, p++ )
        *p = *q;
    free(q);
    if ( i > 0 && i < len )
    {
        p++;
        i++;
        q = strdup( LastName );
        strip_blanks(q);
        for ( ; *q && i < len; i++, q++, p++ )
            *p = *q;
        free(q);
    }
    return (1);
}


/* concatenate City, State, ZipCode and place the result into CityLine
   */
static int concattown( char *p, int x, int y, int len )
{
    char *q, *c, *s,*z;
    int i;

    memset( p, SPC, len );

    c = strdup( City );
    s = strdup( State );
    z = strdup( ZipCode );

    strip_blanks(c);
    strip_blanks(s);
    strip_blanks(z);

    for ( i = 0, q = c; *q && i < len; i++, q++, p++ )
        *p = *q;
    if (strlen(c))
    {
        *p++ = ',';
        i++;
    	i++;
        p++;
    }
    for ( q = s; *q && i < len; i++, q++, p++ )
        *p = *q;

    for ( i++, p++, q = z; *q && i < len; i++, q++, p++ )
        *p = *q;

    free(c);
    free(s);
    free(z);

    return (1);
}

static int moveco( char *p, int x, int y, int len )
{
	memcpy( p, Company, len );
    return (1);
}
static int movea1( char *p, int x, int y, int len )
{
	memcpy( p, Address1, len );
    return (1);
}
static int movea2( char *p, int x, int y, int len )
{
	memcpy( p, Address2, len );
    return (1);
}


/*=============================================*/
/*              Screen Layout                  */
/*=============================================*/


DEFINE_FIELDS( burlap )
#define TOP 0

    TEXT( LastName,   21,  3, SLIDE(38) )
    TEXT( FirstName,  21,  4, SLIDE(38) )
    TEXT( Company,    21,  5, SLIDE(38) )
    TEXT( Address1,   21,  6, SLIDE(38) )
    TEXT( Address2,   21,  7, SLIDE(38) )
    TEXT( City,       21,  8, SLIDE(38) )
    TEXT( State,      21,  9, SLIDE(38) )
    TEXT( ZipCode,    21, 10, SLIDE(38) )
    TEXT( Salutation, 21, 11, SLIDE(38) )
    TEXT( HomePhone,  21, 12, SLIDE(38) )
    TEXT( WorkPhone,  21, 13, SLIDE(38) )
    TEXT( Comment,    21, 14, SLIDE(38) )

#define NUMBER_OF_REAL_FIELDS 12

    VIRTUAL( NameLine,    8, 17, 45, concatnames )
    VIRTUAL( CoLine,      8, 18, 45, moveco )
    VIRTUAL( A1Line,      8, 19, 45, movea1 )
    VIRTUAL( A2Line,      8, 20, 45, movea2 )
    VIRTUAL( CityLine,    8, 21, 45, concattown )

#define NUMFIELDS NUMBER_OF_REAL_FIELDS+5

#define ALLOCATE NUMFIELDS

END_FIELDS

int ISAM_field_range = NUMBER_OF_REAL_FIELDS;  /* important GLOBAL */


static char field [ BUFSIZE ];
static char buffer[ BUFSIZE ];

char vrec[ BUFSIZE ];  /* current ISAM record buffer, all reads */
char vout[ BUFSIZE ];  /* new ISAM record buffer, all writes */
int vrec_len = 0;



/*
=================================================================
   CAUTION:  *** This is a TRICK ***

   the r2 alias defines the order in which the allocated fields
   of the form should be edited; it is only referred to in calls
   to _onepageform()
=================================================================
*/
DEFINE_FIELDS( r2 )
    TEXT( LastName,   21,  3, SLIDE(38) )
    TEXT( FirstName,  21,  4, SLIDE(38) )
    VIRTUAL( NameLine,    8, 17, 45, concatnames )
    TEXT( Company,    21,  5, SLIDE(38) )
    VIRTUAL( CoLine,      8, 18, 45, moveco )
    TEXT( Address1,   21,  6, SLIDE(38) )
    VIRTUAL( A1Line,      8, 19, 45, movea1 )
    TEXT( Address2,   21,  7, SLIDE(38) )
    VIRTUAL( A2Line,      8, 20, 45, movea2 )
    TEXT( City,       21,  8, SLIDE(38) )
    TEXT( State,      21,  9, SLIDE(38) )
    TEXT( ZipCode,    21, 10, SLIDE(38) )
    VIRTUAL( CityLine,    8, 21, 45, concattown )
    TEXT( Salutation, 21, 11, SLIDE(38) )
    TEXT( HomePhone,  21, 12, SLIDE(38) )
    TEXT( WorkPhone,  21, 13, SLIDE(38) )
    TEXT( Comment,    21, 14, SLIDE(38) )
END_FIELDS


/* Control-C mesmerizer */
static jmp_buf isis;
static void osiris( void )
{
	longjmp(isis,-1);
}


void ISAM_crash( char *p )
{
	bomb("c-tree(TM) %s Error %d File %d",p,isam_err,isam_fil);
}


static char *mstop[] = {
    "No!",
    "Yes"
};

int ask( char *msg )
{
    int n;
    fullscreen();
    gotoxy( 62,1+16 );
    wputs( msg );
    n = select( 63, 1+18, &mstop[0], (sizeof(mstop)/sizeof(char *)));
    fullscreen();
    return (n);
}


static void load_default_macro( void )
{
	char *p;
    int i,j,c;
    FILE *mfile;

    if ( macro_file_name == (char *) NULL )
        return;

    strset( macro_buffer, SPC );

    if ( (mfile = fopen( macro_file_name,"rt" )) != NULL && !ferror(mfile) )
    {
        for ( p = &macro_buffer[0], c = fgetc( mfile ), i = j = 0; 
              c != EOF && j < 4 && i < 256;
              c = fgetc( mfile), i++ )
        {
            if ( ferror( mfile ) )
                bomb("can't read macro file: %s", macro_file_name );

            if ( !isprint(c) )
            {
                j++;
                i = j * 64;
            }
            else
            {
                *(p + i) = c;
            }
        }
        fclose(mfile);
    }
}



/* === strip_blanks === 
** removes leading or trailing blanks from the argument, returns nothing
*/
void strip_blanks( char *q )
{
      char *p;

      for ( p = q; *p == SPC; p++ )
      ;

      if (p > q) 
          memcpy( q, p, strlen(p)+1 );

      if ( (p = strchr( q, EOL )) != NULL )
        for ( ; p >= q && !isgraph(*p); p-- )
          *p = EOL;
}



/* === getline ===
** This has the virtue of correctly reading files which
** have characters with their high bits set (Wordstar, etc.);
** getline() will strip high bits from the characters received,
** and does NOT store control characters in the receiving buffer.
**
** Reads until '\n' or end of file.
**
** Will read lines longer than BUFSIZE (excess ignored)
** and correctly initializes buffer for empty input
*/ 

static void getline( FILE *fptr, char buffer[] )
{
     static int c, cnt;

     cnt = 0;
     memset( buffer, 0, BUFSIZE );

     while (1)
     {
         buffer[ cnt ] = EOL;  /* so that if empty line, buffer is right */

         c = ((fgetc( fptr )) & 0x7F);

         if ( ferror( fptr ) )
         {
             bomb("read error: %s", strerror(errno));
         }
         else if ( feof( fptr ) )
         {
            return;
         }
         else if ( c == '\n' )
         {
            return;
         }
         else if (iscntrl(c))  /* any other control char */
         {
            /* do nothing, eat it */
            ;
         }
         else if (cnt < (BUFSIZE - 1)) /* truncate, but continue reading line */
         {
            buffer[ cnt++ ] = c;
         }
     }
}


/* === nextfield ===
** Moves contents of the CURRENT delimited field from bufptr[] to field[];
** returns a pointer to the NEXT field, or to NULL if no fields remain.
**
** Properly reads double- and single-quoted strings with embedded commas,
** embedded complementary single- or double-quotes, and embedded
** double-double- or double-single-quotation marks.  This means, that
** nextfield can tweeze field information out of comma-delimited text
** records created by or for WordStar, MailMerge, PC-Write, dBase II or ///,
** R:Base 5000, or BASIC.  And, no doubt, &c.
*/

static char * nextfield( char * bufptr )
{
      int match, matchtest;
      char *fldptr;

      memset( field, 0, BUFSIZE );
      if ( bufptr == NULL )
          return ( NULL );

      fldptr = field;
      match = EOL;

      while (1)
      {
           *fldptr = EOL;

           if ( *bufptr == EOL )
           {
                break;
           }
           else if ( *bufptr == match )
           {
                bufptr++;
                /* BASIC's double-quote, e.g., "Billy ""Bob"" Smith" */ 
                if ( *bufptr == match )
                {
                    *fldptr = *bufptr;
                    bufptr++;
                    fldptr++;
                }
                else match = EOL;
           }
           else if ( *bufptr == QUOTE || *bufptr == APOSTROPHE )
           {
                matchtest = (*bufptr == QUOTE)? APOSTROPHE : QUOTE;

                if ( match == matchtest )
                {
                     *fldptr = *bufptr;
                     bufptr++;
                     fldptr++;
                }
                else /* match is EOL, QUOTE was in previous test */
                {
                     if ( fldptr == field )
                     {
                          match = *bufptr; 
                          bufptr++;
                     }
                     else
                     {
                          *fldptr = *bufptr;
                          bufptr++;
                          fldptr++;
                     }
                }
           }
           else if ( *bufptr == delimiter && match == EOL )
           {
                bufptr++;
                break;
           }
           else
           {
                *fldptr = *bufptr;
                bufptr++;
                fldptr++;
           }
      }
      *fldptr = EOL;  /* just in case */

      return ( (*bufptr == EOL) ? NULL : bufptr );
}




/* enter with data in the FORM_RULES array
** write variable length ISAM record
*/
static void add_var_record( void )
{
	int i,j;
    char *q;

    memset( vout, 0, BUFSIZE );
    vrec_len = 0;

    for ( i = 0; i < ISAM_field_range; i++ )
    {
        q = strdup( *(burlap[i].fptr) );
        strip_blanks( q );
        for ( j = 0; *(q+j) != EOL; j++ )
        {
            vout[ vrec_len ] = *(q+j);
            if ( vrec_len < BUFSIZE )
                vrec_len++;
        }
        vout[ vrec_len ] = EOL;
        if ( vrec_len < BUFSIZE )
            vrec_len++;
        free( q );
    }
    if ( vrec_len > 0 )
	    if ( ADDVREC( datno, vout, vrec_len ) )
            ISAM_crash("ADDVREC");
}


static void screenmessage( char *p, int x, int y, int len )
{
	MSJ_SetFldAttr( SPC, tm+y, lm+x, vid_attr, len, &video );
    MSJ_DispMsgLen(   p, tm+y, lm+x, len, &video );
}

/* convert record from ascii file to a variable length ISAM record
*/
static void process_record( void )
{
    char **f;
	char *p, *q;
    int i,m,n,x,y,len;
    VALIDATER fi;

    p = buffer;
    i = 0;
    do
    {
        f = burlap[i].fptr;
        x = burlap[i].x;
        y = burlap[i].y;
        len = abs( burlap[i].len );

        /* erase field in form */
        q = *f;
        memset( q, SPC, MAXVFLDLEN );
        *(q+MAXVFLDLEN) = EOL;

        /* current field in the input buffer is moved to the field array
           as a side effect of this call,
           then p advances to the next field in the input buffer;
           if p is NULL, there is no more data (perhaps unexpectedly?)
           */
        if ( p == NULL )
            *field = EOL;
        else
            p = nextfield( p );

        strip_blanks( field );

        /* move field to field in form */
        memcpy( q, field, min( strlen(field), MAXVFLDLEN ) );

        screenmessage( q,x,y,len );

        i++;
    }
    while ( i < ISAM_field_range );

    for ( i = ISAM_field_range; i < NUMFIELDS; i++ )
    {
        f = burlap[i].fptr;
        x = burlap[i].x;
        y = burlap[i].y;
        len = abs( burlap[i].len );
        fi = burlap[i].fi;

        q = *f;

        if ( fi != VNOP )
            (*fi)(q,x,y,len);

        screenmessage( q,x,y,len );
    }

    add_var_record();
}



void put_screen( void )
{
    static char *info[] = {
    	"^1Burlap is free!  Give it away!^0",
        "by",
        "David C. Oshel",
        "1219 Harding Avenue",
        "Ames, Iowa 50010",
        "",
        "^1ISAM file management by FairCom's c-tree(TM)^0",
        "Portions of the compiled version of this",
        "program are the property of FairCom, Inc.,",
        "4006 W. Broadway, Columbia, MO 65203.",
        NULL
    };
    static int flag = 1;
    char **p;
    int y;
    if ( flag )
    {
        fullscreen();
        hidecursor();
        clrscrn();
        windowbox( 5,5,75,18 );
        fullscreen();
    	for ( y = 7, p = info; *p != NULL; p++, y++ )
        {
        	gotoxy( (((80-strlen(*p))/2)+(strchr(*p,'^')==NULL?0:2)),y );
            wputs( *p );
        }
        burlap_screen = PrepScreen( screen_file_name );
        sleep(100);
        defcursor();
        flag = 0;
    }
    PopScreen( burlap_screen );
}


static int ISAM_open = 0;

static void reopen_ISAM_files( void )
{
    int i;
    while ( i = OPNISAM( parameter_file_name ) )
    {
        if ( i == 12 )
        {
    	    if ( CREISAM( parameter_file_name ) )
                bomb("c-tree(TM) CREISAM \"%s\" Error %d File %d", parameter_file_name, isam_err, isam_fil );
            if ( CLISAM() )
                bomb("c-tree(TM) CLISAM Error %d File %d", isam_err, isam_fil );
        }
        else
        {
            bomb("c-tree(TM) OPNISAM \"%s\" Error %d File %d", parameter_file_name, isam_err, isam_fil );
        }
    }
    ISAM_open = 1;
}



static void close_ISAM_files( void )
{
    if ( ISAM_open && CLISAM() )
        bomb("c-tree(TM) CLISAM Error %d File %d", isam_err, isam_fil );
    ISAM_open = 0;
}







/*============= Dump to ASCII File ========================*/

static char *bufptr;

static void verbatimfield( char *p )
{
     for ( ; *p; bufptr++, p++ )
     {
          *bufptr = *p;
     }
}



static void quotefield( char *p )
{
     static int fieldquotemark;

     fieldquotemark = ((strchr(p,QUOTE) == NULL) ? QUOTE : APOSTROPHE);
     *bufptr++ = fieldquotemark;
     for ( ; *p; bufptr++, p++ )
     {
          if (*p == fieldquotemark)
          {
              *bufptr++ = *p; /* double-double quote */
          }
          *bufptr = *p;
     }
     *bufptr++ = fieldquotemark;
}




static int concatenate_fields( void ) 
{
     static int cnt, goodfields;

     bufptr = buffer;
     goodfields = 0; 
     for ( cnt = 0; cnt < ISAM_field_range; cnt++ )
     {
          strcpy( field, *(burlap[ cnt ].fptr) );

          strip_blanks( field );
          goodfields |= strlen( field );
          if ( strchr( field, delimiter ) == NULL )  /* there is NO comma */
          {
              if ( field[0] != QUOTE && field[0] != APOSTROPHE )
              {
                   verbatimfield( field ); /* NO spurious quote */
              }
              else quotefield( field );
          }
          /* case of "has-comma && has-spurious-quote" reduces to the
          ** same case as merely one or the other, the way we do things
          *------------------------------------------
          else if ( *field != QUOTE && *field != APOSTROPHE )  
          {
               quotefield( field );
          }
          -------------------------------------------
          */
          else quotefield( field );  /* there IS a comma in the field! */

          *bufptr++ = delimiter;
     }

     if (bufptr > buffer) --bufptr;  /* point at spurious final delimiter */
     *bufptr = EOL;

     return ( goodfields );  /* were there any fields in the user input? */
}



static void dump_ascii_file( void )
{
    static char *indexes[] = {
        "Name order",
        "Zipcode order",
        "Company order",
        "Return"
    };
	FILE *f;
    int n, index, cnt;
    char numbuf[8];
    union REGS crs;


    nboxmsg( "Export delimited file %s", ASCIIFILE );
    blopbloop();

    if ( !ask( "^1Export file?^0" ) )
        return;

    if ( (index = 1 + select( MLOC, 1, &indexes[0], (sizeof(indexes)/sizeof(char *)) )) > 3 )
        return;

    select_flag = SKIP_FORM;
    if ( ask( "Select records?" ) )
        if ( !choose_records() )
            return;

    fullscreen();
    put_screen();
    hidecursor();
    gotoxy( 62,2 );
    wputs( "^1Esc^0 halts!" );
    windowbox( 0,6,79,24 );
    fullscreen();
    gotoxy( 2,5 );
    wprintf( "µ^2 Exporting in %s -> %s ^0Æ", indexes[index-1], ASCIIFILE );
    setwindow( 1,6,78,23 );
    gotoxy(0,0);


    f = fopen( ASCIIFILE, "wt" );
    if ( f == NULL || ferror(f) )
        bomb("Could not open %s", ASCIIFILE );


    for ( cnt = 0,n = FRSREC(index,vrec); n != 101; n = NXTREC(index,vrec) )
    {
        if ( n )
            ISAM_crash("NXTREC");

        if ( REDVREC( datno,vrec,BUFSIZE ) )
            ISAM_crash("REDVREC");

        sprintf( numbuf, "%d", ++cnt );
        savecursor( &crs );
        fullscreen();
        screenmessage( numbuf,62,5,strlen(numbuf) );
        setwindow( 1,6,78,23 );
        restcursor( &crs );

        load_form();

        if ( kbhit() )  /* stop and go on keypress */
        {
            while ( kbhit() )
                if ( getch() == ESC )
                    goto zoo;
            while ( !kbhit() )
                ;
            while ( kbhit() )
                if ( getch() == ESC )
                    goto zoo;
        }

        if ( selected() && concatenate_fields() )
        {
            wprintf( "%s\n", buffer );
            fprintf( f, "%s\n", buffer );
            if ( ferror(f) )
                bomb( "Could not write %s", ASCIIFILE );
        }
    }
    zoo:
    while ( kbhit() ) getch();

    fullscreen();
    defcursor();

    fclose( f );
    if ( ferror( f ) )
        bomb( "Could not close %s", ASCIIFILE );


    /* make last record the current ISAM record on exit */
    if ( LSTREC(index,vrec) )
        ;
}


/*=================== Load ASCII File ======================*/

static void load_ascii_file( void )
{

	FILE *f;

    nboxmsg( "Import delimited file %s", ASCIIFILE );
    bopbleet();

    if ( !ask( "^1Import file?^0" ) )
        return;

    if ( access( ASCIIFILE, 0 ) != 0 )
    {
    	boxmsg( "%s not found, data not loaded!", ASCIIFILE );
        return;
    }

    /*========== DESTRUCTIVE Method =============
    nboxmsg( "Removing old files..." );

    close_ISAM_files();

    remove( ISAMFILE );
    remove( INDXFILE );

    reopen_ISAM_files();
    ===========================================*/

    if ( (f = fopen( ASCIIFILE, "rt" )) == NULL || ferror(f) )
    {
        boxmsg( "Source file error: %s", strerror(errno) );
        return;
    }

    fullscreen();
    put_screen();
    gotoxy( 2,0 );
    wprintf( "µ^2 Importing <- %s ^0Æ", ASCIIFILE );
    gotoxy( 62,18 );
    wputs( "^1Esc^0 halts!" );
    intense_video();
    while ( kbhit() ) getch();

    do
    {
    	getline( f, buffer );
        if ( *buffer )
        {
            process_record();
        }
        if ( kbhit() && getch() == ESC )
        {
            boxmsg( "** OK, stopping with a partial load! **" );
            break;
        }
    }
    while ( !feof(f) );

    fclose (f);

    close_ISAM_files();
    reopen_ISAM_files();
}


static void add_message( void )
{
	static char *msg[] = {
    	"^2Add record^0",
        "^1F10^0 assists",
        "^1Esc^0 when done",
        NULL
    };
    char **p;
    int n;

    for ( n = 0, p = msg; *p != NULL; n++, p++ )
    {
    	gotoxy( 63,18+n );
        wputs( *p );
    }
}



static void edit_message( void )
{
	static char *msg[] = {
    	"^2Edit record^0",
        "^1F10^0 assists",
        "^1Esc^0 when done",
        NULL
    };
    char **p;
    int n;

    for ( n = 0, p = msg; *p != NULL; n++, p++ )
    {
    	gotoxy( 63,18+n );
        wputs( *p );
    }
}



static void new_record( int many )
{
	int i, len, more;

    do
    {
        more = 0;
        fullscreen();
        put_screen();
        add_message();
        for ( i = 0; i < NUMFIELDS; i++ )
        {
            len = (burlap[i].len < 0) ? MAXVFLDLEN : burlap[i].len;
            memset( *(burlap[i].fptr), SPC, len );
        }
        if ( _onepageform( NULL, r2, NUMFIELDS ) == SAVE_FORM )
        {
            add_var_record();
            more = many;  /* i.e., ONCE or MANY, depending who calls */
        }
    }
    while ( more );
}



void load_form( void )
{
	int i,len;
    char *p;

    for ( p = vrec, i = 0; i < ISAM_field_range; i++ )
    {
        len = (burlap[i].len < 0) ? MAXVFLDLEN : burlap[i].len;

        memset( *(burlap[i].fptr), SPC, len );
        memcpy( *(burlap[i].fptr), p, strlen(p) );
    	p = strchr( p, '\0' );
        p++;
    }
}



void show_form( void )
{
    /*--------- this function is in Fafnir (!) ------------
        DisplayForm( burlap_screen, burlap, NUMFIELDS );

        It is not used, however, to keep PopScreen from 
        erasing current info on the active screen.
    -----------------------------------------------------*/
    char *p;
    int i,x,y,len;
    VALIDATER fi;

    fullscreen();
    intense_video();
    hidecursor();
    for ( i = 0; i < NUMFIELDS; i++ )
    {
        p   = *(burlap[i].fptr);
        x   = burlap[i].x;
        y   = burlap[i].y;
        len = abs( burlap[i].len );
        fi  = burlap[i].fi;
        if ( fi != VNOP )
            (*fi) (p,x,y,len);  /* all with initializers are NOT sliding! */
        screenmessage( p,x,y,len );
    }
    normal_video();
    defcursor();
}



static void delete_record( void )
{
    if ( DELVREC( datno ) )
        ISAM_crash("DELVREC");
}



static int delete_dialogue( void )
{
    if ( ask( "^1Delete Record?^0" ) )
    {
        put_screen();     /* wipe from screen */
        delete_record();
        return (1);
    }
    return (0);
}



/*======== label printing routines ========*/

static void dl0( FILE *label )  /* FILE is open, may be stdprn */
{
	int i, l;
    char *q;

    /* write "smart" label, i.e., all blank lines are at the
       bottom of the label; assumes labels are 6 lines/inch
       */
    for ( l = 0, i = ISAM_field_range; i < NUMFIELDS; i++ )
    {
        q = strdup( *(burlap[i].fptr) );
        strip_blanks(q);
    	if ( strlen(q) )
        {
            /* echo line to reverse-video label area on screen */
            wputs( q );
            if ( ++l < 5 )
                wink('\n');

            /* now print, in case label==printer and it's offline */
            fprintf( label, "%s\r\n", q );
            if ( ferror(label) )
                bomb("Can't write $ file: %s", strerror(errno) );

        }
        free (q);
    }

    /* fill in remainder of 6-line label with blank lines in $ file */
    for ( ; l < 6; l++ )
    {
        fprintf( label, "\r\n" );
        if ( ferror(label) )
            bomb("Can't write $ file: %s", strerror(errno) );
    }
}



static void label_dialogue( void )
{
    static char *style[] = {
        "Write over $",
    	"Append to $",
        "Print label",
        "Return"
    };

	int n;
    FILE *label;

    fullscreen();
    gotoxy( 62,1+16 );
    wputs( "^1Address label^0" );
    if ( (n = select( 63,1+18,&style[0],(sizeof(style)/sizeof(char *)))) > 2 )
        return;

    setwindow( 7,17,53,21 );
    reverse_video();
    clrwindow();
    gotoxy(0,0);

    if ( n < 2 )
    {
        label = fopen( "$", (n == 0 ? "wb" : "ab") );
        if ( label == NULL || ferror(label) )
            bomb("Can't open $ file: %s", strerror(errno) );
        dl0( label );
        fclose( label );
        if ( ferror(label) )
            bomb("Can't close $ file: %s", strerror(errno) );
    }
    else
    {
        nboxmsg( "** Printer **" );
    	dl0( stdprn );
        fflush( stdprn );
    }

    normal_video();
    fullscreen();
}




static int counter = 0;
static int find_string( COUNT index, char *locate, int direction )
{
	int i, n;
    char *p;

    strupr( locate );

    while (1)
    {
        if ( kbhit() )
        {
        	if ( getch() == ESC )
            {
            	return (0);  /* user interrupts search */
            }
        }

        if ( direction == 0 )
        {
            nboxmsg( "%d", counter++ );
            if ( (n = NXTREC( index, vrec )) == 101 )
            {
                /* we're at end of file */
	            if ( LSTREC( index, vrec ) != 0 )
                {
                    ISAM_crash("LSTREC");
                }
                boxmsg( "End of file" );
                break;
            }
            else if ( n != 0 )
                ISAM_crash("NXTREC");
        }
        else
        {
            nboxmsg( "%d", counter-- );
            if ( (n = PRVREC( index, vrec )) == 101 )
            {
                /* we're at beginning of file */
	            if ( FRSREC( index, vrec ) != 0 )
                {
                    ISAM_crash("FRSREC");
                }
                boxmsg( "Beginning of file" );
                break;
            }
            else if ( n != 0 )
                ISAM_crash("PRVREC");
        }

        if ( REDVREC( datno, vrec, BUFSIZE ) )  /* read the record */
        {
            ISAM_crash("REDVREC");
        }

        if ( load_form(), selected() )
            return (1);

        /*----- old way -------
        for ( p = vrec, i = 0; i < ISAM_field_range; p++, i++ )
        {
            if ( strstr( strupr(p), locate ) )
            {
                return (1);
            }
    	    p = strchr(p,'\0');
        }
        -----------------------*/
    }
    return (0); /* no match in any record from point of search */
}



static void locate_record( COUNT index, char *p )
{
    static char *loc[] = {
    	"Locate next",
        "Locate previous",
        "Edit record",
        "Return"
    };
    int n, direction;

    fullscreen();
    put_screen();

    counter = direction = 0;  /* 0=Forward, 1=Backward */

    while (1)
    {
        if ( !find_string(index, p, direction) )
            return;

        kukamonga:  /* jump up here after editing & saving a record */

        /* reread current record because fields were strupr'd above */
        if ( REDVREC( datno, vrec, BUFSIZE ) )  
        {
            ISAM_crash("REDVREC");
        }

        fullscreen();
        put_screen();
        load_form();
        show_form();

        switch ( select( MLOC, 1, &loc[0], (sizeof(loc)/sizeof(char *))))
        {
        case 0:  /* forward */
            direction = 0;
            break;
        case 1:  /* backward */
            direction = 1;
            break;
        case 2:  /* edit */
            fullscreen();
            put_screen();
            edit_message();
            if ( _onepageform( NULL, r2, NUMFIELDS ) == SAVE_FORM )
            {
                nboxmsg("Updating");
                delete_record();  /* old copy is in vrec */
                add_var_record(); /* new copy is in form, goes to vout */
                goto kukamonga;
            }
            break;
        default:
            return;
        }
    }	
}



static char *indices[] = {
    "Select Name",
    "Select Zipcode",
    "Select Company ",
    "Return"
};

static void scan_records( void )
{

    static char *directions[] = {
    /*   ....v....1....v */
    	"Next record",
        "Previous record",
        "Find key",
        "Get first",
        "Get last",
        "Locate string",
        "Edit record",
        "Delete record",
        "Insert record",
        "Use other index",
        "Address label",
        "Return"
    };


    int n, index;
    static char key[] = "....v....1....v....2";  /* 20 chars  */
    static char bln[] = "                    ";  /* 20 spaces */

    restart:
    fullscreen();
    put_screen();
    if ( (index = 1 + select( MLOC, 1, &indices[0], (sizeof(indices)/sizeof(char *)) )) > 3 )
        return;

    top:
	if ( FRSREC( index, vrec ) != 0 )  /* search */
    {
        boxmsg("c-tree(TM) FRSREC Error %d File %d", isam_err,isam_fil);
    	return;
    }

    while (1)
    {
        if ( REDVREC( datno, vrec, BUFSIZE ) )  /* read current record */
        {
            ISAM_crash("REDVREC");
        }

        fullscreen();
        put_screen();
        load_form();
        show_form();

        switch( select( MLOC, 1, &directions[0], (sizeof(directions)/sizeof(char *))))
        {
        case 0:  /* Next */
            if ( (n = NXTREC( index, vrec )) == 101 )
            {
                /* at end of file, wrap to beginning of file */
	            if ( FRSREC( index, vrec ) != 0 )
                {
                    ISAM_crash("FRSREC");
                }
            }
            else if ( n != 0 )
                ISAM_crash("NXTREC");
            break;

        case 1:  /* Previous */
            if ( (n = PRVREC( index, vrec )) == 101 )
            {
                /* at beginning of file, wrap to end of file */
	            if ( LSTREC( index, vrec ) != 0 )
                {
                    ISAM_crash("LSTREC");
                }
            }
            else if ( n != 0 )
                ISAM_crash("PRVREC");
            break;

        case 2:  /* Find */
            strcpy( key, bln ); 
            nboxmsg( "Enter key:                     " );
            fullscreen();
            EditField( key, 33, 10, (index == ixcmp? 16 : 20), 0, c2upr, VNOP );
            put_screen();

            if ( strcmp( key, bln ) != 0 )
            {
                strip_blanks(key);  /* TRICK!!!  Pads key right with nulls! */
                if ( GTEREC( index, key, vrec ) != 0 )
                {
            	    nboxmsg("%s: %s not found", indices[index-1], key);
                    goto restart;
                }
            }
            break;

        case 3:  /* First Record */
            goto top;
            break;

        case 4:  /* Last Record */
	        if ( LSTREC( index, vrec ) != 0 )  /* search */
            {
                boxmsg("c-tree(TM) LSTREC Error %d File %d", isam_err,isam_fil);
    	        return;
            }
            break;

        case 5:  /* Locate String */
            select_flag = SKIP_FORM;
            if ( choose_records() )
            {
                locate_record( index, key ); /* key is a dummy, now */ 
            }
            break;

        case 6:  /* Edit */
            fullscreen();
            put_screen();
            edit_message();
            if ( _onepageform( NULL, r2, NUMFIELDS ) == SAVE_FORM )
            {
                nboxmsg("Updating");
                delete_record();  /* old copy is in vrec */
                add_var_record(); /* new copy is in form, goes to vout */
            }
            break;

        case 7:  /* Delete */
            if ( delete_dialogue() )
            {
                /* record was deleted, so advance to next record */
                if ( (n = NXTREC( index, vrec )) == 101 )
                {
                    /* at end of file, wrap to beginning of file */
	                if ( (n = FRSREC( index, vrec )) != 0 )
                    {
                        if ( n == 101 )
                        {
                            boxmsg("All records deleted!");
                        }
                        else
                        {
                            /* fatal error of some kind */
                            ISAM_crash("FRSREC");
                        }
                        return;
                    }
                }
                else if ( n != 0 )
                    ISAM_crash("NXTREC");
            }
            break;

        case 8:
            new_record( ONCE );  /* add just one record, on the fly */
            break;

        case 9:  /* other index */
            goto restart;
            break;

        case 10:  /* address label */
            label_dialogue();
            break;

        default:
            return;
            break;
        }
    }
}


static void kill_files( void )
{
    /* there must be some way to retrieve the actual pathname from
       the internal data structure which c-tree maintains (?)
       */
    if ( access( ISAMFILE,0 ) != 0 )
    {
    	boxmsg( "Run KILL ISAM only in your actual BURLAP subdirectory!" );
        return;
    }

    nboxmsg( "Danger:  Kill ISAM ** D E S T R O Y S ** existing files!" );

	if ( ask( "Kill ISAM?" ) )
    {
        bopbleet();
        if ( ask( "Are you SURE?" ) )
        {
            put_screen();
            nboxmsg( "Initializing new, empty ISAM files!" );
    	    close_ISAM_files();
            remove( ISAMFILE );
            remove( INDXFILE );
            reopen_ISAM_files();
        }
    }
}

static void utilities( void )
{
	static char *utils[] = {
    /*   ....v....1....v */
        "Print labels",
    	"Import ASCII",
        "Export ASCII",
        "Kill ISAM files",
        "Return"
    };
    int index;

    while (1)
    {
        fullscreen();
        put_screen();
        /* re-read current record, if possible */
        if ( REDVREC( datno, vrec, BUFSIZE ) == 0 ) 
        {
            load_form();
            show_form();
        }

        switch ( select( MLOC, 1, &utils[0], (sizeof(utils)/sizeof(char *))))
        {
        case 0:  /* print */
            fullscreen();
            put_screen();
            gotoxy( 2,0 );
            wputs( "µ^2 Print Labels ^0Æ" );
            load_default_macro();
            print_labels( ixzip );
            break;
        case 1:  /* import ASCII */
            load_ascii_file();
            break;
        case 2:  /* export ASCII */
            dump_ascii_file();
            break;
        case 3:  /* delete ISAM files */
            kill_files();
            break;
        default:
            return;
            break;
        }
    }
}


/* =======  SCREEN COLOR ROUTINES ======= */

static char cmask[] = "%d\n";

static void save_color( void )
{
	FILE *cf;

    if ( (cf = fopen( COLORFILE, "w" )) != NULL )
    {
    	fprintf( cf, cmask, vid[0] );
    	fprintf( cf, cmask, vid[1] );
    	fprintf( cf, cmask, vid[2] );
    	fprintf( cf, cmask, vid[3] );
    	fprintf( cf, cmask, video.SnowFlag );
    }
    fclose (cf);
}

static void get_color( void )
{
	FILE *cf;
    unsigned char a,b,c,d,e;

    a = vid[0];
    b = vid[1];
    c = vid[2];
    d = vid[3];
    e = video.SnowFlag;
    if ( (cf = fopen( COLORFILE, "r" )) != NULL )
    {
    	fscanf( cf, cmask, &a );
    	fscanf( cf, cmask, &b );
    	fscanf( cf, cmask, &c );
    	fscanf( cf, cmask, &d );
    	fscanf( cf, cmask, &e );
        vid[0] = a;
        vid[1] = b;
        vid[2] = c;
        vid[3] = d;
        if ( video.mode != 7 )
            video.SnowFlag = e;
    }
    fclose (cf);
}
               /*   ....v....1....v */
static char pq[] = "Turn snow OFF";
static char qp[] = "Fast video ON";
static char ma[] = "Monochrome, n/a";
static char hue[] = "Video colors";
static char hil[] = "Video hilites";

static char *amenu[] = {
    "View/edit",
    "Add records",
    "Utilities",
    ma,
    hil,
    "Exit to DOS"
};

int menu( void )
{
    union REGS x;

    if ( FRSREC( ixnam, vrec ) == 0 )  /* get first record, if possible */
        ;

    while (1)
    {
        normal_video();
        fullscreen();
        put_screen();

        /* re-read current record, if possible */
        if ( REDVREC( datno, vrec, BUFSIZE ) == 0 ) 
        {
            load_form();
            show_form();
        }

        switch ( select( MLOC, 1, &amenu[0], (sizeof(amenu)/sizeof(char *))))
        {
        case 0:
            scan_records();
            break;

        case 1:
            new_record( MANY );  /* add a batch of records */
            break;

        case 2:
            utilities();
            break;

        case 3:
            if ( video.mode == 3 || video.mode == 2 )
            {
                video.SnowFlag ^= 1;
                amenu[3] = video.SnowFlag ? qp : pq;
                if ( ask( "Save value?" ) )
                    save_color();
            }
            else
                boxmsg( "Monochrome adapters don't have snow!" );
            break;

        case 4:
            PopScreen( burlap_screen ); /* fresh canvas */
            clairol();
            MSJ_MovScrBuf( burlap_screen, 0, 0, 2000, &video );
            if ( ask( "Save values?" ) )
                save_color();
            break;

        default:
            if ( ask( "^1Exit to DOS?^0" ) )
                return;
            break;
        }
    }
}


main( int argc, char *argv[] )
{
    char **p;
    FILE *fp;
    int i;

    static char *pinfo[] = {
    "12 3 4 1",
    "",
    " 0   burlap.dt      0   4096  4  3 lname rem",
    "  1  burlap.ix     24  8 1  2  4096  0 0   0  2 name",
    "     0  15 5",
    "     1   5 5",
    "  2                24  8 1             0   0  3 zip",
    "     7  10 5",
    "     0   5 5",
    "     1   5 5",
    "  3                24  8 1             0   0  3 conam",
    "     2  10 5",
    "     0   5 5",
    "     1   5 5",
    "",
    "",
    "Note:  BURLAP.EXE always starts up using the parameter file BURLAP.P,",
    "       located in the currently active subdirectory.  BURLAP.P directs",
    "       c-tree(TM) ISAM file manager to the appropriate BURLAP files as",
    "       given above.  You may give a complete path to another directory.",
    "",
    "       ********************    C A U T I O N    ***********************",
    "       *  Do Not Change Any Part Of This File Except For Path Names!  *",
    "       ********************    C A U T I O N    ***********************",
    NULL
    };

    vid_init(0);
    video.SnowFlag = 0;  /* assume if mode 3 or 2, video card is NOT a CGA */
    get_color();         /* is there a user configuration file? */
    if (video.mode == 3 || video.mode == 2 )
    {
    	amenu[3] = video.SnowFlag ? qp : pq;
        amenu[4] = hue;
    }
    setsynch(0);

    /* any file names on the command line? */

    parameter_file_name = PARMFILE;
    screen_file_name    = SCRNFILE;
    macro_file_name     = (char *) NULL;

    for ( i = 1; i < argc; i++ )
    {
        strlwr( argv[i] );
    	if ( strstr( argv[i], ".p" ) != NULL )
            parameter_file_name = argv[i];
    	if ( strstr( argv[i], ".s" ) != NULL )
            screen_file_name = argv[i];
    	if ( strstr( argv[i], ".m" ) != NULL )
        {
            macro_file_name = argv[i];
            load_default_macro();
        }
    }

    /* set printer to binary mode, i.e., use explicit "\r\n" newlines */

    setmode( fileno(stdprn),O_BINARY );

    /* blank startup?  create the parameter file, if needed */

    if ( access( parameter_file_name, 0 ) == 0 )  
        ; /* do nothing, parameter file exists */
    else
    {
        if ( access( ISAMFILE, 0 ) == 0 || 
             access( INDXFILE, 0 ) == 0  
           )
        {
        	bomb("Data files exist, so must not create parameter file!");
        }

        fp = fopen( parameter_file_name, "wt" );
        if ( fp == NULL || ferror(fp) )
            bomb("can't create parameter file: %s", strerror(errno));
        for ( p = pinfo; *p != NULL; p++ )
        {
            fprintf( fp, "%s\n", *p );
        }
        fclose (fp);
    }

    set_crash_function( CLISAM );  /* tell bomb0 to close ISAM files */

    AllocateFields( burlap, ALLOCATE );

    reopen_ISAM_files();  /* creates empty ISAM files, if needed */

    signal(SIGINT,osiris);
    if (setjmp(isis))  /* if Ctrl-C, ISAM files must be closed on exit */
        ;
    else
        menu();

    nboxmsg("Closing ISAM files");
    close_ISAM_files();

    ReleaseFields( burlap, ALLOCATE );
    exit_fafnir();

    vid_exit();
    exit (0);
}

