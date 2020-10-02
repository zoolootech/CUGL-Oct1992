/*	HEADER:  CUG136.20;
	TITLE:		TZ;
	VERSION:	0.29;
	DATE:		3/12/1986;
	DESCRIPTION:	"Twilight Zone Adventure Game (part 1)";
	KEYWORDS:	game,adventure;
	SYSTEM:		CP/M,MS-DOS,TOS;
	FILENAME:	TZ1.C;
	AUTHORS:	R. Rodman;
	COMPILERS:	C/80,Aztec,Alcyon;
*/

/* TWILIGHT ZONE GAME

	TZ1.C - First half

	From 810717 (original assembler version) to 830125 in Z80 assembler.

	831025 rr begin conversion to C 0.10
	831101 rr dictionary in room file 0.13
	831104 rr add terminal file stuff 0.14
	850412 rr mods for ibm 0.21
	850429 rr for unbuf,use open returns int; for buf,use fopen
			returns FILE (Aztec) 0.24
	850430 rr try allocating buffer for Aztec 0.25
	850521 rr try to fix messed-up message 0.27
	860228 rr fix \n for cp/m 0.28
	860311 rr split into two parts 0.29
			add type to fopen
	860312 rr st stuff

Version number is currently not displayed anywhere

NOTE:	The alloc() function does not work on the Atari ST.  To simplify
	our lives, I don't use it.

The IBM switch is only known to work with the AZTEC compiler.
*/

/* #define IBM */	/* define this for IBM INT 10 screen control */
#define TERMINAL	/* define this for TERMINAL.SYS */

/* #define AZTEC */	/* define this for Aztec C compiler */
/* #define C80 */	/* define this for C/80 */
#define DRI		/* define this for DRI C */
#define ST		/* define this (and DRI?) for Atari ST */  	

#ifdef C80
#define DOS bdos
#endif

#ifdef AZTEC
#include "stdio.h"
#include "fcntl.h"
#define DOS bdos
#endif

#ifdef DRI
#include "stdio.h"
#define DOS __BDOS
#endif

#ifdef ST
#include "osbind.h"
#endif

#define BUFSIZE 30000

int cnt,i;
char *room[ 256 ];			/* rooms */
char *wlk_msg[ 24 ];			/* points to walker messages */
char inp_buf[ 34 ];			/* input buffer */
int inp_cnt = 0;			/* input buffer pointer */

#ifdef TERMINAL

#define BYTE char
#define ESC '\033'

struct {				/* terminal file structure */	
	BYTE width, height, defint, uselst, yfirst, method, addx, addy;
	char inistr[ 8 ], clrscn[ 8 ], clreos[ 8 ], clreol[ 8 ];
	char curoff[ 8 ], curon[ 8 ], curbeg[ 8 ], curmid[ 8 ];
	char curend[ 8 ], hiint[ 8 ], loint[ 8 ], revvid[ 8 ];
	char norvid[ 8 ], inslin[ 8 ], dellin[ 8 ];
} trmbuf = {
#ifdef ST
/* Values for ST or Heath terminals */

	80, 24, 'H', 'Y', 'Y', '0', 32, 32,
	0, 0, 0, 0, 0, 0, 0, 0,		/* inistr */
	ESC, 'E', 0, 0, 0, 0, 0, 0,	/* clrscn */
	ESC, 'J', 0, 0, 0, 0, 0, 0,	/* clreos */
	ESC, 'K', 0, 0, 0, 0, 0, 0,	/* clreol */
	ESC, 'x', '5', 0, 0, 0, 0, 0,	/* curoff */
	ESC, 'y', '5', 0, 0, 0, 0, 0,	/* curon */
	ESC, 'Y', 0, 0, 0, 0, 0, 0,	/* curbeg */
	0, 0, 0, 0, 0, 0, 0, 0,		/* curmid */
	0, 0, 0, 0, 0, 0, 0, 0,		/* curend */
	0, 0, 0, 0, 0, 0, 0, 0,		/* hiint */
	0, 0, 0, 0, 0, 0, 0, 0,		/* loint */
	ESC, 'p', 0, 0, 0, 0, 0, 0,	/* revvid */
	ESC, 'q', 0, 0, 0, 0, 0, 0,	/* norvid */
	ESC, 'L', 0, 0, 0, 0, 0, 0,	/* inslin */
	ESC, 'M', 0, 0, 0, 0, 0, 0	/* dellin */
#endif

#ifndef ST
/* Values for Televideoid terminals */

	80, 24, 'H', 'Y', 'Y', '0', 32, 32,
	0, 0, 0, 0, 0, 0, 0, 0,		/* inistr */
	'\032', 0, 0, 0, 0, 0, 0, 0,	/* clrscn */
	ESC, 'Y', 0, 0, 0, 0, 0, 0,	/* clreos */
	ESC, 'T', 0, 0, 0, 0, 0, 0,	/* clreol */
	ESC, '.', '0', 0, 0, 0, 0, 0,	/* curoff */
	ESC, '.', '2', 0, 0, 0, 0, 0,	/* curon */
	ESC, '=', 0, 0, 0, 0, 0, 0,	/* curbeg */
	0, 0, 0, 0, 0, 0, 0, 0,		/* curmid */
	0, 0, 0, 0, 0, 0, 0, 0,		/* curend */
	ESC, '(', 0, 0, 0, 0, 0, 0,	/* hiint */
	ESC, ')', 0, 0, 0, 0, 0, 0,	/* loint */
	ESC, 'G', '4', 0, 0, 0, 0, 0,	/* revvid */
	ESC, 'G', '0', 0, 0, 0, 0, 0,	/* norvid */
	ESC, 'R', 0, 0, 0, 0, 0, 0,	/* inslin */
	ESC, 'E', 0, 0, 0, 0, 0, 0	/* dellin */
#endif
};

#endif

#ifdef IBM
struct {
	int width, height;
} trmbuf = {
	80, 25
};
#endif

/* Dictionary: words should be strung together,separated by spaces.
	Only the unique part should be included.Contained in first line
	of TZ.R.Every word including the last one must be followed by spaces.
	Incidentally,all lines of TZ.R should be terminated only by
	line feeds.*/

char *dic_ptr;

/* Controls used in messages are:
	*	clear screen
	^	position cursor to message line (causes CEOL)
	\	carriage return/line feed (causes CEOL)
	|	position cursor to user input line (HEIGHT-2)
	_	include answer here (e.g._4)
*/

/* The following pieces of information must be saved */

int rm_num = 0;			/* room number */
int seed;			/* random seed */
int wtchct = 0;			/* tv watch count */
int beerct = 0;			/* drink beer count */
int cratct = 0;			/* open crate count */
int pushct = 0;			/* push button count */
int pushfl = 0;			/* push button error flag */
int answer[ 5 ] = { 0,1,2,3,4 };	/* digits of answer */

struct walker {
	int inrm;		/* in room flag */
	int rsel;		/* room pointer */
	int dsel;		/* description selector */
	int locn[ 8 ];		/* rooms walker appears at */
};

/* Woman walker descriptor */

struct walker woman = {
	0,0,0,
	4,52,64,95,109,185,204,248 };

/* Man walker descriptor */

struct walker man = {
	0,0,0,
	3,49,97,105,111,144,166,175 };

/* Rod walker descriptor */

struct walker rod = {
	0,0,0,
	1,27,61,120,165,196,180,254 };

/* The following are not saved */

int nu_fl = 1;			/* new room flag */
int curcol = 0;			/* cursor column */

int noun, verb;			/* word numbers */

char databuf[ BUFSIZE ];	/* statically allocated buffer */

extern hello();
extern exbed(), pool(), echh(), busy(), call7(), call11(), sign(), book();
extern opsafe(), dlcomb(), paper(), form(), blot(), seeslf(), seecow();
extern mirhse(), menu(), drwatr(), excrat(), opcrat(), writing();
extern inst(), buyber(), drbeer(), ridhor(), taktrn(), taknot(), plates();
extern opgrat(), examtv(), wtchtv(), exhand(), push();

char xgetc();

/* Main Program */

main()
{
	int junk;

	term_init();		/* init terminal */

	hello();

/* initialize data */

	init_data();

	xputs( "\n\n?Dimensions out of range at address 1423" );

	junk = 0;
	seed = 719;

	while( junk == 0 ) {
		junk = xgetc();
		seed = seed * 13;
	}

/* synthesize solution from random number into answer[ 0..4 ] */

	if( seed < 0 ) seed = 0 - seed;		/* get abs value */ 

	answer[ 0 ] = seed % 5;
	answer[ 1 ] = ( seed / 5 ) % 5;
	answer[ 2 ] = ( seed / 25 ) % 5;
	answer[ 3 ] = ( seed / 125 ) % 5;
	answer[ 4 ] = ( seed / 625 ) % 5;

	while( 1 ) {
		game();		/* process game */
		input();	/* get input */
	}
}

#ifdef TERMINAL

/* --------TERMINAL FILE LOGIC-------- */

term_init()
{
	int t;

#ifdef C80
	t = fopen( "TERMINAL.SYS","r" );
#endif

#ifdef AZTEC
	t = open( "TERMINAL.SYS", O_RDONLY );
#endif

#ifdef DRI
	t = open( "TERMINAL.SYS", 0 );
#endif

	if( t == 0 ) return;	/* default to televideoid */

	read( t, &trmbuf, 128 );

#ifdef C80
	fclose( t );
#endif

#ifdef AZTEC
	close( t );
#endif

#ifdef DRI
	close( t );
#endif 	

}

/* output a coordinate */

coord( c )
int c;
{
	switch( trmbuf.method ) {
	case 0 :
	case '0' :
	case 'B' :			/* binary positioning */
		xputc( c ); break;
	case 1 :
	case '1' :
	case 'A' :			/* ascii positioning */
		if( c >= 100 ) {
			xputc( '0' + c / 100 );
			c %= 100;
		}
		if( c >= 10 ) {
			xputc( '0' + c / 10 );
			c %= 10;
		}
		xputc( '0' + c );
	}
}

/* position cursor */

goxy( x, y )
int x, y;
{
	xputs( trmbuf.curbeg );
	if( trmbuf.yfirst == 'Y' ) coord( y + trmbuf.addy );
		else coord( x + trmbuf.addx );
	xputs( trmbuf.curmid );
	if( trmbuf.yfirst == 'Y' ) coord( x + trmbuf.addx );
		else coord( y + trmbuf.addy );
	xputs( trmbuf.curend );
}

cls()
{
	xputs( trmbuf.clrscn );
}

ceol()
{
	xputs( trmbuf.clreol );
}

high()
{
	xputs( trmbuf.hiint );
}

low()
{
	xputs( trmbuf.loint );
}

reverse()
{
	xputs( trmbuf.revvid );
}

normal()
{
	xputs( trmbuf.norvid );
}

#endif		/* --------END OF TERMINAL FILE LOGIC-------- */

#ifdef IBM

/* --------IBM SCREEN LOGIC-------- */

term_init()
{
}

cls()
{

#asm
	mov	ah,0	;subfunction - set display mode
	mov	al,2	;set mode to 80x25 bw
	int	10H
#endasm

}

goxy( x,y )
int x,y;
{

#asm
	mov	ax,word ptr 8[bp]	;get first parameter - x
	mov	dl,al		;put in dl

	mov	ax,word ptr 10[bp]	;get second parameter - y
	mov	dh,al		;put in dh

	mov	ah,2
	mov	al,0
	mov	bh,0
	int	10H
#endasm

}

ceol()
{
	int i;
	for( i = 0; i < ( trmbuf.width - 1 ); ++i ) xputc( ' ' );
	xputc( '\r' );		/* return to left position */
}

high()
{
}

low()
{
}

reverse()
{
}

normal()
{
}

#endif		/* --------END OF IBM SCREEN LOGIC-------- */

/* init room list and other data */

init_data()
{
	int rc;
	char *p;

#ifdef C80	
	rc = fopen( "TZ.R","r" );
	if( rc == 0 ) {
		xputs( "File not found." );
		exit( 1 );
	}
#endif

#ifdef AZTEC 	
	rc = open( "TZ.R", O_RDONLY );
	if( rc == -1 ) {
		xputs( "File not found." );
		exit( 1 );
	}
#endif

#ifdef DRI
	rc = openb( "TZ.R", 0 );
	if( rc == -1 ) {
		xputs( "File not found." );
		exit( 1 );
	}
#endif

	p = &databuf[ 0 ];		/* buffer for room data */

/* read entire file into buffer */

	cnt = read( rc, p, BUFSIZE );

#ifdef C80
	fclose( rc );
#endif

#ifdef AZTEC
	close( rc );
#endif

#ifdef DRI
	close( rc );
#endif

/* dictionary is first line of file */

	dic_ptr = p;

/* now look up indices to each string */

	for( i = 0; i < 256; i++ ) {
		while( *p != '\012' ) p++;	/* find newline */
		*p++ = '\0';			/* change to end of string */
		room[ i ] = p;			/* assign pointer */
	}
	for( i = 0; i < 24; i++ ) {
		while( *p != '\012' ) p++;	/* find newline */
		*p++ = '\0';			/* change to end of string */
		wlk_msg[ i ] = p;		/* assign pointer */
	}
}

/* Print Message */

print( m )
char *m;
{
	char c;
	char *k;
	int j;

	while( *m ) {
		c = *m++;

		switch( c ) {

/* Special handling for Space.
	See if next word will fit on this line.*/

			case ' ' : j = curcol;
				k = m;		/* temp pointer */

				while( j < ( trmbuf.width - 1 ) &&
					*k != ' '
					&& *k != '\\'
					&& *k != '.'
					&& *k != ','
					&& *k != '\0' ) {
					k++;
					j++;
				}

				if( j < ( trmbuf.width - 1 )) {
					xputc( ' ' );
					curcol++;
				} else {
					xputc( '\n' );
					ceol();
					curcol = 0;
				}

				break;

			case '\\' :		/* go to next line */
				xputc( '\n' );
				ceol();
				curcol = 0;
				break;

			case '*' :		/* clear screen */
				cls();
				high();	
				curcol = 0;
				break;

			case '^' :		/* user line */
				goxy( 0, trmbuf.height / 2 - 1 );

				for( j = 1; j <= trmbuf.height / 2; j++ ) {
					ceol();
					xputc( '\n' );
				}

				goxy( 0, trmbuf.height / 2 - 1 );
				curcol = 0;
				break;

			case '~' :		/* input line */
				goxy( 0, trmbuf.height - 1 );

				low();
				reverse();

/* clear by brute force to make line look nice */

				for( j = 1; j < trmbuf.width - 2; j++ )
					xputc( ' ' );
				normal();

				goxy( 1, trmbuf.height - 1 );
				low();
				reverse();

				curcol = 0;	
				break;

/* answer clues are imbedded in text as _1 to _5 */

			case '_' :		/* insert color */
				c = *m++;	/* get next char */
				switch( answer[ ( c & 7 ) - 1 ] ) {
					case 0:	xputs( "red" );
						curcol += 3;
						break;  
					case 1: xputs( "orange" );
						curcol += 6;
						break;
					case 2: xputs( "yellow" );
						curcol += 6;
						break;
					case 3: xputs( "green" );
						curcol += 5;
						break;
					case 4: xputs( "blue" );
						curcol += 4;
						break;
				}
				break;

			case '.' :
			case ',' :
				xputc( c );	/* print period */
				xputc( ' ' );
				if( c == '.' ) xputc( ' ' );	/* spaces */
				curcol += 3;
				break;

			default :
				xputc( c );
				curcol++;
		}
	}

	normal();
	high();
}

/* Main Game Process */

game()
{
	char *c;

	if( ! nu_fl ) return;	/* return if haven't moved into a new room */

	nu_fl = 0;			/* reset new room flag */

	c = room[ rm_num ];		/* get initial room pointer */
	while( *c != '*' ) c++;		/* point to first asterisk */

	print( c );			/* print description */

/* just arrived in a new room.clear all in-room flags */

	woman.inrm = 0;
	man.inrm = 0;
	rod.inrm = 0;

/* check to see if walkers are in your room */

	if( woman.locn[ woman.rsel ] == rm_num ) {
		print( wlk_msg[ woman.dsel ] );
		woman.rsel = ++woman.rsel % 8;
		if( woman.dsel < 7 ) woman.dsel++;
		woman.inrm = 1;
	}

	if( man.locn[ man.rsel ] == rm_num ) {
		print( wlk_msg[ 8 + man.dsel ] );
		man.rsel = ++man.rsel % 8;
		if( man.dsel < 7 ) man.dsel++;
		man.inrm = 1;
	}

	if( rod.locn[ rod.rsel ] == rm_num ) {
		print( wlk_msg[ 16 + rod.dsel ] );
		rod.rsel = ++rod.rsel % 8;
		if( rod.dsel < 7 ) rod.dsel++;
		rod.inrm = 1;
	}

	print( "~What do you do?" );
}

/* Input Process */

input()
{
	char c;

	c = xgetc();

	if( c == 0 ) return;		/* quit if no input */

	goxy( inp_cnt + 22, trmbuf.height - 1 );

	switch( c ) {
		case '\r' :
			parse();		/* get command */
			print( "~What do you do?" ); /* re-prompt */
			inp_cnt = 0;
			break;
		case '\b' :
			if( ! inp_cnt ) break;	/* backspace */
			inp_cnt--;
			xputs( "\b \b" );
			break;

		default :
			if( c < ' ' || inp_cnt > 30 ) break;
			if( c >= 'A' && c <= 'Z' )
				c += 32;	/* convert lower */
			xputc( c );		/* duplex */
			inp_buf[ inp_cnt++ ] = c;
	}
}

/* Get word */

get_word( p )
char *p;
{
	int k,match;
	char *q,*d;
 
	if( ! *p ) return( 0 );

	k = 0;					/* word counter */
	d = dic_ptr;				/* index into dict */

	while( *d ) {
		q = p;				/* point to begin of word */
		match = 1;			/* pretend we have a match */

		k++;				/* count a word */ 

		while( *d != ' ' ) {		/* compare this word */
			if( *q++ != *d++ ) match = 0;
		}				/* d points to space */

		if( match ) return( k );	/* got a match */
		d++;				/* skip the space */
	}

	return( 0 );	/* came to end of dictionary */
}

/* Parse Command */

parse()
{
	char *p_ptr;

	if( ! inp_cnt ) return;			/* empty line input */

	inp_buf[ ++inp_cnt ] = ' ';		/* add space to input */
	inp_buf[ ++inp_cnt ] = '\0';		/* terminate it */

	noun = 0;
	verb = 0;

	p_ptr = &inp_buf[ 0 ];			/* point to first word */

	verb = get_word( p_ptr );		/* get verb */

	while( *p_ptr != ' ' ) p_ptr++;		/* find blank */
	while( *p_ptr == ' ' ) p_ptr++;		/* find non-blank */

	noun = get_word( p_ptr );		/* get noun if any */

	switch( verb ) {
		case 7 : go( noun ); break;

		case 8 : if( noun == 29 && rm_num == 63 ) taktrn();
				else print( "^You can't take it with you." );
			break;		

		case 9 : exit();	/* quit game */
 
		case 10 : if( noun == 11 ) {
				switch( rm_num ) {
					case 7 : call11(); break;
					case 11 : call7(); break;
					case 174 :
					case 222 :
					case 249 : busy(); break;
					default : cant();
				}
			}
			else cant();
			break;
		case 13 : switch( noun ) {
			case 14 : menu( rm_num ); break;

			case 15 : if( rm_num == 7 || rm_num == 174
					|| rm_num == 222 || rm_num == 249 )
					sign();
					else cant();
				break;

			case 16 : if( rm_num == 7 || rm_num == 174
					|| rm_num == 222 || rm_num == 249 )
					book();
					else cant();
				break;

			case 17 : if( rm_num == 100 ) form();
				else cant();
				break;
			case 24 : writing( rm_num ); break;

			case 25 : if( rm_num == 29 ) inst();
				else cant();
				break;

			case 39 : plates( rm_num ); break;

			case 42 : paper( rm_num ); break;

			default : cant(); break;
			}
			break;

		case 20 : if( noun == 21 && rm_num == 13 ) buyber();
			else cant();
			break;

		case 22 : switch( noun ) {
				case 18 : if( rm_num == 100 ) blot();
					else cant();
					break;
				case 41 : if( rm_num == 21 ) excrat();
					else cant();
					break;
				case 31 : if( rm_num == 163 ) examtv();
					else cant();
					break;
				case 23 : if( rm_num == 198 ) exhand();
					else cant();
					break;
				case 54 :
				case 49 : if( rm_num == 58 ) pool();
					else cant();
					break;
				case 57 : if( rm_num == 38 ||
						rm_num == 66 ||
						rm_num == 178 ) {
						exbed(); break;
					}
				default : print( "^Nothing unusual." );
					break;
			}
			break;

		case 30 : if( noun == 31 && rm_num == 163 ) wtchtv();
				else cant();
				break;
		case 37 : if( rm_num == 128 ) push();
				else cant();
				break;
		case 38 : switch( noun ) {		/* LOOK */
				case 12 : if( rm_num == 0 || rm_num == 66
						|| rm_num == 106
						|| rm_num == 178
						|| rm_num == 255 ) {
						seeslf();
						break;
					}
					if( rm_num == 38 ) {
						seecow();
						break;
					}
					if( rm_num >= 223 && rm_num <= 231 ) {
						mirhse( rm_num );
						break;
					}
					cant();
					break;
				case 54 :
				case 49 : if( rm_num == 58 ) pool();
					else cant();
					break;
				default : nu_fl = 1; break;
			}
			break;		

		case 40 : switch( noun ) {
				case 41 : if( rm_num == 21 ) opcrat();
					else cant(); break;
				case 43 : if( rm_num == 152 ) opsafe();
					else cant(); break;
				case 52 : if( rm_num == 220 ) opgrat();
					else cant(); break;
				default : print( "^There's nothing to open." );
					break;
			}
			break;

		case 44 : if( noun == 45 && rm_num == 152 ) dlcomb();
			else cant(); break;
		case 46 : savgam(); break;
		case 47 : lodgam(); break;

		case 48 : switch( noun ) {
				case 21 : if( rm_num == 13 ) drbeer();
					else cant();
					break;
				case 49 : if( rm_num == 9 || rm_num == 221 ) {
						drwatr();
						break;
					}
					if( rm_num == 58 ) echh();
					else cant();
					break;
				case 54 : if( rm_num == 58 ) echh();
					else cant();
					break;
				default : cant(); break;
			}
			break;

		case 50 : if( noun == 51 && rm_num == 14 ) ridhor();
				else cant();
			break;
		case 55 : say();		/* speak to walkers */
			break;	

/* synonyms for go */

		case 64 : go( 1 ); break;
		case 65 : go( 2 ); break;
		case 66 : go( 3 ); break;
		case 67 : go( 4 ); break;
		case 68 : go( 5 ); break; 
		case 69 : go( 6 ); break;
		case 70 : go( 27 ); break;
		case 71 : go( 28 ); break;
		case 26 : go( 26 ); break;	/* sleep */
 
		case 72 : vahr(); break;	/* Vahr */	

		default : cant();
	}
}

vahr()
{
	char *p;

	p = &inp_buf[ 0 ];		/* point to begin of input buf */

	while( *p != ' ' ) ++p;		/* find end of first word */

	while( *p == ' ' ) ++p;		/* find begin of next word */
 
	if( *p++ != '#' ) cant();
		else {
			rm_num = convert( p );
			nu_fl = 1;
	}
}
		
savgam()
{
	int i;

#ifdef C80
	int fc;
	fc = fopen( "TZ.SAV", "wb" );
#endif

#ifdef AZTEC	
	FILE *fc;
	fc = fopen( "TZ.SAV","w" );
#endif

#ifdef DRI	
	FILE *fc, *fopen();
	fc = fopen( "TZ.SAV","w" );
#endif

	if( fc == 0 ) {
		print( "^Can't create output file." );
		return;
	}

	putc( rm_num,fc );	/* write room number */
	putc( seed % 256,fc );
	putc( seed / 256,fc );	/* random seed */
	putc( wtchct,fc );	/* tv watch count */
	putc( beerct,fc );	/* drink beer count */
	putc( cratct,fc );	/* open crate count */
	putc( pushct,fc );	/* push button count */
	putc( pushfl,fc );	/* push button error flag */

	for( i = 0; i < 5; i++ ) putc( answer[ i ],fc );
				/* digits of answer */

	putc( woman.inrm,fc );
	putc( woman.rsel,fc );
	putc( woman.dsel,fc );

	putc( man.inrm,fc );
	putc( man.rsel,fc );
	putc( man.dsel,fc );

	putc( rod.inrm,fc );
	putc( rod.rsel,fc );
	putc( rod.dsel,fc );

	fclose( fc );

	print( "^Game has been saved." );
}

lodgam()
{
	int i;

#ifdef C80
	int fc;
	fc = fopen( "TZ.SAV", "rb" );
#endif

#ifdef AZTEC	
	FILE *fc;
	fc = fopen( "TZ.SAV", "r" );
#endif

#ifdef DRI	
	FILE *fc, *fopen();
	fc = fopen( "TZ.SAV", "r" );
#endif

	if( fc == 0 ) {
		print( "^Can't open input file." );
		return;
	}

	rm_num = getc( fc );		/* write room number */
	seed = getc( fc );
	seed += 256 * getc( fc );	/* random seed */
	wtchct = getc( fc );		/* tv watch count */
	beerct = getc( fc );		/* drink beer count */
	cratct = getc( fc );		/* open crate count */
	pushct = getc( fc );		/* push button count */
	pushfl = getc( fc );		/* push button error flag */

	for( i = 0; i < 5; i++ ) answer[ i ] = getc( fc );
				/* digits of answer */

	woman.inrm = getc( fc );
	woman.rsel = getc( fc );
	woman.dsel = getc( fc );

	man.inrm = getc( fc );
	man.rsel = getc( fc );
	man.dsel = getc( fc );

	rod.inrm = getc( fc );
	rod.rsel = getc( fc );
	rod.dsel = getc( fc );

	fclose( fc );

	nu_fl = 1;			/* flag new room */
}

/* Go Routine */

go( n )
int n;
{
	char dirc,*c;

	switch( n ) {
		case 1 : dirc = 'n'; break;	/* get direction character */
		case 2 : dirc = 's'; break;
		case 3 : dirc = 'e'; break;
		case 4 : dirc = 'w'; break;
		case 5 : dirc = 'u'; break;
		case 6 : dirc = 'd'; break;
		case 27 : dirc = 'i'; break;
		case 28 : dirc = 'o'; break;
		case 19 : dirc = 'b'; break;	/* below */
		case 26 : dirc = 'z'; break;	/* sleep */
		default : dirc = '?'; break;
	}

	c = room[ rm_num ];		/* get pointer to room */

	while( *c != '*' ) {
		if( *c++ == dirc ) {
			rm_num = convert( c );
			nu_fl = 1;	/* inform game() */
			return;
		}
	}

/* fell through,direc must be bad */

	print( "^You can't go that way." );
}

int convert( c )
char *c;
{
	int r;

	r = 0;

	while( *c >= '0' && *c <= '9' )
		r = 10 * r + *c++ - '0';

	return( r );
}

/* Simply Cannot Do Something */

cant()
{
	print( "^You cannot do that here." );
}

/* Speak to the Walkers */

say()
{
	if( woman.inrm ) {
		switch( noun ) {
			case 56 :
	print( "^'Hi,' she smiles back.'Have you found anything?'" );
				break;
			case 45 : print( "*Aileen has shot you." );
				exit( 0 );
			default :
	print( "^The woman gives you a puzzled look." );
	print( " 'What are you trying to say?'" );
		}
		return;
	}

	if( man.inrm ) {
print( "^You'd like to talk to him,sure,but he's already gone." );
		return;
	}

	if( rod.inrm ) {
print( "^The man in the dark suit pays no attention to anything you say." );
		return;
	}

	print( "^There is no one here to speak to." );
}

/* raw output of string */

xputs( c )
char *c;
{
	while( *c ) xputc( *c++ );
}

/* sample keyboard, return 0 if no char avail */

char xgetc()
{
#ifdef ST
	if( Bconstat( 2 ) != 0 ) return( Bconin( 2 ) & 0xFF );
		else return 0;
#endif

#ifndef ST
	return( DOS( 6, 0xFF ));
#endif
}

/* Raw output of a character */

xputc( c )
char c;
{
	if( c == '\n' ) xputc( '\r' );
#ifdef ST
	Bconout( 2, c );
#endif

#ifndef ST
	DOS( 6, c );
#endif
}

#ifdef C80
#include "stdlib.c"
#endif

/* end of tz1.c */


