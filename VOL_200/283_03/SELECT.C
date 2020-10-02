
/* select.c : 11/27/88, d.c.oshel */

#include "burlap.h"

#define XCRD 6
#define YCRD 18

int select_flag = SKIP_FORM;

/* this exhibits the underlying relationships between a form and the
   actual storage location(s) of the data collected into the form; 
   
   NOTE, in this case we DO NOT use Allocate_Fields, because the storage
   locations are NOT dynamic(!); also, while fields are NOT null-terminated
   ASCIZ strings, the macro_buffer as a whole IS.
   */

char macro_buffer[] =  /* four 64-char lines will be edited as one buffer */
  "                                                                "
  "                                                                "
  "                                                                "
  "                                                                \0";

char * l1 = &macro_buffer[   0 ];
char * l2 = &macro_buffer[  64 ];
char * l3 = &macro_buffer[ 128 ];
char * l4 = &macro_buffer[ 192 ];

int macro_help( char *p, int x, int y, int len );  /* forward */

DEFINE_FIELDS( mstring )
    FIELD( l1, TX_MASK, XCRD, YCRD+0, 64, VNOP, cgood, macro_help )
    FIELD( l2, TX_MASK, XCRD, YCRD+1, 64, VNOP, cgood, macro_help )
    FIELD( l3, TX_MASK, XCRD, YCRD+2, 64, VNOP, cgood, macro_help )
    FIELD( l4, TX_MASK, XCRD, YCRD+3, 64, VNOP, cgood, macro_help )
END_FIELDS


int macro_help( char *p, int x, int y, int len )
{
    char far *fp;
    union REGS xx;
    char **q;
    static char *help[] = {
"^1Record Selection Macros^0 evaluate to TRUE or FALSE and may be nested.",
"",
"^1(OP N S)^0  where  ^1OP^0 is ^1=^0, ^1>^0, ^1>=^0, ^1<^0, ^1<=^0, or ^1?^0, where ? means CONTAINS",
"                  ^1N^0 is the contents of field N, where N = 1, 2, 3, ...",
"                  ^1S^0 is a string (must not contain parentheses)",
"",
"^1(^^ x)^0      means  ^1NOT x^0,     where x is TRUE or FALSE or a macro",
"^1(& x1 x2)^0  means  ^1x1 AND x2^0, where x1, x2 are like x",
"^1(| x1 x2)^0  means  ^1x1 OR x2^0",
"",
"E.g.,  ^1(& (? 1 Smith) (^^ (= 8 50010) ) )^0",
"       selects Last Names containing \"Smith\" where Zipcode is not 50010",
"",
"^2Press spacebar to resume editing macro^0",
NULL
    };

    if ( *p == '' )  /* help request signal */
    {
        fp = savescreen(&xx);
        normal_video();
        hidecursor();
        windowbox( 0, 0, 79, YCRD-3 );
 
        for ( q = help; *q != NULL; q++ )
        {
            wputs( *q );
            wink('\n');
        }
        defcursor();
        setsynch(1);
        while ( keyin(screenwait) != SPC )
            ;
        restorescreen(fp,&xx);
        return (0);
    }
    return (1);  /* same as TEXT, otherwise; i.e., anything is ok */
}



int record_criterion_met( void )
{
    char *p, *q;
    int n;

    p = malloc( MACROSIZE );
    if ( p == (char *) NULL )
        bomb("out of memory");

    memset( p, 0, MACROSIZE );
    q = strdup( macro_buffer );
    strip_blanks( q );
    strcpy( p, q );
    free( q );

    n = evaluate_macro( p );
    free( p );

    return (n);
}


int selected( void )
{
    if ( select_flag == SAVE_FORM )  /* there is a valid select macro */
    {
        return ( record_criterion_met() );
    }
    /* else no select macro, so the record should always be selected */
	return (1);
}


static int valid_macro( void )
{
    char *p;
    for ( p = macro_buffer; *p == SPC; p++ )
    ;
    if ( *p != '(' )
    {
        macro_help( "",0,0,0 );
        return (0);
    }
    /* else macro correctly leads off with opening parenthesis;
       all other syntax errors are trapped by the criterion test
       */
    return ( (record_criterion_met() == -1) ? 0 : 1 );
}



static void swap_menus( void )
{
	static char *macro_actions[] = {
    	"Continue editing macro",
        "Accept macro and proceed",
        "Discard macro and return"
    };
    int i;
    char *p;

    for ( i = 0; i < 3; i++ )
    {
    	p = Fafnir_form_exit_actions[i];
        Fafnir_form_exit_actions[i] = macro_actions[i];
        macro_actions[i] = p;
    }
}



int choose_records( void )
{
    char far *p;
    union REGS x;
    char *temp;

    temp = strdup( macro_buffer );  /* save previous macro */

    p = savescreen(&x);
    put_screen();
    normal_video();
    windowbox( XCRD, YCRD, XCRD-1+64, YCRD-1+4 );
    fullscreen();
    gotoxy( XCRD+1, YCRD-1 );
    wputs( "µ^2 Enter Record Selection Macro ^0 - ^1F1^0 help, ^1Esc^0 done Æ" );

    select_flag = SKIP_FORM;

    swap_menus();
    do
    {
        select_flag = _onepageform( NULL, mstring, 4 );
    }
    while ( select_flag == SAVE_FORM && !valid_macro() );
    swap_menus();

    restorescreen(p,&x);

    if ( select_flag == SAVE_FORM )
    {
        free( temp );
        return (1);
    }
    else /* discard edited macro and quit */
    {
        memcpy( macro_buffer, temp, 256 ); /* restore previous macro */
        free( temp );
        return (0);
    }
}

