/* fafnir.c -- general purpose forms dragon, with Elysian Fields
               5/10/88, 8/20/88, 9/10/88, 9/24/88, by d.c.oshel
               */


#include "ciao.h"
#include "keys.h"
#include "fafnir.h"
#include <stdarg.h>

#define FVERSION "Fafnir v. 3.0"

#define VERSION "µ^2 Elysian Fields v. 3.0 ^0ÆÍÍÍÍÍÍÍÍÍÍ" 
/*              "±±±±±±±±°°°°°±±±±°°°±±±±±±±±±±±±Ý",*/
 

/* value returned from user dialogue boxes only */
#define EDIT_FORM 0


/* =====  Common Default Field Masks  ===== */

char zssn[]   = "000-00-0000";
char zblank[] = "";
char zdate[]  = "00/00/00";
char zfone[]  = "(   )    -    ";  /* 14 chars */
char zeroes[] = "0000000000";  /* ten zeroes */
char zdol[]   = "0.00";




/*================  Global Character Validaters  ==================*/


/* allow any printable char */
unsigned cgood( unsigned c )
{
	return ( (isprint(c))? c: 0 );
}


/* allow only uppercase alpha, else any printable */
unsigned c2upr( unsigned c )
{
	return ( (isprint(c))? toupper(c): 0 );
}


/* allow only digits 0..9 */
unsigned cnmrc( unsigned c )
{
	return ( (isdigit(c))? c: 0 );
}


/* allow only 0..9, blanks and hyphen */
unsigned cnzip( unsigned c )
{
	return ( (isdigit(c) || c == ' ' || c == '-') ? c : 0 );
}


/* allow only chars appropriate in dollar amounts */
unsigned cdoll( unsigned c )
{
	if ( isdigit(c) || c == '.' || c == ',' || 
           c == '$' || c == ' ' || c == '-' )
		return (c);
	else
		return (0);
}


/* return Y on T,Y,1, return N on F,N,0 */
unsigned ctfyn( unsigned c )
{
    c = toupper(c);
    if ( c == 'T' || c == 'Y' || c == '1' )
        return ('Y');
    else if ( c == 'F' || c == 'N' || c == '0' )
        return ('N');
    else
        return (0);
}


/* fills "(   )    -    " zfone mask with "(123) 678-ABCD" */
unsigned cfone( unsigned c, int pos )  
{
    if ( pos == 0 || pos == 4 || pos == 5 || pos == 9 )
        return (0);
    else if (pos < 10)
        return ( cnzip( c ) );
    else if ( isalnum( c ) || c == ' ' )
        return ( toupper( c ) );
    else
        return (0);  /* does not allow SPC in some positions */
}

/*======================  Global Field Validaters  ==================*/


/* === vdate(): Date Validater === */

static int goodday( int m, int d, int y )
{
	static int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	static char *mo_names[] = {
		NULL, 
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	};

	int leapyear;

	if ( m < 1 || m > 12 )
	{
		boxmsg("MM/dd/yy range error: MM = %d?", m );
		return (0);
	}

	leapyear = y % 4;  /* fails in 2000 A.D. */
	days[2] = leapyear? 28 : 29;

	if ( d < 1 || d > days[ m ] )
	{
		boxmsg("%s %d?  %d days hath %s...", mo_names[m], d, days[m], mo_names[m] );
		return (0);
	}
	return (1);
}

/* is the date valid? 
   */
int vdate( char *p, int x, int y, int len )
{
	int n,m,d,yr;
    char date[9];

    if (*p == '')
    {
        cf_date( date, 2 );
        boxmsg( "MM/DD/YY  E.g., today is %s", date );
        return (0);
    }

	m  = atoin( p + 0, 2 );			/* "00/00/00" */
	d  = atoin( p + 3, 2 );
	yr = atoin( p + 6, 2 );

	n = goodday(m,d,yr);			/* prints range error messages */
	return (n);
}


/* "validate" date field by setting it to today's date
*/
int vtoday( char *p, int x, int y, int len )
{
	char d[9];
    cf_date( d, 2 );
    memcpy( p,d,8 );
    return (1);
}


static char doll[] = "Enter $ amount";

/* field init, left justify $0.00 in field */
int lj_dol ( char *p, int x, int y, int len )
{
	long n;
    char *q,*r;
    char temp[48];

    if (*p == '')
    {
    	boxmsg( doll );
        return (0);
    }

    n = atodoln( p, len );  /* interpret field contents */
    q = dlrcvt( n );        /* make it canonical 0.00 format */ 
    r = strchr( q, '\0' );  /* remove trailing minus or blank */
    r--;
    *r = '\0';

    if ( n < 0L )
    {
        r = malloc(len);
        strcpy( r, "-" );
        strcat( r,q );
        sprintf( temp, "%-*s", len, r );  /* left justify, <--- */
        free(r);
    }
    else
        sprintf( temp, "%-*s", len, q );  /* left justify, <--- */

    if ( strlen(temp) != len )  /* do not alter field */
        return (0);
    else
        strcpy( p,temp );

    return (1);
}



/* field validate, right justify user input as a canonical $0.00 dollar 
   amount (adds missing cents digits!) 
   */

int rj_dol ( char *p, int x, int y, int len )
{
	long n;
    char *q,*r;
    char temp[48];

    if (*p == '')
    {
    	boxmsg( doll );
        return (0);
    }

    n = atodoln( p, len );  /* interpret field contents */
    q = dlrcvt( n );        /* make it canonical 0.00 format */ 
    r = strchr( q, '\0' );  /* remove trailing minus or blank */
    r--;
    *r = '\0';

    if ( n < 0L )
    {
        r = malloc(len);
        strcpy( r, "-" );
        strcat( r,q );
        sprintf( temp, "%*s", len, r );  /* right justify, ---> */
        free(r);
    }
    else
        sprintf( temp, "%*s", len, q );   /* right justify, ---> */

    if ( strlen(temp) != len )  /* do not alter field */
        return (0);
    else
        strcpy( p,temp );

    return (1);
}


static char *tf[] = {
    "Yes",
    "No"
};
int vtrufal( char *p, int x, int y, int len )
{
    if (*p == '')
    {
        memcpy( p, tf[ select(x,y,tf,2) ], 1 );
    }
    return (1);
}

/* ================  Other Global Utilities ================= */

static PTR_TO_CRASH_FN cleanup = NO_CRASH_FN;

                                   /*....v....1....v....2....v....3.*/
static char DamageControlBuffer[] = "<<<< Fafnir Damage Control >>>>";
static char DamageControlMsg[]    = "<<<< Fafnir Damage Control >>>>";
static char Damage80Slop[]        = "Fafnir by Oshel"; /* 32+32+16 makes 80 for slop */

/* a recommended bomb function, user may #define bomb(X) bomb0(X)
   */


void bomb0( char *msg, ... )
{
    va_list arg_ptr;

    /* call user's fatal error shutdown routine, if set */

    if ( cleanup != NO_CRASH_FN )
        (*cleanup)();

    /* shut ourselves down */

    setsynch(1);
    gotoxy(0,24);
    vid_exit();

    /* print the terminating diagnostic message */

    printf("\a\n*** error: ");
    va_start( arg_ptr, msg );
    vprintf( msg, arg_ptr );
    va_end( arg_ptr );
    printf("\n");

    if (strcmp(DamageControlBuffer,DamageControlMsg) != 0)
        printf( "\n*** assignment to unallocated field\n");

    /* don't allow batch to take over before user has had a chance to
       see and record the message
       */
    while ( kbhit() )
        getch();
    printf( "\n*** press any key " );
    getch();

    /* and die */

    exit (255);
}


void set_crash_function( PTR_TO_CRASH_FN x )
{
    *cleanup = x;
}


void exit_fafnir( void )
{
    if ( strcmp(DamageControlBuffer,DamageControlMsg) != 0 )
        bomb0( FVERSION );
}

/* end of error handlers */




/* returns the value of len (or fewer) digits in p as a signed int 
   */
int atoin( char *p, int len )
{
	int i;
    int n, sign;

	n = 0;
    sign = 1;
	for ( i = 0; i < len && *p; i++, p++ )
	{
		if ( *p == '-' )
            sign = -1;
        else if ( isdigit( *p ) )
		{
			n *= 10;
			n += (*p - '0');
		}
	}
	return ( n * sign );
}



/* returns the value of len (or fewer) digits in p as a signed long int 
   */
long atoln( char *p, int len )
{
	int i;
    long n, sign;

	n = 0L;
    sign = 1L;
	for ( i = 0; i < len && *p; i++, p++ )
	{
		if ( *p == '-' )
            sign = -1L;
        else if ( isdigit( *p ) )
		{
			n *= 10L;
			n += (long)(*p - '0');
		}
	}
	return ( n * sign );
}


/* atodoln() returns the value of len (or fewer) digits in p as a signed 
   "dollar long" int, i.e., the string "$1.00" returns 100L, normalized
   to n * 100 cents

   allows free-form user cents fields, i.e., 
      $100 is $100. is $100.0 is $100.00 is $100.000 (mills digit ignored!)
      all these cases return 10000L, equivalent to $100.00
   */
long atodoln( char *p, int len )
{
	int i, dotflag, count;
	long n, sign;

	for ( count = dotflag = i = 0, n = 0L, sign = 1L; 
	      *p && (i < len) && (count < 2); 
		  i++, p++ )
	{
		if ( isdigit(*p) )
		{
			n *= 10L;
			n += (long) (*p - '0');
			if ( dotflag )
			{
				++count;
			}
		}
		else if ( *p == '-' )
		{
			sign = -1L;
		}
		else if ( *p == '.' )
		{
			dotflag = 1;
		}
	}

	/* adjust 1 dollar to 100 cents precision */

	for ( ; count < 2; count++ )
	{
		n *= 10L;
	}
	return ( n * sign );
}

/* put a transient error message in the middle of the screen; vanishes in
   about 8 seconds, or if user presses a key
   */
void boxmsg( char *msg, ... )
{
	char far *p;
    union REGS x;
	int n;
	va_list arg_ptr;
	char buf[128];

	va_start( arg_ptr, msg );
	vsprintf( buf, msg, arg_ptr );
	va_end(arg_ptr);

	p = savescreen( &x );

    hidecursor();
	reverse_video();
	n = ((80 - strlen(buf)) / 2) - 4;
	windowbox( n, 9, n + strlen(buf) + 3, 11 );
	gotoxy( 2,1 );
	wputs( buf );
	sleep( 40 );

    restorescreen( p,&x );

}

/* same, but overwrites current screen, not timed */
void nboxmsg( char *msg, ... )
{
    union REGS x;
	int n,a,b,c,d;
	va_list arg_ptr;
	char buf[128];

	va_start( arg_ptr, msg );
	vsprintf( buf, msg, arg_ptr );
	va_end(arg_ptr);

    savecursor( &x );

    hidecursor();
	reverse_video();
	n = ((80 - strlen(buf)) / 2) - 4;
	windowbox( n, 9, n + strlen(buf) + 3, 11 );
	gotoxy( 2,1 );
	wputs( buf );

    restcursor( &x );
}



static void ScrnMsg( char *q, int x, int y, int len )
{
    /* this is a local function because the next assumes both len's 
       equal and non-printing portions of the string blank-filled,
       true for sure only in Fafnir */

	MSJ_SetFldAttr( SPC, tm+y, lm+x, vid_attr, len, &video );
	MSJ_DispMsgLen( q, tm+y, lm+x, len, &video );
}


static void DispIntense( char *p, int x, int y, int len )
{
    /* this is a local function because the next assumes both len's 
       equal and non-printing portions of the string blank-filled,
       true for sure only in Fafnir */

    /* set intense video attr */
	MSJ_SetFldAttr( SPC, tm+y, lm+x, vid[1], len, &video );
	MSJ_DispMsgLen( p, tm+y, lm+x, len, &video );
}



/* select0() does NOT adjust coordinates, or save the screen 
   */

int select0( int topx, int topy, char *menu[], int menu_size )
{
	int line, width, limit, frame;
	char *q;
	unsigned c;

    setsynch(0);
	hidecursor();
	intense_video();

	limit = min( 16, menu_size );

	for ( width = line = 0; line < menu_size; line++ )
	{
		q = menu[ line ];
		width = max( width, strlen(q) );
	}

    windowbox( topx, topy, (topx + width + 1), (topy + limit - 1) );


zoo:
	for ( line = 0; line < limit; line++ )
	{
        q = menu[ line ];
        MSJ_SetFldAttr( SPC, tm+line, lm+0, vid[1],    width + 2, &video );
        MSJ_DispMsgLen(   q, tm+line, lm+1, strlen(q), &video );
	}

	frame = line = 0;
	while (1)
	{
		q = menu[ (frame + line) % menu_size ];

        MSJ_SetFldAttr( SPC, tm+line, lm+0, vid[2],    width + 2, &video );
        MSJ_DispMsgLen(   q, tm+line, lm+1, strlen(q), &video );

		c = keyin( screenwait );

        MSJ_SetFldAttr( SPC, tm+line, lm+0, vid[1],    width + 2, &video );
        MSJ_DispMsgLen(   q, tm+line, lm+1, strlen(q), &video );

		switch ( c )
		{
		default:
                boxmsg( "Arrow keys move bar to your choice, Enter selects" );
                break;

		case CR:
				goto exit;
				break;

		case UP:
		case LF:
		case PGUP:
        case '4':
        case '8':
				--line;
				if ( line < 0 )
				{
					scrolldn();
					line = 0;
					--frame;
					if ( frame < 0 )
					{
						frame = menu_size - 1;
					}
				}
				break;

		case HOME:
        case '7':
				goto zoo;
				break;

		case DN:
		case RT:
		case PGDN:
        case '2':
        case '6':
				if ( line == limit - 1)
				{
					scrollup();
					++frame;
					if ( frame > menu_size - 1 )
					{
						frame = 0;
					}
				}
				else 
					++line;
				break;
		}
	}

    exit:
	return ( ((frame + line) % menu_size) );
}


/* select() saves the screen, restores it on exit, adjusts coordinates
   if necessary to fit on screen
   */

int select( int topx, int topy, char *menu[], int menu_size )
{
    char far *p;
    union REGS x;
    char *q;
    int result, limit, width, line;

    p = savescreen(&x);

	limit = min( 16, menu_size );

	for ( width = line = 0; line < menu_size; line++ )
	{
		q = menu[ line ];
		width = max( width, strlen(q) );
	}

	while (topy && (( topy + limit + 2 ) >= 23))
		topy -= 1;

	if ( topx > 39 )
		topx = 39;
	while (topx && (( topx + width + 4 ) >= 79))
		topx -= 1;

    result = select0( topx, topy, menu, menu_size );
    restorescreen(p,&x);
    return (result);
}



unsigned screenroutine( unsigned (*fn)(char *), char *msg, ... )
{
	char far *p;
    union REGS x;
    unsigned result;
	va_list arg_ptr;
	char buf[128];

	va_start( arg_ptr, msg );
	vsprintf( buf, msg, arg_ptr );
	va_end(arg_ptr);

    p = savescreen(&x);
    result = (*fn)( buf );
    restorescreen( p,&x );
    return (result);
}



/* ================  Local Utilities ================== */

static char *startup[] = {
    "Edit this record",
    "Skip this record",
    "Delete this record",
    "Stop searching"
};

static int opening_dialogue( void )
{
    bopbleet();
    switch ( select( (80-strlen(startup[2]))/2, 1, startup, 4 ) )
    {
    default:
    case 0:  return ( EDIT_FORM );
    case 1:  return ( SKIP_FORM );
    case 2:  return ( DELETE_FORM );  /* will ask about this! */
    case 3:  return ( STOP_SEARCH );
    }
}


char *Fafnir_form_exit_actions[] = {  /* GLOBAL, 12/4/88, dco */
	"Continue editing",
    "Save this record",
    "Quit"
};

static int decide_form_exit( void )
{
    bopbleet();
    switch ( select( (80-strlen(Fafnir_form_exit_actions[0]))/2, 1, Fafnir_form_exit_actions, 3 ) )
    {
    default:
    case 0:  return ( EDIT_FORM );
    case 1:  return ( SAVE_FORM );
    case 2:  return ( SKIP_FORM );
    }
}

static char *help[] = {             
/*   ....v....1....v....2....v....3... */  /* 177, 176, 182, 198, 205, 221 */
    VERSION,
    "±±±±±±±±°°°°°±±±±°°°±±±±±±±±±±±±Ý",
    "°ÝPress ^1Enter^0 or ^1Esc^0 when done °Ý",    
    "±ÝúúúúúúúúúOther Keysúúúúúúúúúú±Ý",
	"°Ý    ^1F1^0 input assistance      °Ý",
    "°Ý    ^1F2^0 reset field & exit    °Ý",
    "°Ý    ^1F3^0 reset current field   °Ý",
    "°Ý    ^1F4^0 musical gadgets +/-   °Ý",
    "°Ý    ^1F10^0 displays this box    °Ý",
    "±Ý        Home ^1^0 PgUp          ±Ý",
    "±Ý           ^1^0 ^1ú^0 ^1^0             ±Ý",      
    "±Ý        End  ^1^0 PgDn          ±Ý",
    "±Ý  In text fields: ^1Ins^0, ^1Del^0,  °Ý",
    "°Ý  ^1F9^0 erases to end of field  ±Ý",
    "±±±±°°±±±±±±±±±±±±±±°°°±±°°°±±±±Ý",
    NULL
};

static void version_info( void )
{
	char **p;
    int y,t;
    char far *fp;
    union REGS x;

    fp = savescreen(&x);
    hidecursor();
    normal_video();
    t = getsynch();
    setsynch(0);
    for ( y = 0, p = help; *p != NULL; p++,y++ )
    {
        gotoxy(1,y);
    	wputs(*p);
    }
    setsynch(t);
    sleep( 200 );
    restorescreen(fp,&x);
}

static char dynmemerr[] = "out of memory";

/* PrepScreen() reads a 4000-byte screen file from disk to a buffer, and 
   returns a char far * to the buffer which contains the screen.

   Screen files must be created using ES.EXE, or any similar program which
   writes video text page 0 to a disk file.
   */

char far * PrepScreen( char *screen )
{
	 char far *p;
	 FILE *fptr;
	 int i, a;

     if ( screen == (char *) NULL )
        return ( (char far *) NULL );

	 p = _fmalloc( 4000 );
     if ( p == (char far *) NULL )
     {
        bomb0("prep screen: %s", dynmemerr);
     }

	 if ((fptr = fopen(screen,"rb")) == NULL)
	 {
		_ffree(p);
	 	bomb0("can't find screen file: %s\n",screen);
	 }
	 else
	 {
		for (i = 0; i < 4000; i++)
		{
			p[i] = fgetc( fptr );	/* character */
			a    = fgetc( fptr );	/* attribute */

			if (ferror(fptr))
			{
				_ffree(p);
				bomb0("can't read screen file: %s\n",screen);
			}

			/* translate monochrome screen attributes to color */

			if ( video.mode == 3 )  /* running color on CGA? */
			{
				switch ( a )
				{
				default:
				case '\007':			/* NRM */
						a = vid[0];
						break;
				case '\017':			/* BRN */
						a = vid[1];
						break;
				case '\160':			/* RVR */
						a = vid[2];
						break;
				case '\360':			/* BLR */
						a = vid[3];
						break;
				case '\001':			/* UNL */
						a = vid[4];
						break;
				case '\011':			/* BRU */
						a = vid[5];
						break;
				case '\207':			/* BLN */
						a = vid[6];
						break;
				case '\201':			/* BLU */
						a = vid[7];
						break;
				case '\217':			/* BBN */
						a = vid[8];
						break;
				case '\211':			/* BBU */
						a = vid[9];
						break;
				}
			}
			++i;
			p[i] = a;
		}
	 }
	 fclose(fptr);
	 return (p);
}



void PopScreen( char far *p )
{
    MSJ_MovBufScr( p, 0, 0, 2000, &video );
}

/* ========================= Display Form Fields ======================= */


/* DisplayForm returns nothing, assumes PrepScreen has been called to
** load a screen file, and that screen points to a buffer with screen image
*/

void DisplayForm( char far *screen, FORM_RULES form[], int NumFields )
{
    char **pp, *p;
	int i, x, y, len, winlen, temp;
	VALIDATER fi;

    if ( NumFields < 1 )
        return; /* bad domain, assume caller will abort */
    
    if ( screen != (char far *) NULL )
    {
        PopScreen( screen );
    }

    /* show all fields */

    temp = getsynch();
    setsynch(0);

    for ( i = 0; i < NumFields; i++ )
    {
		pp  = form[i].fptr;
        p   = *pp;
		x   = form[i].x;
		y   = form[i].y;
		len = form[i].len;

        winlen = abs(len);
        if ( len < 0 )
            len = MAXVFLDLEN;

        if ( (fi = form[i].fi) != VNOP )
            (*fi)( p,x,y,len );  /* set up virtual fields, etc. */

        DispIntense( p,x,y,winlen );
    }

    setsynch(temp);
}


/* ========================= Edit Field and Form ======================= */

/* EditField can return ESC or CR, or F2, END, PGDN, PGUP, UP, DN
** exits via F2..DN do not validate the field, and do not change its contents
** exits via Esc or Enter, however, DO validate, and update if valid!
** Note: if ((*cvalid)(SPC,i)) in all positions, INS and DEL keys are enabled!
** F1 now requests assistance from (*fvalid)(), but does NOT exit EditField!
** F9 zaps to end-of-field, if INS and DEL keys are enabled.
** F3 is "undo", restores default field contents, does NOT exit.
** see also version_info(), above
**
** New, 9/24/88, d.c.oshel -- if (len < 0), the field is treated like a
**               variable length field padded right with blanks in a
**               buffer MAXVFLDLEN chars long.  The edit field becomes a
**               window len wide which "slides" over the longer edit buffer.
**
**               CAUTION:  fldptr  * M U S T *  point to a string which
**               contains MAXVFLDLEN characters or blanks!  If INS and DEL
**               keys can't be enabled (space not allowed in all positions),
**               the field is treated like an ordinary field len chars wide.
**
**               AllocateFields() manages this detail, for forms only.
*/

unsigned EditField (char *fldptr,		/* data address */
					int x, int y, 		/* screen coordinates, relative 0,0 */
					int len,			/* field length */ 
					int skip_to_next, 	/* end-of-field handling flag */
					VALIDATER cvalid,	/* individual char validation */
					VALIDATER fvalid)	/* field validation */
{
     char *p, *q;
     int i, field_cursor, temp;
     int j, k, insdel, winlen, maxlen, echoline;
     unsigned ch;

     /* bottom line save buffer if variable len edit */
     char far *sylvia;


     /* programmer stupidity check #1; is the field allocated? */

     if ( (fldptr == DamageControlBuffer) || (fldptr == (char *) NULL) )
        bomb0("field not allocated" );

     temp = getsynch();
     setsynch(0);

     echoline = 0;

     window_error:

     if ( len == 0 )
     {
        return ( END );  /* invalid length, same as slipped through field */
     }
     else if ( len < 0 ) /* request for sliding field input */
     {
        maxlen = MAXVFLDLEN;
        winlen = abs(len);
     }
     else
     {
     	winlen = maxlen = len;
     }

	 /* allocate space for copy of field string */


	 p = malloc( maxlen + 4 );		/* extra bytes are for luck */
	 if ( p == (char *) NULL )
	 {
        /* unconditional program exit */
        grunge:
		bomb0("edit field: %s", dynmemerr);
	 }

     /*------------------------------------------------------------------*/
     /*            DANGER!  Must null-terminate this buffer!             */
     /*                                                                  */
     /* Even though the actual field is not necessarily null-terminated, */
     /* the working edit buffer * MUST * be a null-terminated string, to */
     /* prevent accidental errors by sloppily-written field validaters!  */
     /*------------------------------------------------------------------*/

     memset( p, 0, maxlen+4 ); 

     /* align window with edit buffer */

     q = p;



	 do {  /* F1 assistance loop restarts here, if necessary */

	 /* get copy of field string into work buffer */

     for ( insdel = 1, i = 0; i < maxlen; i++ )
     {
            /* do any edit character translations in mask up front;
               for example, if user's keystrokes are translated to uppercase, 
               the default field contents are presented in uppercase!
               */
            if ( cvalid == VNOP )
            {
                ch = fldptr[i];
            }
            else
            {
                ch = (*cvalid)( fldptr[i], i );  /* ch might be set to 0 */

                /* i.f.f. SPC is ok for all i, enable INS and DEL keys
                   */
                insdel &= ( (*cvalid)( SPC, i ) == SPC );
            }
	 		p[i] = ch? ch: fldptr[i];
     }

     if ( (len < 0) && !insdel )
     {
     	    /* request for sliding field, but INS and DEL are not valid,
               so treat this as an ordinary text field
               */
            len = abs(len);
            free (p);
            goto window_error;
     }

     if ( len < 0 )  /* save line 25 in case used by echoline */
     {
        if ( (sylvia = _fmalloc( 80 * sizeof(int) )) == (char far *) NULL )
            bomb0("_fmalloc error");
        MSJ_MovScrBuf( sylvia, 24, 0, 80, &video );
     }


     /* highlight and display string to be edited */

     reverse_video();
     ScrnMsg( q,x,y,winlen );


	 /* go to beginning of edit field */

     setsynch(1);
     defcursor();
	 gotoxy( x,y );

     /* set flag to see if the cursor ever stops in this field */
    
     field_cursor = 0;  /* FLAG, does cursor stop?  assume it never does */



     /* edit the copy of field in p, staying within window limits,
        that is, i is relative to q
        */

	 i = 0;
     while (1)
     {
         /* test mask char, can it be edited? */
         if ( (cvalid == VNOP) || (*cvalid)( q[i], i ) )
         {
              if ( echoline && len < 0 )
              {
                    j = (q-p)+i;
                    ScrnMsg( p,0,24,maxlen ); /* echo entire line */
                    intense_video();          /* cursor attribute */
                    ScrnMsg( (p+j),j,24,1 );  /* echo cursor */
                    reverse_video();          /* back to edit attribute */
              }

              ch = getkey();        /* yes, get user's replacement */
              field_cursor = 1;     /* cursor finally landed! */

           top:
              switch ( ch )
              {
              case F1:
                    if ( len < 0 )
                    {
                        /* reasonable help is to turn the echoline
                           on or off
                           */
                    	echoline ^= 1;
                        MSJ_MovBufScr( sylvia, 24, 0, 80, &video );
                    }
                    else
                    {
			  		    p[0] = '';		/* request assistance */
					    ch = CR;        /* must exit with field validation! */
                        goto top;
                    }
                    break;

              case Shift_F1:
              case Alt_F1:
              case Ctrl_F1:
                    if ( len < 0 )
                    {
                        MSJ_MovBufScr( sylvia, 24, 0, 80, &video );
			  		    p[0] = '';		/* request assistance */
					    ch = CR;        /* must exit with field validation! */
                        goto top;
                    }
                    /* all others fall into... */

              case F10:
                    version_info();
                    break;
              
              case F4:
                    tone ^= 1;      /* toggle sound effects */
                    break;

              case F3:              /* reset default field contents */
                    for ( i = 0; i < maxlen; i++ )
                    {
                        if ( cvalid == VNOP )
                            ch = fldptr[i];
                        else
                        {
                            ch = (*cvalid)( fldptr[i], i );
                        }
	 		            p[i] = ch ? ch : fldptr[i];
                    }
                    q = p;
                    i = 0;
                    ScrnMsg( q,x,y,winlen );
                    gotoxy( x+i,y );
                    break;

              case F9:              /* zap to end of field */
                    if ( insdel )
                    {
                        for ( j = (q-p) + i; j < maxlen; j++ )
                            *(p+j) = SPC;
                        ScrnMsg( q,x,y,winlen );
                        gotoxy( x+i,y );
                    }
                    else thurb();
                    break;
              
              case INS:             /* insert space at cursor */
                    if ( insdel )
                    {
                        for ( j = maxlen - 1, k = j - 1;
                              k >= (q-p)+i;
                              k--, j-- 
                            )
                            *(p + j) = *(p + k);
                        *(p + ((q-p)+i)) = SPC;
                        ScrnMsg( q,x,y,winlen );
                        gotoxy( x+i,y );
                    }
                    else thurb();
                    break;

              case DEL:             /* delete character at cursor */
                    if ( insdel )
                    {
                        for ( k = (q-p)+i, j = k + 1;
                              j < maxlen;
                              k++, j++
                            )
                            *(p + k) = *(p + j);
                        *(p + k) = SPC;
                        ScrnMsg( q,x,y,winlen );
                        gotoxy( x+i,y );
                    }
                    else thurb();
                    break;

              case '\x18':  /* Ctrl-X */
              case TAB:
                    ch = DN;
                    goto top;
                    break;

              case '\x5':  /* Ctrl-E */
              case Shift_TAB:
                    ch = UP;
                    goto top;
                    break;

			  case UP:  /* exit WITHOUT field validation */
			  case DN:
			  case PGUP:
			  case PGDN:
              case F2:
					goto no_update;
					break;

              case ESC:  /* exit WITH field validation */
              case CR:
			  		goto update_edits;
					break;

              case HOME:    /* go to start of field */
                    q = p;
                    ScrnMsg( q,x,y,winlen );
                    i = 0;
                    gotoxy( x+i,y );
                    break;

              case END:    /* go to end of text, or to end of field */
                    for ( j = maxlen-1; j > 0 && *(p+j) == SPC; j-- )
                    ;
                    if ( j == 0 && *p == SPC )
                        j = maxlen-1;
                    else if ( j == maxlen-1 )
                        ;
                    else
                        j++;

                    q = p;
                    i = winlen-1;
                    if ( (p+j) > (q+i) )
                    {
                        q = p+j-i;
                    }
                    else
                    {
                    	i = j;
                    }
                    ScrnMsg( q,x,y,winlen );
                    gotoxy( x+i, y );

                    break;
              
			  /* case '\x13': */ /* DOS interferes with Ctrl-S */
              case BS:		/* backspace or left arrow */
              case LF:
                    /* user's keypress behaves like UP if in position 0 */

                    if ( len < 0 )
                    {
                    	if ( i == 0 )
                        {
                        	--q;
                            if ( q < p )
                            {
                                ch = UP;
                                goto top;
                            }
                            ScrnMsg( q,x,y,winlen );
                        }
                        else
                        {
                            --i;
                        }
                        gotoxy( x+i,y );
                    }
                    else
                    {
                        if ( i == 0 )
                        {
                    	    ch = UP;
                            goto top;
                        }

					    /* otherwise, skip backwards... */
			  		    do
					    {
                            --i;
	              		    if ( i < 0 ) 
							    i = 0;
	                        gotoxy( x + i, y );
                            /* ...over non-edit chars in mask */

                            if ( cvalid == VNOP )
                                break;
					    }
					    while ( i > 0 && (!(cvalid( p[i], i ))));

                        /* did mask bounce left put us in position 0? */

                        if ( cvalid == VNOP )
                            ;
                        else if ( i == 0 && (!(cvalid( p[i], i ))) )
                        {
                    	    ch = UP;
                            goto top;
                        }
                    }
                    break;
			  
              case '\x4':   /* Ctrl-D */
              case RT:	 	/* right arrow */
                    ++i;
                    if ( len < 0 )
                    {
                    	if ( i == winlen )
                        {
                        	--i;
                            ++q;
                            if ( (q+winlen-1) > (p+maxlen-1) )
                            {
                            	if ( skip_to_next )
                                {
                                	ch = CR;
                                    goto top;
                                }
                                --q;
                            }
                            ScrnMsg( q,x,y,winlen );
                        }
                    }
                    else
                    {
                        if ( i == maxlen )
                        {
						    if ( skip_to_next )
						    {
							    ch = CR;
							    goto top;
						    }
						    --i;
                        }
                    }
                    gotoxy ( x+i,y );
                    break;

              
			  default:
                    if ( ch < ' ' || ch > '~' )
                    {
                        thurb();  /* non-ASCII key not defined above */
                    }
			  		else if ((cvalid == VNOP) || (ch = (*cvalid)( ch, i )))
					{
			            q[i] = ch;          /* put valid char in buffer */
						rptchar(ch,1);      /* echo */
                        ++i;                /* advance index */
                        if ( len < 0 )
                        {
                    	    if ( i == winlen )
                            {
                        	    --i;
                                ++q;
                                if ( (q+winlen-1) > (p+maxlen-1) )
                                {
                            	    if ( skip_to_next )
                                    {
                                	    ch = CR;
                                        goto top;
                                    }
                                    --q;
                                }
                                ScrnMsg( q,x,y,winlen );
                            }
                        }
                        else
                        {
					        if ( i == maxlen )
						    {
							    if ( skip_to_next )
							    {
								    ch = CR;
								    goto top;
							    }
							    else  /* end of field, but need explicit exit */
							    {
								    --i;
							    }
						    }
                        }
					    gotoxy( x+i,y );    /* advance cursor */
					}
                    else  /* invalid key */
					{
						thurb();
					}
					break;
              }
         }
         else  /* not a "valid" char in mask, so accept "input" from buffer */ 
		 {
	         rptchar( q[i],1 );         /* echo mask character */
             ++i;                       /* advance index */
             gotoxy( x+i,y );           /* advance cursor */
			 if ( (q-p)+i == maxlen )	/* last pos in field cannot be edited (?!) */
			 {
                if ( !field_cursor )
                {
                	ch = END;    /* slipped through mask, no edits! */
                    goto no_update;
                }
                else if ( skip_to_next )
                {
			 	    ch = DN;     /* cursor will SLIP out of field! */
			        goto top;
                }
                else
                {
			 	    ch = LF;     /* cursor will BOUNCE to previous pos! */
			        goto top;
                }
			 }
		 }
     }

     /* validate the entire field, when done... */

update_edits:  /* Esc, Enter */

     /* programmer stupidity check #2; did the field validater write garbage
        over the receiving field?  note: p is known to be a string, but 
        fldptr is only known to be an array of char maxlen in size (even
        though fldptr probably is a string, we can't assume that's true!)
        */
     if ( strlen( p ) != maxlen )  /* too long or too short? */
        bomb0("field size violation: %d [%s]", maxlen, p );

     /* good string in field buffer, continue with normal exit */

	 if (fvalid == VNOP) 
     {
        if (*p == '') /* user requested assistance on no-validate field */
        {
            boxmsg( "Could be anything!" ); /* give permission */
            ch = 0;
        }
	    else /* good field by definition */
        {
            memcpy( fldptr, p, maxlen );
        }
	 }
     else if ( (*fvalid)( p,x,y,maxlen ) )
     {
        memcpy( fldptr, p, maxlen );
     }
     else
     {
	 	ch = 0;
     }

     } while ( ch == 0 ); /* F1 assistance loop ends here, 9/3/88, dco */

no_update: /* End, PgUp, PgDn, Up, Down */

     DispIntense( fldptr,x,y,winlen ); /* show true field contents on exit */
     normal_video();
     setsynch(temp);

     if ( len < 0 )  /* restore line 25 used by echoline */
     {
        MSJ_MovBufScr( sylvia, 24, 0, 80, &video );
        /* Good night, Mrs. Calabash, wherever you are */
        _ffree(sylvia);
     }

	 free( p );      /* end of field copy requirement */
     return ( ch );	 /* inform caller how edit was terminated */
}




/*  EditForm returns these exit codes:       SAVE_FORM, SKIP_FORM,
**  plus, IFF MultiPage:                     PGDN, PGUP,
**  plus, IFF cursor did not stop anywhere:  END
*/
unsigned EditForm( FORM_RULES form[], int NumFields, int MultiPage )
{
    char **pp, *p;
	int i, go, x, y, len, winlen, maxlen, cursor_landed, temp, action;
    unsigned ExitKey;
	VALIDATER fi;
	VALIDATER cv;
	VALIDATER fv;

    if ( NumFields < 1 )
        return ( END );  /* bad domain, assume caller will abort */
    
    /* show all fields */

    temp = getsynch();
    setsynch(0);

    for ( i = 0; i < NumFields; i++ )
    {
		pp  = form[i].fptr;
        p   = *pp;
		x   = form[i].x;
		y   = form[i].y;
		len = form[i].len;

        if ( len < 0 )
        {
            maxlen = MAXVFLDLEN;
            winlen = abs(len);
        }
        else
        {
        	maxlen = winlen = len;
        }

        if ( (fi = form[i].fi) != VNOP )
            (*fi)( p,x,y,maxlen );  /* set up virtual fields, etc. */

        DispIntense( p,x,y,winlen );
    }

    /* edit fields in FORM_RULES order */

    cursor_landed = 0;  /* assume it never does */
    do
    {
        i = 0;
        go = 1;
        while ( go )
        {
		    pp  = form[i].fptr;
            p   = *pp;
		    x   = form[i].x;
		    y   = form[i].y;
		    len = form[i].len;
		    cv  = form[i].cv;
		    fv  = form[i].fv;

            if ( len < 0 )
            {
                maxlen = MAXVFLDLEN;
                winlen = abs(len);
            }
            else
            {
        	    maxlen = winlen = len;
            }

            if ( (fi = form[i].fi) != VNOP )
            {
                (*fi)( p,x,y,maxlen );     /* initialize the field */
                DispIntense( p,x,y,winlen ); /* show changes, if any */
            }

            if ( cv == VNOP )     /* display-only or computed field? */
            {
                if ( fv != VNOP )
                {
                    (*fv)( p,x,y,maxlen );     /* validate or compute the field */
                    DispIntense( p,x,y,winlen ); /* show any changes, if computed */
                }
                ExitKey = END;
            }
            else
            {
                ExitKey = EditField( p, x, y, len, SKIP_OK, cv, fv );

                if ( ExitKey != END )
                    cursor_landed = 1;  /* cursor stopped SOMEWHERE! */
            }

            /* is user's field exit key a form exit key? */

            switch ( ExitKey )
            {
            default:  /* case 0 is obsolete */
                        break;

            case F2:
                        ExitKey = ESC;  /* fall into */
            case ESC:
                        /* decide_form_exit() will determine exit code */
                        go = 0;
                        break;

            case PGDN:
            case PGUP:
                        if ( MultiPage )
                        {
                        	action = ExitKey;  /* PgUp or PgDn, slide off */
                            goto zoo;
                        }
                        else if ( ExitKey == PGUP )  /* top of form */
                            i = 0;
                        else                         /* bottom of form */
                            i = NumFields - 1;
                        break;

		    case UP:
				    do
				    {
                        --i;
					    if ( i < 0 )
						    i = NumFields - 1;
				    }
				    while ( form[i].cv == VNOP );
				    break;

		    case DN:
		    case CR:
                    ++i;
                    if ( i == NumFields )
                    {
                	    i = 0;

                        if ( MultiPage )
                        {
                    	    action = PGDN; /* slide off form */
                            goto zoo;
                        }
                    }
				    break;
        
            case END:  /* user cannot present this value to switch */
                    ++i;
                    if ( i == NumFields )
                    {
                        i = 0;
                        if ( cursor_landed == 0 ) /* no edits by end of form? */
                        {
                            action = END;
               	            goto zoo;
                        }
                        else if ( MultiPage )
                        {
                    	    action = PGDN;  /* slide off form */
                            goto zoo;
                        }
                    }
                    break;
            }
        }
    }
    while ( (action = decide_form_exit()) == EDIT_FORM );

    zoo:
    setsynch(temp);
    return (action);
}



/* =========================  Screen Managers  ========================== */


#define ptrseg(P) ((int)(((long)P)>>16))
#define ptrofs(P) ((int)((long)P))

/*  _onepageform returns:  SAVE_FORM, SKIP_FORM
*/

int _onepageform( char far *screen, FORM_RULES Page[], int NumFields )
{
	unsigned result;
    int i,j,action;

    if ( screen != (char far *) NULL )
    {
        PopScreen( screen );
    }

    do
    {
    	result = EditForm( Page, NumFields, SINGLEPAGE );
        if ( result == END )
        {
        	action = SKIP_FORM; /* null form, cursor never stopped */
        }
        else
        {
            action = result;
        }
    }
    while ( !(action == SAVE_FORM || action == SKIP_FORM) );

    return ( action );
}



/*  _twopageform returns:  SAVE_FORM, SKIP_FORM
*/

int _twopageform( char far *screen1, FORM_RULES Page1[], int NumFields1,
                  char far *screen2, FORM_RULES Page2[], int NumFields2 )
{
    char far *scrfptr;
	unsigned result;
    int i, j, action, flag, cursor_landed;

    cursor_landed = 0;  /* assume cursor never lands on either page */

    do
    {
        if ( screen1 != (char far *) NULL )
        {
            PopScreen( screen1 );
        }

    	result = EditForm( Page1, NumFields1, MULTIPAGE );
        if ( result == PGUP || result == PGDN || result == END )
        {
            action = EDIT_FORM;
            if ( result != END )
                cursor_landed = 1;
        }
        else  /* anything else exits from entire form */
        {
            action = result;
            continue;
        }

        if ( screen2 != (char far *) NULL )
        {
            PopScreen( screen2 );
        }

    	result = EditForm( Page2, NumFields2, MULTIPAGE );
        if ( result == PGUP || result == PGDN || result == END )
        {
            action = EDIT_FORM;
            if ( result != END )
                cursor_landed = 1;
        }
        else  /* anything else exits from entire form */
        {
            action = result;
        }

        if ( cursor_landed == 0 )  /* cursor never landed on either page */
            action = SKIP_FORM;
    }
    while ( !(action == SAVE_FORM || action == SKIP_FORM ) );

    return ( action );
}



/*  OnePageForm returns:  SAVE_FORM, SKIP_FORM,
**  plus, IFF OldRecord:  DELETE_FORM, STOP_SEARCH
*/

int OnePageForm( char *ScreenName, FORM_RULES Page[], int NumFields, 
                 int OldRecord )
{
    char far *screen;
    int action;

    if ( ScreenName != (char *)NULL )
        screen = PrepScreen( ScreenName );

    if ( OldRecord )
    {
        DisplayForm( screen, Page, NumFields );
        if ( (action = opening_dialogue()) != EDIT_FORM )
            goto zoo;
    }
        
    action = _onepageform( screen, Page, NumFields );

    zoo:
    if ( ScreenName != (char *)NULL )
        _ffree(screen);

    return (action);
}


/*  TwoPageForm returns:  SAVE_FORM, STOP_SEARCH,
**  plus, IFF OldRecord:  SKIP_FORM, DELETE_FORM
*/

int TwoPageForm( char *ScreenName1, FORM_RULES Page1[], int NumFields1, 
                 char *ScreenName2, FORM_RULES Page2[], int NumFields2, 
                 int OldRecord )
{
    char far *screen1;
    char far *screen2;
    int action;

    if ( ScreenName1 != (char *)NULL )
        screen1 = PrepScreen( ScreenName1 );
    if ( ScreenName2 != (char *)NULL )
        screen2 = PrepScreen( ScreenName2 );

    if ( OldRecord )
    {
        DisplayForm( screen1, Page1, NumFields1 );
        if ( (action = opening_dialogue()) != EDIT_FORM )
            goto zoo;
    }
        
    action = _twopageform( screen1, Page1, NumFields1, 
                           screen2, Page2, NumFields2 );

    zoo:
    if ( ScreenName2 != (char *) NULL )
        _ffree(screen2);
    if ( ScreenName1 != (char *) NULL )
        _ffree(screen1);

    return (action);
}



/* ======================  Initialize a Form ==================== */

/* copies up to len bytes of each field's default string into its 
   actual field buffer, in the range of fields indicated; if the
   default (source) is shorter than the field itself (destination),
   the remainder of the field is padded to len bytes with blanks
   */
void InitForm( FORM_RULES Form[], int NumFields )
{
    int i, j, len;
    char **p, *src, *dst;

    for ( i = 0; i < NumFields; i++ )
    {
        p   = Form[i].fptr;
        dst = *p;
        src = Form[i].dflt;
        len = Form[i].len;

        memset( dst, SPC, len );     /* clear the field to blanks */

        if ( src != (char *) NULL )  /* set the field to its default */
        {
            for ( j = 0; j < len && *( src + j ); j++ )
            {
                *( dst + j ) = *( src + j );
            }
        }
    }
}



/* dynamically allocates the indicated range of fields in a form,
   and initializes each to a null-terminated string of len blanks;
   if the dflt field is not NULL, initializes the field's mask 
   */

static int AllocatedFlag = 0;

/* On exit, a program may test to see whether DamageControlBuffer is still
   DamageControlMsg.  If it isn't, there was a write to a field which has
   been Released somewhere in the program.  However, these are not declared
   in "fafnir.h"!  (This is handled by bomb0().  9/30/88, d.c.oshel )
   */

void AllocateFields( FORM_RULES form[], int NumFields )
{
	int i, j, len;
    char **p, *src, *dst;

    for ( i = 0; i < NumFields; i++ )
    {
        len = form[i].len;

        if ( len < 0 )
            len = MAXVFLDLEN;

        p   = form[i].fptr;
    	*p = (char *) malloc( (len+1) +4 ); /* 4 luck! I don't trust Microsoft's malloc() */
        if ( *p == (char *) NULL )
        {
        	bomb0("alloc field: %s", dynmemerr);
        }
        dst = *p;

        memset( dst, '\0', len+1 );  /* put '\0's to end of string */
        memset( dst, SPC, len );     /* clear the field to blanks */

        src = form[i].dflt;

        if ( src != (char *) NULL )  /* set the field to its default */
        {
            for ( j = 0; j < len && *( src + j ); j++ )
            {
                *( dst + j ) = *( src + j );
            }
        }
    }
    AllocatedFlag = 1;
}




/* releases a form's dynamic memory allocation as set by AllocateFields()
   */
void ReleaseFields( FORM_RULES form[], int NumFields )
{
	int i;
    char **p;

    if ( AllocatedFlag )
    {
        for ( i = 0; i < NumFields; i++ )
        {
    	    p = form[i].fptr;
            free (*p);
            *p = &DamageControlBuffer[0];
        }
        AllocatedFlag = 0;
    }
}


