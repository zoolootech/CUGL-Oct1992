/*                                  CFLOW.C
    This program identifies all function calls within a function
*/

#define FALSE   0
#define TRUE    !FALSE
#define ERROR   -1

#include        "stdio.h"
/*
#include        "process.h"
#include        "string.h"
#include        "ctype.h"
*/

/*
void    modules( void );
void    comment( void );
void    lookbak( int );
void    comout( char * );
int     modname( void );
int     unreserved( void );
int     flowchar( char );
int     ischar( char );
int     fgets_sp( void );
*/
void    modules(  );
void    comment(  );
void    lookbak(  );
void    comout(  );
int     modname(  );
int     unreserved(  );
int     flowchar(  );
int     ischar(  );
int     fgets_sp(  );


FILE    *file_pointer;       /* input file pointer */

int     open_braces;      /* keep track of open_braces of open "{"s */
char    module_name[100]; /* module name buffer */

char    input_buffer[256];  /* source input line buffer */
int     curchar; /* current character in input line buffer array subscript */


void main( argc, argv )
int     argc;
char    *argv[];
{
    printf("\nCFLOW --> function declarations and calls in C source");
    printf("\n Modified by Jon R. Herbold"); 

    if (argc != 2)
        {
        printf( "\nusage: cflow [infilename.ext] " ); 
        exit( 1 );
        }

    if( ( file_pointer = fopen( argv[1], "r" ) ) == FALSE )
        {
        printf( "\nCan't open %s\n", argv[1] );
        exit( 1 );
        }

    printf( "\nSource file: %s", argv[1] );

    modules( ); 
    fclose( file_pointer );
}





void modules( )    /* find function declarations and calls */
{
    int     j;
    char    c;
    int     decl;   /* module declaration line flag */
    int     lastlin = FALSE; /* last line of file flag */
    int     quoted = FALSE;  /* within " quotes */
    int     header = FALSE;  /* within function header (before 1st '{') */

    open_braces = 0;

    do 
        {
        lastlin = fgets_sp( );    /* read a line of source */
        decl = FALSE;  /* assume nothing */
        while( input_buffer[curchar] ) /* read for significant characters */
            {
            comment( );  /* strip any comments */

            c = input_buffer[curchar];

            /* read for significant characters */
            /* skip double quoted strings */
            if( c == '\042' )
                quoted = !quoted;

            if( !quoted )
                {
                switch( c )
                    { 

                case '{' : 
                    open_braces++;
                    header = FALSE;
                    break;
                case '}' : 
                    open_braces--;
                    break; 
                case '(' : /* "(" always follows function call or declaration*/
                    if( !ischar( input_buffer[curchar-1] ) ) 
                        break;
                    lookbak( curchar );
                    j = modname( );
                    if( !j )
                        break;
                    else
                        decl = TRUE;
                    if( j == 2 ) 
                        header = TRUE; 
                    break;
                default : 
                    break;
                    }
                }

            curchar++;     /* next character */
            }

        comout( input_buffer );    /* display argument declarations */
        } 
    while( lastlin );    /*  = 0 if last line */
}





/* return TRUE if entering comment, FALSE if exiting */
void comment( )
{
    int     in_comment = 0;  /* comment flag */

    if( input_buffer[curchar] == '/' && input_buffer[curchar+1] == '*' )
        {
        in_comment++;
        while( in_comment )
            {
            if( !input_buffer[curchar] )
                fgets_sp( );
            if( input_buffer[curchar-1]=='*' && input_buffer[curchar]=='/' )
                in_comment--;
            curchar++;
            }
        }
}





/* look back from position n in string.  called with n indicating '('.
   determine function module_name  */
void lookbak( n )
int     n;
{
    int     i;

    while( !ischar( input_buffer[n] ) )
        {
        if( n == 0 )
            break;
        --n;
        }

    while( ischar( input_buffer[n-1] ) )    /* find leading blank */
        {
        if( n == 0 )
            break;
        --n;
        }

    /* save module_name */
    /* include variable declarations if module declaration */
    i = 0;
    if( open_braces == 0 )
        {
        while( input_buffer[n] )      /* full line if declaration */
            module_name[i++] = input_buffer[n++];
        }
    else
        {
        while( ischar( input_buffer[n] ) )      /* function call within function */
            module_name[i++] = input_buffer[n++];
        }

    module_name[i] = '\0';

    comout( module_name ); /* remove comment from module_name string */
}





/* terminate string at comment */
void comout( s )
char *s;
{
    char c; 

    while( c = *s++ ) 
        if( c == '/' )
            if( *s == '*' )
                {
                --s;
                *s++ = '\n';
                *s = '\0';
                break;
                }
}





/* display module name with indentation according to { open_braces */
/* returns 0 if not module, 1 if call within module, 2 if    */
/* module declaration  */
int modname( )
{
    int     j;

    if( unreserved( ) )
        {  /* test if builtin like while */
        if( open_braces == 0 )
            {
            printf( "\n}\n\n\n\n" );
            comout( input_buffer );
            printf( "%s{", input_buffer );
            return( 2 );
            }
        else
            {
            printf( "\n" );
            for( j=0; j < open_braces; ++j ) 
                printf( "  " );
            printf( "%s()", module_name );
            return( 1 );
            }
        }
    return( 0 );
}





/* test for names that are operators not functions */
int unreserved( )
{
    if( !strcmp( module_name, "return" ) )
        return( 0 );
    else if( !strcmp( module_name, "if" ) )
        return( 0 );
    else if( !strcmp( module_name, "while" ) )
        return( 0 );
    else if( !strcmp( module_name, "for" ) )
        return( 0 );
    else if( !strcmp( module_name, "switch" ) )
        return( 0 );
    else
        return( 1 );
}





/* test if character is one that program tracks */
int flowchar( c )
char c;
{
    return( (c == '{' || c == '}' || c == '\"' ) ? TRUE : FALSE );
} 





/* test for character */
int ischar( c )
char c;
{
    return( ( (c>='A' && c<='z') || (c>='0' && c<='9') ) ? TRUE : FALSE );
}





/* read a line of source */
int fgets_sp( )
{
    int  ch;
    char *s;

    s = &input_buffer[0];
    while( ( ch = getc( file_pointer ) ) != EOF )
        {
        *s++ = (char)ch;
        if( ch == '\n' )
            {
            *s = '\0';
            curchar = 0;
            return( TRUE );
            }
        }
    *s = '\0';
    return( FALSE );
} 
