/*	HEADER:  CUG136.19;
	TITLE:		CMAZE;
	VERSION:	1.00;
	DATE:		5/29/1984;
	DESCRIPTION:	"3-D Maze Game";
	KEYWORDS:	game,maze;
	SYSTEM:		CP/M;
	FILENAME:	CMAZE.C;
	AUTHORS:	R. Rodman;
	COMPILERS:	C/80;
*/

/*		>< CMAZE by R. Rodman ><

	5/6/81 v0.8
	9/14/81 v0.9 (Basic-E)
	1/15/82 back to MBASIC
	840512 convert to c
	840513, 840514, 840515, 840516, 840518 rr continue conversion
	840529 fix boxes 1.00

	Robots move crudely.
	Find objects in boxes:  Compass - Radio - etc.
	Shoot algorithm could show an alternating / \ or something.
	Solving maze is difficult and gets boring.  Tools would help. */

#define MX 15           /* define maximum X dimension of maze */
#define MY 10		/* define maximum Y dimension of maze */
#define MXMY 150	/* product of mx and my */

#define CLRSCN "\033+"
#define REVVID "\033G4"
#define NORVID "\033G0"
#define CURBEG "\033="
#define CLREOS "\033Y"
#define CUROFF "\033.0"
#define CURON "\033.2"

	int maze[ MX ][ MY ];

	int rx[ 4 ], ry[ 4 ];
	char *robot[ 4 ][ 9 ] = {
		"  _______",
		" / o' `o \\",
		" \\ ----- /",		/* close robot, height 9 */
		"  [TTTTT]",
		" /[=====]\\",
		"/ [MMMMM] \\",
		") [=====] (",
		" / \\   / \\",
		"/   \\ /   \\",

		" _____",
		"/ o o \\",
		"\\ --- /",
		"/[TTT]\\",		/* height 7 */
		")[===](",
		" [===]",
		"/ \\ / \\", "", "",

		" ____",
		"< o_o>",
		"/[TT]\\",
		" [==]",		/* height 5 */
		"/ \\/ \\", "", "", "", "",

		"<oo>",
		"/[]\\",		/* dist 4, height 3 */
		"/\\/\\", "", "", "", "", "", ""
	};

	int boxx[ 4 ], boxy[ 4 ];
	char *boxes[ 3 ][ 3 ] = {
		" ____",
		"|\\ ___\\",
		" \\|____|",

		" __",
		"<__>", "",

		"<>", "", "" 
	};

	int seed;

	int plyr_x, plyr_y, plyr_face;		/* player x, y, direction */

	int alive;		/* nonzero while alive */
	int won;		/* won game */

	int comp, radi, shots;

	int repaint;		/* nonzero to cause repaint */

/* main program */

main()
{
	instructions();		/* print instructions */

	randomize();            /* randomize seed */
	generate();		/* generate maze */

	init_robots();
	init_boxes();

	plyr_face = 2;			/* face to "east" */

	alive = 1;
	repaint = 1;
	won = 0;
	
	while( alive && ! won ) {
		if( repaint ) {
			depict();
			ck_boxes();
			repaint = 0;
		}
		ck_instruments();
		if( ck_robots()) alive = 0;
			else {
			get_command();
			update_robots();
		}
	}
	sound( ",,,,,,,,,,,,,,,," );	/* shut up */
	puts( CURON );
}

/* return random number between 0 and n-1 */

fnr( n )
int n;
{
	return ( rnd() % n );
}

int rnd()
{
	seed = seed * 3;
	if( seed > 10000 ) seed = seed / 4;
	return seed;
}

randomize()
{
	int i;
	seed = 1;
	puts( "<Press a key>" );
	while( bdos( 6, 0xFF ) == 0 ) i = rnd();
}

goxy( x, y )
int x, y;
{
	puts( CURBEG );
	putchar( y + 32 );
	putchar( x + 32 );
}

/* print instructions */

instructions()
{
	puts( CLRSCN );
	puts( "\t\t\t> > > A M A Z E ! < < <\n\n" );
	puts( "\nVersion 1.00\n\n" );
	puts( "This program presents a Perspective Maze Display on your\n" );
	puts( "terminal.  Your object is to get through this maze.\n" );
	puts( "four robots in the maze which will attempt to\n" );
	puts( "capture you.  You have a laser pistol to protect yourself;\n" );
	puts( "the pistol can fire only 5 shots.\n\n" );
	puts( "On each turn, you will be asked for a command, which must\n" );
	puts( "be made up of any combination of:\n\n" );
	puts( "S to step forward one square\n" );
	puts( "L to turn to your left, R to turn to your right\n" );
	puts( "F to fire the laser pistol\n" );
	puts( "O to open a box\n\n" );
	puts( "I am generating the maze now.\n" );

	sound( ",,,,,,,,,,,,,,,," );	/* turn sound off */
}

/*	>< Generate Maze ><

This program uses the algorithm described by R J Bishop
in his article in Byte Oct 78 P 136 */

generate()
{
	int x, y;
	int lc, oc, wf, bx, by;
	int to_bit, from_bit, tp, path;
	int qu[ MXMY ][ 2 ];

	for( x = 0; x < MX; ++x )
		for( y = 0; y < MY; ++y )
			maze[ x ][ y ] = 0;

	lc = 0;		/* last cell in queue */
	oc = 0;		/* last cell taken out of queue */
	wf = 0;		/* found win path yet? */

	plyr_x = 0;
	plyr_y = fnr( MY );	/* player start position */

	bx = plyr_x;
	by = plyr_y;		/* random walk start position */
	
	x = 1; y = 1;		/* junk values to cause loop to execute */

/* loop while any valid starting points for walks are in the queue */

	while( oc <= lc ) {

/* random walk from bx, by until we come to a dead end or generate a goal. */

		x = bx;
		y = by;

		while( x + y ) {

/* inner loop, try to move to another square from bx, by */

			tp = 0;		/* tried path */
			while( tp < 15 ) {
				x = bx;
				y = by;
				path = fnr( 4 );	/* get random path */

				switch( path ) {

				case 0 : 	to_bit = 1;
						from_bit = 2;
						--x;
						break;

				case 1 : 	to_bit = 2;
						from_bit = 1;
						++x;
						break;

				case 2 : 	to_bit = 4;
						from_bit = 8;
						--y;
						break;

				case 3 : 	to_bit = 8;
						from_bit = 4;
						++y;
						break;
				}

				tp = tp | to_bit;

/* break out of loop if we hit an empty square */

				if( x >= 0 && x <= ( MX - 2 )
					&& y >= 0 && y < MY )
					if( maze[ x ][ y ] == 0 ) break;

/* otherwise try again */

			}

/* see if we got out, or just fell out */

			if( tp >= 15 ) {
				x = 0;		/* no path left */
				y = 0;
			}

/* if not, we must have a good path here */

			if( x + y ) {

/* flag where we opened path to */

				maze[ bx ][ by ] = maze[ bx ][ by ] | to_bit;

				bx = x;		/* new origin */
				by = y;

/* flag where we opened path from */

				maze[ bx ][ by ] = maze[ bx ][ by ] | from_bit;

				qu[ lc ][ 0 ] = bx;
				qu[ lc ][ 1 ] = by;	/* save in queue */
				++lc;

/* have we no solution yet, and are we on next to last column of maze? */

				if( wf == 0 && bx == ( MX - 2 )) {

/* if so, generate a goal */

					++wf;		/* mark goal found */

					maze[ bx ][ by ] =
						maze[ bx ][ by ] | 2;
					maze[ MX - 1 ][ by ] = 1;

					x = 0;		/* cause no path */
					y = 0;
				}
			}
		}

/* no path. what do you do now? */

		if( lc == MXMY ) break;	/* give up if we've used all */

		bx = qu[ oc ][ 0 ];	/* pull next starting point */
		by = qu[ oc ][ 1 ];	/* from the queue */
		++oc;
	}
}

init_robots()
{
	int i;
	for( i = 0; i < 4; ++i ) {
		rx[ i ] = fnr( MX - 3 ) + 1;
		ry[ i ] = fnr( MY );
	}

	shots = 5;
}

init_boxes()
{
	int i;
	for( i = 0; i < 4; ++i ) {
		boxx[ i ] = fnr( MX - 3 ) + 1;
		boxy[ i ] = fnr( MY );
	}

	comp = 0;	/* you have no compass */
	radi = 0;	/* you have no radio */
	shots = 5;
}

/* depict view from current position and location */

depict()
{
	int lsm, rsm;	/* left side, right side masks */
	int wx, wy;	/* wall x, y */
	int mw, wf, w;
	int lw[ 6 ], rw[ 6 ];
	char ldol, rdol, wdol[ 3 ];
	int js, l1, l2, j1, j2, l, j, mi; 

/* define side masks */
	
	switch( plyr_face ) {

	case 1 :	lsm = 8;
			rsm = 4;
			break;

	case 2 :	lsm = 4;
			rsm = 8;
			break;

	case 4 :	lsm = 1;
			rsm = 2;
			break;

	case 8 :	lsm = 2;
			rsm = 1;
			break;
	}

/* now we know which way to look */

	wx = plyr_x;
	wy = plyr_y;
	mw = 4;		/* max wall even if we don't run into one */
	wf = 0;

	w = 0;

	while( w < mw ) {

		lw[ w ] = maze[ wx ][ wy ] & lsm;
		rw[ w ] = maze[ wx ][ wy ] & rsm;

/* mw is the wall beyond which we cannot see */

		if( ! ( maze[ wx ][ wy ] & plyr_face )) {
			mw = w;
			lw[ w + 1 ] = 1;
			rw[ w + 1 ] = 1;
		}

		if( wx == MX - 1 ) wf = 1;

		switch( plyr_face ) {
		case 1 :	--wx;	break;
		case 2 :	++wx;	break;
		case 4 :	--wy;	break;
		case 8 :	++wy;	break;
		}

		++w;
	}

/* got wall arrays, now draw them */

	puts( CLRSCN );		/* clear the screen */
	puts( CUROFF );

	ldol = '\\';
	rdol = '/';

/* js is 1 for top half of screen, -1 for bottom */

	for( js = 1; js >= -1; js -= 2 ) {

		if( js == 1 ) {
			l1 = 0;
			l2 = 4;
			j1 = 0;
			j2 = 1;
		} else {
			l1 = 4;
			l2 = 0;
			j1 = 1;
			j2 = 0;
			ldol = '/';
			rdol = '\\';
		}

/* l is line of screen, going 0 to 4 on top, then 4 to 0 on bottom. */

		for( l = l1; l != l2 + js; l += js ) {

/* j is used to cause two screen lines to be used for each step of l. */

			for( j = j1; j != j2 + js; j += js ) {

/* how many spaces go in the middle of line l? */

				mi = 40 - 6 * l;

/* draw left side old walls, ie walls already passed by l. */

				for( w = 0; w < l; ++w ) {

					if( w > mw ) puts( "   " );
						else {

						if(( w == l - 1 )
							&& ( lw[ w ] != 0 )
							&& ( j == 0 ))
							wdol[ 0 ] = '-';
							else wdol[ 0 ] = ' ';

						wdol[ 1 ] = wdol[ 0 ];
						wdol[ 2 ] = wdol[ 1 ];

						if(( lw[ w ] > 0 )
							&& ( w != mw ))
							wdol[ 2 ] = '|';

						if(( lw[ w ] == 0 )
							&& ( lw[ w + 1 ] != 0
							))
							wdol[ 2 ] = '|';

						putchar( wdol[ 0 ] );
						putchar( wdol[ 1 ] );
						putchar( wdol[ 2 ] );
					}
				}

/* left side new wall */

				switch( j ) {

				case 0 :	if( l == ( mw + 1 )) {
							puts( "--" );
							break;
						}
						if( l > mw ) {
							puts( "  " );
							break;
						}
						if( lw[ l ] == 0 ) {
							putchar( ldol );
							putchar( ' ' );
						} else puts( "  " );

						break;

				case 1 :	if( l > mw ) {
							puts( "  " );
							break;
						}
						if( lw[ l ] == 0 ) {
							putchar( ' ' );
							putchar( ldol );
						} else puts( "  " );
				}

				if(( l == ( mw + 1 )) && ( j == 0 ))
					repchr( mi, '-' );
					else repchr( mi, ' ' );

/* right side new walls */

				switch( j ) {

				case 0 :	if( l == ( mw + 1 )) {
							puts( "--" );
							break;
						}
						if( l > mw ) {
							puts( "  " );
							break;
						}
						if( rw[ l ] == 0 ) {
							putchar( ' ' );
							putchar( rdol );
						} else puts( "  " );

						break;

				case 1 :	if( l > mw ) {
							puts( "  " );
							break;
						}
						if( rw[ l ] == 0 ) {
							putchar( rdol );
							putchar( ' ' );
						} else puts( "  " );
				}

/* right side old walls */

				for( w = l - 1; w >= 0; --w ) {

					if( w > mw ) puts( "   " );
						else {

						if(( w == l - 1 ) 
							&& ( rw[ w ] != 0 )
							&& ( j == 0 ))
							wdol[ 0 ] = '-';
							else wdol[ 0 ] = ' ';

						wdol[ 1 ] = wdol[ 0 ];
						wdol[ 2 ] = wdol[ 1 ];

						if(( rw[ w ] > 0 )
							&& ( w != mw ))
							wdol[ 0 ] = '|';

						if(( rw[ w ] == 0 )
							&& ( rw[ w + 1 ] != 0
							))
							wdol[ 0 ] = '|';

						putchar( wdol[ 0 ] );
						putchar( wdol[ 1 ] );
						putchar( wdol[ 2 ] );
					}
				}

				putchar( '\n' );

			}	/* loop on j */

		}	/* loop on l */

	}	/* loop on js (top or bottom) */

	puts( CLREOS );

/* draw exit sign */

	if( wf ) {
		goxy( 18, 10 );
		puts( "E X I T" );
	}
}

/* check for robots */

int ck_robots()
{
	int wx, wy, k, dr, r, d, h, w;

	wx = plyr_x;
	wy = plyr_y;

	k = 0;
	dr = 99;

	w = 0;

/* step forward along player's view */

	while( maze[ wx ][ wy ] & plyr_face ) {

		switch( plyr_face ) {
		case 1 :	--wx;	break;
		case 2 :	++wx;	break;
		case 4 :	--wy;	break;
		case 8 :	++wy;	break;
		}

		for( r = 0; r < 4; ++r ) {
			if( rx[ r ] == wx && ry[ r ] == wy ) {

				for( h = 0; h < 9; ++h ) {
					goxy( 15 + w, 13 + h );
					puts( robot[ w ][ h ] );
				}
			}

			if( rx[ r ] == plyr_x && ry[ r ] == plyr_y ) k = r;

			d = abs( rx[ r ] - plyr_x ) + abs( ry[ r ] - plyr_y );
			if( d < dr ) dr = d;
		}
		++w;	/* increment distance */
	}

	if( dr < 5 ) {

		sound( "255,9,0,0,1,0,1,234" );
		cricket( 8, 9 - 2 * dr );	/* make a robot sound */
		cricket( 9, 9 - 2 * dr );
		cricket( 10, 9 - 2 * dr );

	} else sound( ",,,,,,,,,,,,,,,," );

	if( k ) {
		goxy( 50, 22 );
		puts( "Aighh! The robots got you!" );
	}

	return k;	/* nonzero value means you got it */
}

/* check boxes */

ck_boxes()
{
	int wx, wy, b, d, h, w;

	wx = plyr_x;
	wy = plyr_y;

	w = 0;

/* look forward along player's view */

	while( maze[ wx ][ wy ] & plyr_face ) {

		switch( plyr_face ) {
		case 1 :	--wx;	break;
		case 2 :	++wx;	break;
		case 4 :	--wy;	break;
		case 8 :	++wy;	break;
		}

		if( w < 3 ) for( b = 0; b < 4; ++b ) {
			if( boxx[ b ] == wx && boxy[ b ] == wy ) {
				d = fnr( 10 ) + 12;
				for( h = 0; h < 3; ++h ) {
					goxy( d, 16 + w * 2 + h );
					puts( boxes[ w ][ h ] );
				}
			}
		}
		++w;		/* increment distance */
	}
}

/* check the instruments */

ck_instruments()
{
	if( comp ) {
		goxy( 50, 10 );
		puts( "Compass Reading: " );

		switch( plyr_face ) {
		case 1 :	puts( "WEST " );	break;
		case 2 :	puts( "EAST " );	break;
		case 4 :	puts( "NORTH" );	break;
		case 8 :	puts( "SOUTH" );	break;
		}
	}

	goxy( 50, 15 );
	putchar( shots + '0' );
	puts( " shots remaining" );
}

/* get and process user command */

get_command()
{
	int b;
	char command;

	goxy( 0, 22 );
	puts( "Command (L/R/S/F/O): " );
	puts( CURON );
	while( ! ( command = bdos( 6, 0xFF ))) ;
	puts( CUROFF );

	switch( command ) {

	case '\003' :	exit();

/* open box */

	case 'O' :	for( b = 0; b < 4; ++b ) {
				if( boxx[ b ] == plyr_x
					&& boxy[ b ] == plyr_y ) {

					boxx[ b ] = 99;

					goxy( 40, 22 );
					puts( "Box contains a " );
					switch( b ) {

					case 0 :	puts( "Compass!" );
							comp = 1;
							break;
					case 1 :	puts( "Radio!" );
							radi = 1;
							break;
					case 2 :	puts( "Rubik's Cube" );
							break;
					case 3 :	puts(
	"Laser Pistol recharge unit!" );
							shots += 5;
					}
				}
			}
			break;

	case 'F' :	fire();
			break;

/* turn left */

	case 'L' :	repaint = 1;
			if( plyr_face == 1 ) {
				plyr_face = 8;
				break;
			}
			if( plyr_face == 8 ) {
				plyr_face = 2;
				break;
			}
			if( plyr_face == 2 ) {
				plyr_face = 4;
				break;
			}
			if( plyr_face == 4 ) {
				plyr_face = 1;
				break;
			}
			break;

/* turn right */

	case 'R' :	repaint = 1;
			if( plyr_face == 1 ) {
				plyr_face = 4;
				break;
			}
			if( plyr_face == 4 ) {
				plyr_face = 2;
				break;
			}
			if( plyr_face == 2 ) {
				plyr_face = 8;
				break;
			}
			if( plyr_face == 8 ) {
				plyr_face = 1;
				break;
			}
			break;

/* step forward */

	case 'S' :	if( maze[ plyr_x ][ plyr_y ] & plyr_face ) {

				switch( plyr_face ) {
				case 1 :	--plyr_x;	break;
				case 2 :	++plyr_x;	break;
				case 4 :	--plyr_y;	break;
				case 8 :	++plyr_y;	break;
				}

				if( plyr_x == MX - 1 ) {
					puts( "You made it!! You''ve Won!!!" );
					won = 1;
				} else repaint = 1;

			} else {
				goxy( 40, 22 );
				puts( "CAN'T!" );
			}
			break;
	default:	putchar( '\007' );
	}
}

update_robots()
{
	int r, x, y, rd, ok;

	for( r = 0; r < 4; ++r ) {

		if( rx[ r ] != 99 ) {

			ok = 0;

			while( ! ok ) {
				x = rx[ r ];
				y = ry[ r ];

				switch( fnr( 3 )) {

				case 0 :	if( rx[ r ] < plyr_x ) {
							rd = 2;
							++x;
						} else {
							rd = 1;
							--x;
						}
						break;
				case 1 :	if( ry[ r ] < plyr_y ) {
							rd = 8;
							++y;
						} else {
							rd = 4;
							--y;
						}
						break;
				case 2 :	rd = 15;	/* no move */
				}

				ok = maze[ rx[ r ]][ ry[ r ]] & rd;
			}
		}
	}
}

/* fire laser */

fire()
{
	int wx, wy, r, w, h;

	if( ! shots ) return;

	--shots;

	sound( "1,0,,,,,1,238,7,7" );

	r = 0;
	for( wx = 2; wx < 56; ++wx ) cricket( r, wx );

	sound( ",,,,,,,,," );

	wx = plyr_x;
	wy = plyr_y;

/* look forward along player's view */

	w = 0;

	while(( maze[ wx ][ wy ] & plyr_face ) && ( w < 4 )) {

		switch( plyr_face ) {
		case 1 :	--wx;	break;
		case 2 :	++wx;	break;
		case 4 :	--wy;	break;
		case 8 :	++wy;	break;
		}

		goxy( 19, 18 - ( 2 * w ));
		putchar( '*' );

		for( r = 0; r < 4; ++r ) {

			if( rx[ r ] == wx && ry[ r ] == wy ) {
				goxy( 50, 22 );
				puts( "\007>HIT!<" );

				for( h = 3; h < 12; ++h ) {
					goxy( 19 + ( h / 2 ) - fnr( h ),
						12 + ( h / 2 ) - fnr( h ));
					putchar( ' ' + fnr( 16 ));
				}

				rx[ r ] = 99;
				break;
			}
		}
		++w;
	}
	repaint = 1;
}

/* generate sound effects */

sound( p )
char *p;
{
 	int r, v;
	char c;

	r = 0;
	v = 0;

	while( c = *p++ ) {
		if( c >= '0' && c <= '9' ) {
			v = 10 * v + c - '0';
		} else {
			cricket( r, v );	/* send out value */
			v = 0;
			++r;			/* increment register */
		}
	}
}

/* send value to cricket */

cricket( r, v )
int r, v;
{
	portout( 177, r );
	portout( 176, v );
}

repchr( n, c )
int n;
char c;
{
	while( n-- ) putchar( c );
}

puts( p )
char *p;
{
	while( *p ) putchar( *p++ );
}

int abs( i )
int i;
{
	if( i < 0 ) return 0 - i;
		else return i;
}

#include "portio.c"
#include "stdlib.c"


