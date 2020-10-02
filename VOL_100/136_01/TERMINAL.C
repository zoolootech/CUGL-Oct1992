/*	HEADER:  CUG136.03;
	TITLE:		TERMINAL;
	VERSION:	1.00;
	DATE:		4/24/1984;
	DESCRIPTION:	"Terminal installation program";
	KEYWORDS:	terminal interface,installation,configuration;
	SYSTEM:		CP/M;
	FILENAME:	TERMINAL.C;
	AUTHORS:	R. Rodman;
	COMPILERS:	C/80;
*/

/* TERMINAL - terminal configuration program

	831123 rr convert to c 1.002
	840206 rr make it work if terminal.sys doesn't exist 1.003
	840424 rr put library in program 1.004 */

/* compile: c -c2000 terminal */

char trmbuf[ 128 ];
char *trmdat[ 16 ];

/* width height defint uselst sndfst method xadd yadd
	inistr clrscn clreos clreol curoff curon curbeg
	curmid curend hiint lowint revvid norvid inslin dellin */

char *library[ 16 ][ 17 ] = {
	"ANSI",		"\120\030HNY1\001\001",
		"", "\033[2J", "\033[J", "\033[K", "", "", "\033[",
		";", "H", "\033[2m", "\033[0m", "\033[7m", "\033[0m", "\033[L",
		"\033[M",
	"Televideo 950", "\120\030HNY0\040\040",
		"", "\032", "\033Y", "\033T", "\033.0", "\033.2", "\033=",
		"", "", "\033(", "\033)", "\033G4", "\033G0", "\033E", "\033R",
	"Televideo 912/920", "\120\030HNY0\040\040",
		"", "\032", "\033Y", "\033T", "", "", "\033=",
		"", "", "\033(", "\033)", "\033j", "\033k", "\033E", "\033R",
	"Osborne I", "\064\030HNY0\040\040",
		"", "\032", "\033Y", "\033T", "", "", "\033=",
		"", "", "\033(", "\033)", "", "", "\033E", "\033R",
	"Beehive", "\120\030HNY0\040\040",
		"", "\033X\033E", "\033J", "\033K", "", "", "\033F",
		"", "", "\033W", "\033X", "", "", "", "",
	"ADM-3A", "\120\030HNY0\040\040",
		"", "\032", "", "", "", "", "\033=",
		"", "", "", "", "", "", "", "",
	"Soroc IQ-120", "\120\030HNY0\040\040",
		"", "\033*", "\033Y", "\033T", "", "", "\033=",
		"", "", "\033(", "\033)", "", "", "", "",
	"Hazeltine 1400/1500", "\120\030LNX0\000\140",
		"", "\033\014", "\033\030", "\033\017", "", "", "\033\021",
		"", "", "\033\031", "\033\037", "", "", "\033\032", "\033\023",
	"Perkin-Elmer Fox", "\120\030HNX0\040\040",
		"", "\033K", "\033J", "\033I", "", "", "\033X",
		"\033Y", "", "", "", "", "", "", "",
	"ADDS Regent 20", "\120\030HNY0\040\040",
		"", "\014", "", "\033K", "", "", "\033Y",
		"", "", "", "", "", "", "", "",
	"IBM 3101", "\120\030HNY0\040\040",
		"", "\033L", "\033J", "\033I", "", "", "\033Y",
		"", "", "", "", "", "", "\033N", "\033O",
	"Intertube", "\120\030HNY0\040\040",
		"", "\014", "\033k", "\033K", "", "", "\033Y",
		"", "", "", "", "", "", "", "",
	"Visual 200", "\120\030HNY0\040\040",
		"", "\033v", "\033y", "\033x", "", "", "\033Y",
		"", "", "\0333", "\0334", "\033s", "\033k", "\033L", "\033M",
	"Heath H-19", "\120\030HNY0\040\040",
		"", "\033E", "\033J", "\033K", "\033x5", "\033y5", "\033Y",
		"", "", "", "", "\033p", "\033q", "\033L", "\033M",
	"Datamedia", "\120\030HNX0\040\040",
		"", "\014", "\013", "\035", "", "", "\036",
		"", "", "\004", "\030", "", "", "", "",
	"Intecolor 3602", "\100\037HNX0\000\000",
		"", "\014", "", "", "", "", "\003",
		"", "", "\021", "\022", "\025", "\026", "\033U", "\033V"
};

main()
{
	char keyp;
	int i;
	char *p, *q, *r;

	puts( "Terminal Configuration Program v.1.004\n\n" );

	puts( "(1) Select terminal from library\n(2) Modify terminal (1/2):" );

	keyp = getch();

	switch( keyp ) {

		case '1' : {

			puts( "\n\n\n\n" );
			for( i = 0; i <= 15; i++ ) {
				putchar( '(' );
				putchar( 'A' + i );
				puts( ") " );
				puts( library[ i ][ 0 ] );
				putchar( '\n' );
			}

			keyp = '\0';
			while( keyp < 'A' || keyp > 'P' ) {
				puts( "\nEnter selection (A-P): " );
				keyp = getch();
				if( keyp >= 'a' ) keyp -= 32;
			}

			p = &trmbuf[ 0 ];

			for( i = 0; i <= 15; i++ ) {

				trmdat[ i ] = p;

/* move library info into terminal buffer */

				q = p;
				r = library[ keyp - 'A' ][ i + 1 ];

				while( *r ) *q++ = *r++;

				p++; p++; p++; p++;
				p++; p++; p++; p++;
			}

			term_write();
			break;
		}

		case '2' : {
			term_read();

			while( 1 ) {
				disp_rec();	/* display record */

				puts( "\n(C) Change lines in above," );
				puts( " (X) Exit program (C/X): " );
				keyp = getch();
				if( keyp >= 'a' ) keyp -= 32;

				switch( keyp ) {
					case 'C' : change_rec(); break;

					case 'X' : term_write(); exit();

					default : putchar( '\007' );
				}
			}
		}
	}
}

	/* read previous values of terminal file */
 
term_read()
{
	int t, u;
	char *p;
	t = fopen( "TERMINAL.SYS", "rb" );

	if( t == 0 ) {
		puts( "TERMINAL.SYS doesn't exist as yet." );
		for( u = 0; u < 128; ++u ) trmbuf[ u ] = '\0';
	} else {
		p = &trmbuf[ 0 ];
		read( t, p, 128 );
		fclose( t );
	}

	p = &trmbuf[ 0 ];

	for( u = 0; u <= 15; u++ ) {
		trmdat[ u ] = p;

		p++; p++; p++; p++;
		p++; p++; p++; p++;
	}
}

puts( p )
char *p;
{
	while( *p ) putchar( *p++ );
}

	/* write out .sys file */

term_write()
{
	int t, u;
	char *p;

	t = fopen( "TERMINAL.SYS", "wb" );
	p = &trmbuf[ 0 ];
	u = write( t, p, 128 );
	if( u < 128 ) puts( "\n?Write error!" );
	fclose( t );
}

	/* print integer in decimal */

putd( i )
int i;
{
	if( i > 99 ) putchar( '0' + i / 100 );
		else putchar( ' ' );

	i %= 100;
 
	if( i > 9 ) putchar( '0' + i / 10 );
		else putchar( ' ' );

	i %= 10;

	putchar( '0' + i );
}

	/* input a number */

int getd()
{
	int i, j;
	char c;

	i = 0;
	j = 0;
	while( ( c = getch() ) >= '0' && c <= '9' ) {
		j++;
		i = ( i * 10 ) + c - '0';
	}

	if( j ) return( i );
		else return( -1 );
}

	/* display a record */

disp_rec()
{
	int ind, x;
	char *p;

	puts( "\n\n\n" );

	for( ind = 1; ind <= 16; ind++ ) {
		putd( ind );	
		putchar( ' ' );

		switch( ind ) {
			case 1 : puts( "  Screen Parameters " ); break;
			case 2 : puts( "       Init. String " ); break;
			case 3 : puts( "       Clear Screen " ); break;
			case 4 : puts( "   Clear end screen " ); break;
			case 5 : puts( "     Clear end line " ); break;
			case 6 : puts( "         Cursor off " ); break;
			case 7 : puts( "          Cursor on " ); break;
			case 8 : puts( "    Address lead-in " ); break;
			case 9 : puts( "  Address separator " ); break;
			case 10 : puts( "    Address trailer " ); break;
			case 11 : puts( " Begin Hi Intensity " ); break;
			case 12 : puts( " Begin Lo Intensity " ); break;
			case 13 : puts( "Begin Reverse Video " ); break;
			case 14 : puts( "  End Reverse Video " ); break;
			case 15 : puts( "        Insert Line " ); break;
			case 16 : puts( "        Delete Line " ); break;
		}

		if( ind == 1 ) {
			p = trmdat[ 0 ];	/* get pointer */

			puts( "      Width:" );
			putd( *p++ );
			puts( "     Height:" );
			putd( *p++ );
			puts( "  Def. Int.:" );
			putchar( *p++ );
			puts( "\n                          Use Last:" );
			putchar( *p++ );
			puts( " Send first:" );
			putchar( *p++ );
			puts( "     Method:" );
			putchar( *p++ );
			puts( "\n                          X addend:" );
			putd( *p++ );
			puts( "   Y addend:" );
			putd( *p++ );
		} else {
			p = trmdat[ ind - 1 ];

			while( *p ) {
				if( *p < ' ' ) {
					putchar( '^' );
					putchar( *p++ + 64 ); 
				} else putchar( *p++ );
			}
		}

		puts( "\n" );
	}
}

	/* change a record */

change_rec()
{
	int ind, x;
	char c;
	char *p;

	puts( "\nLine to change? " );
	ind = getd();
	if( ind < 0 ) return;

	puts( "\n(Press RETURN for defaults in parentheses)\n" );

	if( ind == 1 ) {
		p = trmdat[ 0 ];

		puts( "\nWidth (" );
		putd( *p );
		puts( "): " );
		ind = getd();
		if( ind >= 0 ) *p = ind;
		p++;

		puts( "\nHeight (" );
		putd( *p );
		puts( "): " );
		ind = getd();
		if( ind >= 0 ) *p = ind;
		p++;

		puts( "\nDefault intensity (H or L) (" );
		putchar( *p );
		puts( "): " );
		c = getch();
		if( c == 'H' || c == 'L' ) *p = c;
		p++;

		puts( "\nUse Last (Y or N) (" );
		putchar( *p );
		puts( "): " );
		c = getch();
		if( c == 'N' || c == 'Y' ) *p = c;
		p++;

		puts( "\nSend first (X or Y) (" );
		putchar( *p );
		puts( "): " );
		c = getch();
		if( c == 'X' || c == 'Y' ) *p = c;
		p++;

		puts( "\nMethod (0 or 1) (" );
		putchar( *p );
		puts( "): " );
		c = getch();
		if( c == '0' || c == '1' ) *p = c;
		p++;

		puts( "\nX addend (" );
		putd( *p );
		puts( "): " );
		ind = getd();
		if( ind >= 0 ) *p = ind;
		p++;

		puts( "\nY addend (" );
		putd( *p );
		puts( "): " );
		ind = getd();
		if( ind >= 0 ) *p = ind;
		p++;
	} else {
		puts( "New value " );
		puts( "(use ^ followed by capital for control characters," );
		puts( "\nor press space bar to clear)\n(" );

		p = trmdat[ ind - 1 ];

		while( *p ) {
			if( *p < ' ' ) {
				putchar( '^' );
				putchar( *p++ + 64 ); 
			} else putchar( *p++ );
		}

		puts( "): " );

		c = getch();		/* read first char */
		if( c == '\r' ) return;	/* skip rest of input */

		p = trmdat[ ind - 1 ];

		for( x = 1; x <= 8; x++ )
			*p++ = '\0';	/* null out record */

		if( c == ' ' ) return;	/* leave cleared out */

		p = trmdat[ ind - 1 ];

	/* now get from 1 to 7 characters */

		for( x = 1; x <= 7; x++ ) {
			if( c == '^' ) c = getch() & 0x1f;
			*p++ = c;
			c = getch();
			if( c == '\r' ) return;
		}
	}
}

	/* get a char and duplex it */

int getch()
{
	int i;

	while( ! ( i = bdos( 6, 0xff )));
	putchar( i );
	return( i );
}

#include "stdlib.c"


