/* demo.c -- demonstrate some of the Fafnir library's forms editing routines 
   September 10, 1988
   David C. Oshel
   */


#include "ciao.h"        /* screen & keyboard i/o, supporting fns & macros */
#include "fafnir.h"      /* magic forms macros, etc. */
#include <stdarg.h>


#define PRETEND UPDATE   /* pretend we're going to update records */

                   /*  ....v....1....v  */
static char title[] = "Fafnir ver. 2.0 - elysian fields and forms nirvana by d.c.oshel";

/* forward references to functions named in DEFINE_FIELDS()
   */
static int difference( char *p, int x, int y, int len );
static int concatname( char *p, int x, int y, int len );



/* Field Type macros (ain't C sweet?)
   */

FIELDS last, first, conam, addr1, addr2, city, state, zip, homef, workf,
       today, name, dues, paid, blnc, saltn, rem;
    
DEFINE_FIELDS( fafnir )

    /* Note: do not put trailing commas after closing parenthesis! */

	/* Page 1, starts at offset 0, has 11 fields */

#define PAGE1 TOP_OF_FORM

    TODAY(      today,                              17, 15     )   /* 0 */
    TEXT_SET(   last,   "Oshel",                    17,  3, 30 )   /* 1 */
    TEXT_SET(   first,  "David C.",                 17,  4, 30 )   /* 2 */
    TEXT_SET(   conam,  "MicroConsulting Services", 17,  5, 30 )   /* 3 */
    TEXT_SET(   addr1,  "1219 Harding Avenue",      17,  6, 30 )   /* 4 */
    TEXT(       addr2,                              17,  7, 30 )   /* 5 */
    TEXT_SET(   city,   "Ames",                     17,  8, 30 )   /* 6 */ 
    STATE_SET(  state,  "IA",                       17,  9     )   /* 7 */
    TENZIP_SET( zip,    "50010",                    17, 10     )   /* 8 */

    /* note that cursor follows field element order, here! */

    PHONE_SET( workf, "(515) 292-    ", 17, 12 )       /* first  9 */
    PHONE_SET( homef, "(515) 233-    ", 17, 11 )       /* then  10 */

#define RANGE1 11  /* be very, very careful about this number! */

	/* Page 2, starts at offset 11, has 6 fields */

#define PAGE2 RANGE1

    VIRTUAL(      name,           17,  4, 30, concatname      ) /* 11, 0 */
    DOLLAR_SET(   dues,  "10.00", 17,  6                      ) /* 12, 1 */
    DOLLAR(       paid,           17,  7                      ) /* 13, 2 */
    COMPUTE_SET(  blnc,  "10.00", 17,  8, DL_SIZE, difference ) /* 14, 3 */
    TEXT_SET(     saltn, "Dave",  17, 15, 20                  ) /* 15, 4 */
    TEXT_SET(     rem,   title,   17, 17, SLIDE(15)           ) /* 16, 5 */

#define RANGE2 6  /* don't guess, count, count, count, and then RECOUNT! */

END_FIELDS;


/*-------------------------------------------------------------------
  Number of fields: this argument will be passed to AllocateFields()

                          *** DANGER *** 
                          CRITICAL VALUE!

   Alternate method for computing number of fields to allocate: 

                   #define ALL FormSize(fafnir)

   This works, so long as you're not doing anything fancy with 
   duplicate field names.  Be advised that allocating too many
   or too few fields is guaranteed to crash the system!
---------------------------------------------------------------------*/

#define ALL RANGE1+RANGE2   /* <-- don't add this in your head, stupid! */




/*=================== Utilities ==================*/

/* global system error handler */

void bomb( char *msg, ... )
{
	va_list arg_ptr;

	printf("\a\n*** Error: ");
	va_start( arg_ptr, msg );
	vprintf( msg, arg_ptr );
	va_end( arg_ptr );
	exit (1);
}



/* true computed field, difference of two other fields */
static int difference( char *p, int x, int y, int len )
{
	long n1, n2, bal;
    char *q, *r, temp[48];

    n1 = atoln( dues, len );
    n2 = -1L * atoln( paid, len );

    bal = n1 + n2;

    q = dlrcvt( bal );        /* make it canonical 0.00 format */ 
    r = strchr( q, '\0' );    /* remove trailing minus or blank */
    r--;
    *r = '\0';

    if ( bal < 0L )
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
        strset( p, '$' );       /* field overflow */
    else
        strcpy( p,temp );

    return (1);
}


/* "validate" virtual field by concatenating two other fields */

static int concatname( char *p, int x, int y, int len )
{
    register int i,j;

    memcpy( p, first, len );

    for ( i = len - 1; i > 0; i-- )
    {
    	if ( *(p + i) != ' ' )
           break;
    }

    if ( *p != ' ' ) 
    {
    	i++;
        i++;
    }

    for ( j = 0; j < 30 && i < 30; i++, j++ )
    {
    	*( p + i ) = *( last + j );
    }

    return (1);
}



main()
{
    extern char DamageBuffer[];
    extern char DamageControlMsg[];

    static char *dialogue[] = {
        "Continue demonstration",
        "Exit program"
    };

    vid_init(0);  /* *** MUST *** */

    AllocateFields( fafnir, ALL );  /* *** MUST *** */

    /* would normally move the data record into form fields at this
       point, but we're faking the UPDATE with an initialized form
       */

    do
    {
        zoo:
        switch ( TwoPageForm( GetPage( "FAF1.S", fafnir, PAGE1, RANGE1 ), 
                              GetPage( "FAF2.S", fafnir, PAGE2, RANGE2 ), 
                              PRETEND ) )
        {
        case SAVE_FORM:
            /* would normally move the form fields back into the data
               record at this point, but we're just pretending
               */
            boxmsg( "Pretending to save this form!" );
            break;
        case DELETE_FORM:  /* only if OLDUP */
            boxmsg( "You can't delete a demonstration form!");
        case SKIP_FORM:
            nboxmsg( "Searching..." );  /* fake search for OLDUP */
            goto zoo;
            break;
        case STOP_SEARCH:  /* only if OLDUP */
            boxmsg( "What, just like that?!" );
            break;
        }
    }
    while ( (blopbloop(),select( 25,1,dialogue,2 )) == 0 );

    vid_exit();  /* restore sign-on screen */

#if defined (TEST)

    /* check to make sure the formedit module has not fouled up somehow
       */
    printf( "If the program screwed up and got outside a field dimension,\n");
    printf( "there will be an F beside a wrong number beside the field name.\n");

    printf( "last   %2d  %c\n", strlen(last ), ((strlen(last ) == 30)? 'T':'F'));
    printf( "first  %2d  %c\n", strlen(first), ((strlen(first) == 30)? 'T':'F'));
    printf( "conam  %2d  %c\n", strlen(conam), ((strlen(conam) == 30)? 'T':'F'));
    printf( "addr1  %2d  %c\n", strlen(addr1), ((strlen(addr1) == 30)? 'T':'F'));
    printf( "addr2  %2d  %c\n", strlen(addr2), ((strlen(addr2) == 30)? 'T':'F'));
    printf( "city   %2d  %c\n", strlen(city ), ((strlen(city ) == 30)? 'T':'F'));
    printf( "state  %2d  %c\n", strlen(state), ((strlen(state) ==  2)? 'T':'F'));
    printf( "zip    %2d  %c\n", strlen(zip  ), ((strlen(zip  ) == 10)? 'T':'F'));
    printf( "homef  %2d  %c\n", strlen(homef), ((strlen(homef) == 14)? 'T':'F'));
    printf( "workf  %2d  %c\n", strlen(workf), ((strlen(workf) == 14)? 'T':'F'));
    printf( "today  %2d  %c\n", strlen(today), ((strlen(today) ==  8)? 'T':'F'));
    printf( "name   %2d  %c\n", strlen(name ), ((strlen(name ) == 30)? 'T':'F'));
    printf( "dues   %2d  %c\n", strlen(dues ), ((strlen(dues ) == 15)? 'T':'F'));
    printf( "paid   %2d  %c\n", strlen(paid ), ((strlen(paid ) == 15)? 'T':'F'));
    printf( "blnc   %2d  %c\n", strlen(blnc ), ((strlen(blnc ) == 15)? 'T':'F'));
    printf( "saltn  %2d  %c\n", strlen(saltn), ((strlen(saltn) == 20)? 'T':'F'));
    printf( "rem    %2d  %c\n", strlen(rem  ), ((strlen(rem  ) == 60)? 'T':'F'));

    printf("Total of %u fields on the form!\n\n",
            (sizeof( fafnir )/sizeof(FORM_RULES)));
#endif

    /* prove that a FORM_RULES alias works as advertised! 
       */
    printf( "*** Balance: $%s\n\n", crdrform( atoln( blnc, 16 ) ) );

    ReleaseFields( fafnir, ALL );

#if defined (TEST)
    printf( "Damage Control Test: %s\n", 
            ((strcmp(DamageBuffer,DamageControlMsg)==0)?"Passed":"Failed") 
          );
#endif

    exit (0);
}

