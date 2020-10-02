/*	HEADER:  CUG136.08;
	TITLE:		DUMPSTAR;
	VERSION:	1.00;
	DATE:		9/10/1984;
	DESCRIPTION:	"DumpStar Video Game";
	KEYWORDS:	game,video;
	SYSTEM:		CP/M;
	FILENAME:	DUMPSTAR.C;
	AUTHORS:	R. Rodman;
	COMPILERS:	C/80;
*/

/*	DUMPSTAR

	830721 remove WS references
	830730 both x and y for enemies
	830908 convert to c
	830912 try to get working
	830913 correct bug in movea, make 40 cols
	830913 make 64 cols per sector
	830916 add blast routine
	830917 various fixes
	830926 acceleration, other misc changes
	830929 ground by calculation
	831013 some refinements
	831014 more refinements
	840306 changes for adm5
	840811 22 aliens, strategy aliens, became DUMPSTAR
	840910 read terminal file
*/

#include "printf.c"
#include "random.c"

/* value of ships includes the one you're playing with */

	int ax[ 22 ], ay[ 22 ], dx[ 22 ], dy[ 22 ];
	char as[ 22 ][ 3 ];
	int aliens, ships, other, gnd1ind, gnd2ind;
	int ux, uy, uface, uspeed, uaccel, score, oldsco;

	char hi_name[ 10 ][ 16 ];
	int hi_cnt, hi_scor[ 10 ];

	char trmbuf[ 128 ];
	char *p;
	int i;

	int width, height, defint, uselst, yfirst, method, addx, addy;
	char *inistr, *clrscn, *clreos, *clreol, *curoff, *curon,
		*curbeg, *curmid, *curend, *hiint, *loint, *revvid, *norvid,
		*inslin, *dellin;

/* main program */

main()
{
	char wplayer(), inkey();

	terminit();			/* init terminal file */

	while( wplayer() != 'Q' ) {	/* wait for a player */

		init();
		take();			/* take a ship */

		while( ships && aliens ) {
			ckkey();	/* check user input */

			update();	/* update screen */

			movea();	/* move aliens */

			if( ux == 999 ) take();
		}

		if( ships ) {
			goxy( 0, 20 );
			puts( "YOU GOT THEM ALL! PRESS A KEY: " );
			puts( curon );
			while( ! inkey() );
		}
	}

	puts( clrscn );		/* exit if player typed 'Q' */
	puts( curon );

	if( defint != 'H' ) puts( loint );
}

terminit()
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

/* get a random number between 0 and range */

rnd( range )
int range;
{
	unsigned q, rand();

	q = rand();
	return( q % range );
}

/* initialize game */

init()
{
	int j;

	srand( 0 );

	score = 0;
	oldsco = 0;
	aliens = 22;

	for( j = 0; j < 22; j++ ) {
		ax[ j ] = rnd( 512 );
		dx[ j ] = 2 * rnd( 2 ) - 1;
		dy[ j ] = 2 * rnd( 2 ) - 1;
		ay[ j ] = rnd( 22 );
	}

	for( j = 0; j < 8; ++j ) {
		dx[ j ] = 0;		/* mines */
		dy[ j ] = 0;
		strcpy( as[ j ], "::" );
	}

	for( j = 8; j < 16; ++j )
		if( dx[ j ] > 0 ) strcpy( as[ j ], ">o" );
			else strcpy( as[ j ], "o<" );	/* fighters */

	for( j = 16; j < 22; ++j )
		if( dx[ j ] > 0 ) strcpy( as[ j ], ")O" );
			else strcpy( as[ j ], "O(" );	/* flagships */

	ships = 4;	/* initial count of ships, extra for init take() */

	gnd1ind = 0;
	gnd2ind = 0;
	other = 0;
}

/* check keyboard for a char */

char inkey()
{
	return toupper( bdos( 6, 0xFF ) & 0x7F );
}

/* send out string */

puts( c )
char *c;
{
	while( *c ) putchar( *c++ );
}

/* wait for someone to play */

char wplayer()
{
	char c;

	puts( clrscn );

	puts( "\n\n\n" );

	puts( "[[[[[[                                 \n" );
	puts( "[[[ [[[                                \n" );
	puts( "[[[  [[[ [[[  [[[ [[[[   [[[[ [[[[[[[  \n" );
	puts( "[[[  [[[ [[[  [[[ [[[[[ [[[[[ [[[  [[[ \n" );
	puts( "[[[  [[[ [[[  [[[ [[[[[[[[[[[ [[[ [[[  \n" );
	puts( "[[[  [[[ [[[  [[[ [[[ [[[ [[[ [[[[[[   \n" );
	puts( "[[[  [[[ [[[  [[[ [[[     [[[ [[[      \n" );
	puts( "[[[[[[[   [[[[[[  [[[     [[[ [[[      \n" );
	puts( "                                       \n" );
	puts( "  [[[[[[[[[[[[[[[[[  [[[      [[[[[[[  \n" );
	puts( " [[[[       [[[     [[[[[     [[[  [[[ \n" );
	puts( "  [[[[      [[[    [[[ [[[    [[[ [[[  \n" );
	puts( "   [[[[     [[[   [[[   [[[   [[[[[[   \n" );
	puts( "    [[[[    [[[  [[[[[[[[[[[  [[[[[[[  \n" );
	puts( "     [[[[   [[[ [[[       [[[ [[[  [[[ \n" );
	puts( "      [[[[                             \n" );
	puts( "]]]]]][[[                              \n" );

	puts( "\n\n" );

	puts( curon );
	puts( "PRESS ANY KEY TO PLAY (Q TO QUIT): " );

	while( c = inkey() );	/* clear up any keypresses pending */
	while( ! ( c = inkey() ));	/* wait for a new keypress */

	puts( curoff );

	return( c );
}

/* set up screen */

setup()
{
	int j;

	puts( clrscn );
	puts( curoff );

	for( j = 0; j <= 23; j++ ) {
		goxy( 64, j );
		puts( loint );
		putchar( '|' );
		puts( hiint );
	}

	/* show ships in storage, ships - 1 */

	for( j = 13; j < ships + 12; j++ ) ship( 67, j, 1 );

	ux = 30;
	uy = 14;
	uface = 1;
	uspeed = 1;
	uaccel = 0;

	ship( ux, uy, uface );
}

/* take a ship away */

take()
{
	int k;

	--ships;
	if( ! ships ) return;

	for( k = 1; k < 30000; k++ );	/* delay */

	setup();		/* repaint screen */
}

/* add another ship */

give()
{
	ships++;
	ship( 65, ships + 13, 1 );
}

/* draw ship */

ship( x, y, p )
int x, y, p;
{
	goxy( x, y );
	switch( p ) {
		case -1 : puts( "<>==<" ); break;
		case 0 : puts( "     " ); break;
		case 1 : puts( ">==<>" ); break;
	}
}

/* evaluate keyboard command */

ckkey()
{
	int k;

	k = inkey();

	switch( k ) {
		case 'Q' : ships = 0; break;

		case 'C' : if( uy < 21 ) {
				ship( ux, uy, 0 );
				ship( ux, ++uy, uface );
			}; break;

		case 'E' : if( uy > 0 ) {
				ship( ux, uy, 0 );
				ship( ux, --uy, uface );
			}; break;

		case 'S' :	uface = -1;
				uspeed = 1;
				uaccel = 0;
				ship( ux, uy, uface );
				break;

		case 'F' :	uface = 1;
				uspeed = 1;
				uaccel = 0;
				ship( ux, uy, uface );
				break;

		case ' ' :	fire();
				break;
	}
	uaccel++;
	if( uaccel >= 10 && uspeed < 3 ) {
		++uspeed;	/* accelerate up to 3 */
		uaccel = 0;
	}
}

/* fire a shot */

fire()
{
	int ll, ul, j, k;

	ll = ux + 5;	/* lower limit */
	ul = 63;

	if( uface < 0 ) {
		ll = ux - 2;
		ul = -1;
	}

	for( j = ll; j != ul; j += uface ) blast( j, uy, '-' );
}

/* move aliens */

movea()
{
	int j, k, xx;
	int q_cnt[ 8 ];

	for( j = 0; j < 8; j++ ) q_cnt[ j ] = 0;

	for( j = 0; j < 22; j++ ) {
		if( ax[ j ] == 999 ) {
			/* alien destroyed */
		} else {
			/* clear old pic of alien */

			for( k = 0; as[ j ][ k ]; k++ ) {
				xx = ax[ j ] + k;
				if( xx > 511 ) xx -= 512;

				if( ( xx >= 0 ) && ( xx <= 63 ) ) {
					goxy( xx, ay[ j ] );
					putchar( ' ' );
				}
			}

			ax[ j ] += dx[ j ];	/* movement of alien */
			ax[ j ] -= uface * uspeed;	/* movement of ship */
			if( ax[ j ] < 0 ) ax[ j ] += 512;
			if( ax[ j ] > 511 ) ax[ j ] -= 512;

			ay[ j ] += dy[ j ];

			if( ay[ j ] < 0 || ay[ j ] > 21 ) {
				dy[ j ] = -dy[ j ];	/* reverse */
				ay[ j ] += dy[ j ];	/* direction */
			}

/* now check strategy.  first mines */

			if( j < 8
				&& absdif( ax[ j ], ux ) < 6
				&& absdif( ay[ j ], uy ) < 6 ) {
				xx = ax[ j ];
				ax[ j ] = 999;	/* get rid of alien */
				--aliens;
				explode( xx, ay[ j ] );
			}

			if( j >= 8 && j < 16
				&& absdif( ax[ j ], ux ) > 8 ) {
					dx[ j ] = sign( ux - ax[ j ] );
					if( dx[ j ] > 0 )
						strcpy( as[ j ], ">o" );
						else strcpy( as[ j ], "o<" );
			}

			if( j >= 16
				&& absdif( ax[ j ], ux ) > 16 ) {
					dx[ j ] = sign( ux - ax[ j ] );
					if( dx[ j ] > 0 )
						strcpy( as[ j ], ")O" );
						else strcpy( as[ j ], "O(" );
			}

/* draw new pic of alien */

			for( k = 0; as[ j ][ k ]; k++ ) {
				xx = ax[ j ] + k;
				if( xx > 511 ) xx -= 512;

				if( ( xx >= 0 ) && ( xx <= 63 ) ) {
					goxy( xx, ay[ j ] );
					putchar( as[ j ][ k ] );
				}

/* check for collision */

				if( ( xx >= ux && xx <= ux + 4 )
					&& ( ay[ j ] == uy ) ) {
					ax[ j ] = 999;
					--aliens;
					explode( xx, ay[ j ] );
				}
			}

			q_cnt[ ax[ j ] / 64 ]++;
		}
	}

	for( j = 0; j < 8; j++ ) {
		switch( j ) {
			case 0 : goxy( 69, 6 ); break;
			case 1 : goxy( 72, 6 ); break;
			case 2 : goxy( 72, 8 ); break;
			case 3 : goxy( 72, 10 ); break;
			case 4 : goxy( 69, 10 ); break;
			case 5 : goxy( 66, 10 ); break;
			case 6 : goxy( 66, 8 ); break;
			case 7 : goxy( 66, 6 ); break;
		}
		printf( "%2d", q_cnt[ j ] );
	}
}

/* update score, etc. */

update()
{
	char t;
	int i;

	if( score != oldsco ) {
		oldsco = score;
		goxy( 66, 0 );
		puts( loint );
		printf( "Score: %d", score );
		puts( hiint );
	}

	other = ! other;	/* only repaint gnd1 every other time */

/*	ground should look like this, roughly:

	_/_______/_______/______|_____\_______\________\_
	/	/	/	|	\	\	\	*/

	goxy( 0, 23 );
	puts( loint );
	for( i = gnd2ind; i < ( gnd2ind + 63 ); i++ ) {
		if( i % 16 ) putchar( ' ' );
			else if( i - gnd2ind < 30 )
				putchar( '/' );
				else if( i - gnd2ind > 34 )
					putchar( '\\' );
					else putchar( '|' );
	}
	puts( hiint );

	if( ! other ) {
		goxy( 0, 22 );
		puts( loint );
		for( i = gnd1ind; i < ( gnd1ind + 63 ); i++ ) {
			if( i % 12 ) putchar( '_' );
				else if( i - gnd1ind < 30 )
					putchar( '/' );
					else if( i - gnd1ind > 34 )
						putchar( '\\' );
						else putchar( '|' );
		}
		puts( hiint );

		gnd1ind += uface * uspeed;
	}

	puts( hiint );

	gnd2ind += uface * uspeed;
}

/* blast a location */

blast( x, y, p )
int x, y;
char p;
{
	int k, tx;

	if(( x > 63 ) || ( x < 0 )) return;

	goxy( x, y );
	putchar( p );

	if(( x >= ux && x <= ux + 4 ) && y == uy ) {
		tx = ux;
		ux = 999;
		explode( tx, uy );		/* explode you */
	}

	for( k = 0; k < 22; k++ ) {
		if( ( ay[ k ] == y ) && ( ax[ k ] == x ) ) {
			tx = ax[ k ];		/* save x */
			ax[ k ] = 999;		/* remove alien */
			--aliens;		/* count down aliens left */
			score += 10;		/* add 10 points */
			explode( tx, ay[ k ] );
		}
	}

	for( k = 1; k < 150; k++ );		/* delay */

	goxy( x, y );
	putchar( ' ' );
}

/* explosion subroutine */

explode( x, y ) 
int x, y;
{
	blast( x, y - 1, '|' );
	blast( x - 2, y, '-' );
	blast( x, y + 1, '|' );
	blast( x + 2, y, '-' );
	blast( x - 2, y - 1, '\\' );	/* explode in a ring around it */
	blast( x - 2, y + 1, '/' );
	blast( x + 2, y + 1, '\\' );
	blast( x + 2, y - 1, '/' );

	blast( x, y - 2, ':' );		/* outer ring */
	blast( x - 4, y, ':' );
	blast( x, y + 2, ':' );
	blast( x + 4, y, ':' );

	blast( x - 4, y - 2, '.' );
	blast( x + 4, y + 2, '.' );
	blast( x - 4, y + 2, '.' );
	blast( x + 4, y - 2, '.' );
}

absdif( x, y )
int x, y;
{
	if( x > y ) return x - y;
		else return y - x;
}

sign( x )
int x;
{
	if( x > 0 ) return 1;
	if( x < 0 ) return -1;
	return 0;
}

#include "stdlib.c"


