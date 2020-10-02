/*
HEADER:		;
TITLE:		Screen entry;
VERSION:	1.0;

DESCRIPTION:	"General screen oriented input/output module.
		This is a component of the diskette sector editor,
		SE.C, but may be useful in other C programs.";

KEYWORDS:	Editor, screen;
SYSTEM:		CP/M-80, V2.2;
FILENAME:	SENTER.C;
SEE-ALSO:	SE.C, SE.DOC;
AUTHORS:	Jan Larsson;
COMPILERS:	BDS C, v1.43 and 1.5a;
*/
/***********************************************************

Version 1.0, 1981:
	Jan Larsson, Kosterv. 12, S-181 35  Lidingo, SWEDEN

************************************************************/

#include <bdscio.h>

#define LCHR '_'
#define ECHR ' '
#define BS 0x08
#define ESC 0x1B

/* Raw character output routine */

co( c )
char c ;
{
	bdos( 6, c );
	if(c == 0x0a)bdos( 6, 0x0d );
}


/* Raw character input routine */

ci(){
	char c ;

	while((c = bdos( 6, 0xff )) == 0);
	if(c == 0x03)stop();
	return( c );
}


/* tplot plots a text at the cursor adress given */

tplot( x, y, text )
char x, y, *text ;
{
	gotoxy( x, y );
	pl( text );
}


/* print a text at the current position */

pl( text )
char *text ;
{
	while(*text != '\0') co( *text++ );
}





bit( c, number )
char c, number ;
{
	return(((c >> number) & 0x01));
}




/*
	ENTER() is a general input routine for CRT terminals with cursor
	adressing. It is called with six parameters (described below) and
	returns TRUE if any characters were input, else FALSE is returned.
	A special case occurs if the ESCAPE key was depressed, if so the
	function will exit immediately and return ESCAPE.

	Call parameters:
		
		retval = enter( x, y, text, len, inbuf, mode );

		x 	is the column on the screen

		y	is the row on the screen (0 - 23)

		text    a pointer to the text printed before input

		len	is the maximum number of bytes allowed as input

		inbuf   a pointer to the buffer where input will be copied

		mode	a byte which controls the functioning of enter(),
				bit 0 :  only numerical input accepted
				bit 1 :  upper case conversion
				bit 2 :  wait for input
				bit 3 :  print contents of inbuf first
				bit 4 :  print contents of text
				bit 5 :  clear inbuf before inbuf, after bit 3
				bit 6 :  print 'mark' line on screen
				bit 7 :  not implemented yet

			
		    (c) 1981 Jan Larsson, Occam Software
*/


char enter( x, y, text, len, inbuf, mode )
char x, y, *text, len, *inbuf, mode ;
{
	int z ;
	char buf[255], x2, y2, numerical, upper, print, input, ledtext ;
	char nolla, wrap, n, *p, c, points ;

	numerical = bit( mode, 0 );
	upper     = bit( mode, 1 );
	input     = bit( mode, 2 );
	print     = bit( mode, 3 );
	ledtext   = bit( mode, 4 );
	nolla	  = bit( mode, 5 );
	points    = bit( mode, 6 );

	y2 = y ; x2 = x + strlen( text );

	if( ledtext )tplot( x, y, text );

	if( print ){
		tplot( x2, y2, inbuf );
		if( points ){
			n = len - strlen( inbuf );
			while(n-- > 0)co(LCHR);
			co( ECHR );
			}
		}
	else if( points ){
		gotoxy( x2, y2 );
		n = len ;
		while(n-- > 0)co( LCHR );
		co( ECHR );
		}

	if( nolla )*inbuf = '\0' ;

	if( ! input )return( FALSE );

	gotoxy( x2, y2 ); bell() ; c = '\0' ;
	p = buf ; z = 0 ; wrap = FALSE ;

	while( ! eoi( c ) ){
		if( numerical )while( nodigit((c = ci())));
		else while( unacceptable((c = ci())));
		if(c == ESC)return( ESC );
		if((p - buf) >= len && c != BS && ! eoi( c )){
			 wrap = TRUE ; gotoxy( x2, y2 ); }
		if(c == BS && p == buf)bell();
		else if(c == BS){
			z-- ; p-- ;
			co( BS ) ; co( LCHR ) ; co( BS ) ;
			}
		else if( ! eoi( c ) ){
			c = upper ? toupper( c ) : c ;
			if( wrap ){
				wrap = FALSE ;
				n = len ;
				bell() ;
				while(n-- > 0)co( LCHR );
				co( ECHR );
				gotoxy( x2, y2 );
				z = 0 ; p = buf ;
				}
			*p++ = c ;
			z++ ;
			co( c );
			}
		}
	*p = '\0' ;
	p-- ;
	if(z == 0)return( FALSE );
	while(*p == ' '){ co( BS ) ; co( LCHR ) ; co( BS ); p-- ; }
	p++ ;
	*p = '\0' ;
	strcpy( inbuf, buf );
	return( TRUE );
}



bell(){ co( 7 ); }


/* eoi() returns TRUE when a terminating character has been typed */

char eoi( c )
char c ;
{
	switch ( c ) {

		case 0x0d :
		case 0x0a : return( TRUE ) ; break ;

		default   : return( FALSE ) ;
		}
}




/* nodigit() returns true if the character is not a digit */

char nodigit( c )
char c ;
{
	if((c >= '0' && c <= '9') || c == BS || c == ESC || eoi( c ))
							return( FALSE );
	else return( TRUE );
}


/* unacceptable() returns TRUE if c is control character */

char unacceptable( c )
char c ;
{
	if((c >= ' ' && c < 0x7f) || eoi( c ) || c == BS || c == ESC)
							return( FALSE );
	else return( TRUE );
}



