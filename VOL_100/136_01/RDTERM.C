/*	HEADER:  CUG136.12;
	TITLE:		RDTERM;
	VERSION:	1.00;
	DATE:		11/1/1983;
	DESCRIPTION:	"Terminal installation checkout program";
	KEYWORDS:	terminal interface,installation,configuration;
	SYSTEM:		CP/M;
	FILENAME:	RDTERM.C;
	AUTHORS:	R. Rodman;
	COMPILERS:	C/80;
*/

/* Program to read the TERMINAL.SYS file
	831101 rr convert to C 1.003 */

char trmbuf[ 128 ];
char *p;
int i;

int width, height, defint, uselst, yfirst, method, addx, addy;
char *inistr, *clrscn, *clreos, *clreol, *curoff, *curon,
	*curbeg, *curmid, *curend, *hiint, *loint, *revvid, *norvid,
	*inslin, *dellin;

main()
{
	puts( "Terminal Test Program v.1.003\n" );

	term_init();

	/* Begin testing of terminal */

	puts( "Clear screen: should not appear." );
	puts( clrscn );

	goxy( width - 18, 0 );
	puts( "Goxy: Upper Right" );

	goxy( 0, height - 1 );
	puts( "Delete line: Center" );
	goxy( 15, height / 2 + 2 );
	puts( "Delete line: should not appear." );
	goxy( 0, height / 2 );
	for( i = 1; i <= height / 2 - 1; i++ ) puts( dellin );

	goxy( width - 26, height / 2 + 1 );
	puts( "Insert line: Bottom Right" );
	goxy( 0, height / 2 + 1 );
	for( i = 1; i <= 10; i++ ) puts( inslin );

	goxy( 5, 4 );
	puts( revvid );
	puts( "Reverse Video" );
	puts( norvid );

	goxy( 30, 4 );
	puts( loint );
	puts( "Low Intensity" );

	goxy( 30, 5 );
	puts( hiint );
	puts( "High Intensity" );

	if( defint != 'H' ) puts( loint );

	goxy( 0, height - 1 );
	puts( "Press RETURN: " );
	i = getchar();
	exit();
}

term_init()
{
	int t, u;
	char *p;
	t = fopen( "TERMINAL.SYS", "r" );

	if( t == 0 ) {
		puts( "Run TERMINAL first, please." );
		exit();
	}

	p = &trmbuf[ 0 ];

	read( t, p, 128 );

	fclose( t );

	width = *p++;
	height = *p++;
	defint = *p++;
	uselst = *p++;
	yfirst = *p++;
	method = *p++;
	addx = *p++;
	addy = *p++;

	for( u = 1; u <= 15; u++ ) {
		switch( u ) {
			case 1 : inistr = p; break;
			case 2 : clrscn = p; break;
			case 3 : clreos = p; break;
			case 4 : clreol = p; break;
			case 5 : curoff = p; break;
			case 6 : curon = p; break;
			case 7 : curbeg = p; break;
			case 8 : curmid = p; break;
			case 9 : curend = p; break;
			case 10 : hiint = p; break;
			case 11 : loint = p; break;
			case 12 : revvid = p; break;
			case 13 : norvid = p; break;
			case 14 : inslin = p; break;
			case 15 : dellin = p; break;
		}

		p++; p++; p++; p++;
		p++; p++; p++; p++;
	}
}

	/* output a coordinate */

coord( c )
int c;
{
	switch( method ) {
		case '0' : putchar( c ); break;
		case '1' : {
			if( c >= 100 ) {
				putchar( '0' + c / 100 );
				c %= 100;
			}
			if( c >= 10 ) {
				putchar( '0' + c / 10 );
				c %= 10;
			}
			putchar( '0' + c );
		}
	}
}

	/* position cursor */

goxy( x, y )
int x, y;
{
	puts( curbeg );
	if( yfirst == 'Y' ) coord( y + addy );
		else coord( x + addx );
	puts( curmid );
	if( yfirst == 'Y' ) coord( x + addx );
		else coord( y + addy );
	puts( curend );
}

puts( p )
char *p;
{
	while( *p ) putchar( *p++ );
}


