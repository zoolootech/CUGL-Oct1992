/*
HEADER:		;
TITLE:		Diskette sector editor;
VERSION:	1.4;

DESCRIPTION:	"Lets you view, edit and write back a diskette sector.

		It needs a 24*80 cursor-addressable terminal, and requires
		your custom CLRSCREEN() and GOTOXY() functions.  The
		definitions of these functions appear early in SE.C.

		Compiling requires SENTER.C, included on the diskette.
		Also included are SE.DOC and SE.SUB.";

KEYWORDS:	Disk, utility, editor, sector;
SYSTEM:		CP/M-80, V2.2;
FILENAME:	SE.C;
WARNINGS:	"Requires Version 2.x of CP/M.
		SE is not designed to work with systems using the DEBLOCK
		routine in the BIOS to handle physical sectors larger than
		128 bytes, so if you have such a system, and want to use SE,
		you must modify the routines in SE which write back the edited
		sector.  The BitMap command is not implemented.";

SEE-ALSO:	SE.DOC, SENTER.C;
AUTHORS:	Jan Larsson;
COMPILERS:	BDS C, v1.43 and 1.5a;
*/
/************************************************************************

Version 1.0:
	SE uses 2.x functions which are not available on CP/M 1.x,
	so it will need patching for running under 1.x.  Functions
	for handling XY and CLEAR screen are at the beginning of SE.C.
	Jan Larsson, Kosterv. 12, S-181 35  Lidingo, SWEDEN

************************************************************************/

#include <bdscio.h>

#define YOFF 6
#define XOFF 7
#define AYOFF 6
#define AXOFF 60
#define LEFT 0x13
#define RIGHT 0x04
#define UP 0x05
#define DOWN 0x18
#define ESC 0x1B


struct dp {
	unsigned spt ;
	char blocks ;
	char blockm ;
	char nullm ;
	unsigned dsiz ;
	unsigned dirsiz ;
	char all0 ;
	char all1 ;
	unsigned checksiz ;
	unsigned tracko ;
	};

struct dp *pdp ;

unsigned maxtrack, maxsec, spb, ctrack, csec, cblock, trans, *ptrans ;
char cbuf[128], line[128], tbuf[80] ;
char a,b,c ;
unsigned x,y,z ;

	

/*
**  Send a character to the video terminal
*/

pchar( c )
char c ;
{
	bios( 4, c );
}


/*
**  Get a char from keyboard
*/

gchar()
{
	return( toupper( bios(3) ) );
}


/*
**  Clear the screen
*/

clrscreen()
{
	pchar( 26 );
}


/*
**  Cursor adressing routine, y = vertical 0...n, x = horizontal 0...n
*/

gotoxy( x, y )
char x, y ;
{
	pchar( ESC );
	pchar( '=' );
	pchar( y + 32 );
	pchar( x + 32 );
}


/*
**  Handle cursor positioning on a HEX map of memory
**  on the screen, 128 bytes as 8 rows of 16 bytes.
**  One space between bytes on screen
*/

hxy( pos )
unsigned pos;
{
	char x, y ;

	y = pos / 32 + YOFF ;
	x = pos % 32 ;
	x += x/2 + XOFF ;
	gotoxy( x, y );
}
              

/*
**  Cursor positioning on the ASCII display, 8 rows of 16 chars
*/

axy( pos )
unsigned pos ;
{
	char x, y ;

	pos /= 2 ;
	y = pos / 16 + AYOFF ;
	x = pos % 16 + AXOFF ;
	gotoxy( x, y );
}


/*
**  The real workhorse editing routine, handles (almost) everything.
*/

unsigned hget( pos, buf )
unsigned pos ;       
char *buf ;
{            
	char c, d ;

	d = c = gchar();
	if(c == 3)stop();
	if(c >= '0' && c <= '9')c -= '0' ;
	else if(c >= 'A' && c <= 'F')c = (c - 'A') + 10 ;
	else {
		switch( c ) {
			case RIGHT  : if(pos < 255)pos++ ; hxy( pos ); return( pos );
			case LEFT   : if(pos >   0)pos-- ; hxy( pos ); return( pos );
			case UP     : if(pos >  31)pos -= 32 ; hxy( pos ); return( pos );
			case DOWN   : if(pos < 224)pos += 32 ; hxy( pos ); return( pos );
			case ESC    : hxy( pos ); return( ERROR );
			case 0x01   : pos -= (pos % 32) ; hxy( pos ); return( pos );
			case 0x06   : pos += (32 - (pos % 32)) - 1 ; hxy( pos ); return( pos );
			case 0x1a   : pos = 0 ; hxy( pos ); return( pos );
			default     : ermsg("Illegal HEX digit.");
		                  enter( 0, 18, " Command: ", 48, line, 0x62 );
 						  hxy( pos ); return( pos );
    		}
		}
	if(pos % 2 == 0)buf[ pos/2 ] = (buf[ pos/2 ] & 0x0f) | (c << 4) ;
	else buf[ pos/2 ] = (buf[ pos/2 ] & 0xf0) | c ;
	hxy( pos ); pchar( d );
	axy( pos );
	if(buf[ pos/2 ] >= ' ' && buf[ pos/2 ] <= 0x7e)pchar( buf[ pos/2 ] ) ;
	else pchar('.');
    if(pos < 255)pos++ ;
	hxy( pos );		
	return( pos );
}	


/*
**  Display a 128 byte buffer on screen
*/

display( buf )
char *buf ;
{
	unsigned ppos ;
	char c, i ;

	gotoxy( 0, YOFF );
	ppos = 0 ;
	while(ppos < 255){
		printf("%04x:  ", ppos/2 );
		hxy( ppos );
		for(i = 0 ; i < 16 ; i++ )printf("%02x ", buf[ ppos/2 + i ] );
		axy( ppos );
		for(i = 0 ; i < 16 ; i++ ){
			c = buf[ ppos/2 + i ] ;
			if(c >= ' ' && c <= 0x7e)pchar( c );
			else pchar('.');
			}
		ppos += 32 ;
		printf("\n");
		}
	ppos = 0 ;
	hxy( ppos );
}


/*
**  Stop immediately, and return to DOS
*/

stop()
{
	gotoxy(0,22);
	exit();
}


/*
**  Display ERROR message on screen
*/

ermsg( s )
char *s ;
{
	gotoxy( 10, 18 );
	printf("ERR: %s ", s );
	gchar();
}
                         


/*
**  CHECKPAR checks the track sector parameters
*/

checkpar( track, sector )
unsigned track, sector ;
{
	if(track > maxtrack){ ermsg("Too large track number.");
						  return( FALSE );
					    }
	if(sector > maxsec){ ermsg("Too large sector number");
						  return( FALSE );
				        }
	if(sector == 0){ ermsg("Sector number cant be 0");
					 return( FALSE );
					}
	return(TRUE);
}



/*
**  PSEC stores a sector, using actual skewing factor
*/

psec( track, sector, buffer )
unsigned track, sector ;
char *buffer ;
{
	if(track >= pdp -> tracko)sector = sectran( (sector - 1) );
	bios( 10, track );
	bios( 11, sector );
	bios( 12, buffer );
	bios( 14 );
}




/*
**  GSEC fetches a sector, using actual skewing factor
*/

gsec( track, sector, buffer )
unsigned track, sector ;
char *buffer ;
{
	if(track >= pdp -> tracko)sector = sectran( (sector - 1) );
	bios( 10, track );
	bios( 11, sector );
	bios( 12, buffer );
	bios( 13 );
}




/*
**  Header displays the header at the top of the page
*/

header()
{
	printf("Ayatollans Mjukisar");
        gotoxy( 26,0 );
        printf("S E C T O R  E D I T O R");
	gotoxy( 68, 0 );
	printf("Version 1.4");          
        gotoxy( 0, 1 );
        printf("----------------------------------------");
        printf("----------------------------------------");
}



/*
**  Display the commands at the lowest line of the
**  the screen.
*/

docs()
{
	gotoxy( 0, 21 );
	printf(" NewDisk/Look/Edit/Write/Bye/BitMap/Drive");
}


/*
**  copy the n-th item of command line into spec mini-buf
*/

ctok( buf, line, n )
char *buf, *line ;
char n ;
{
	char *p1, *p2, i, ccc, bool ;

	bool = FALSE ;
	*buf = NULL ;
	p1 = line ;
	while(*p1 == ' ' && *p1 != NULL)p1++ ;
	i = 1 ;
	while(i < n && *p1 != NULL){
		while(*p1 != ' ' && *p1 != NULL)p1++ ;
		while(*p1 == ' ' && *p1 != NULL)p1++ ;
		i++ ;
		}
	if(*p1 != NULL){
		p2 = p1 ;
		while(*p2 != ' ' && *p2 != NULL)p2++ ;
		ccc = *p2 ;
		*p2 = NULL ;
		strcpy( buf, p1 );
		*p2 = ccc ;
		}
}

	

/*
**  Display the maximum numbers
*/

dispmax()
{
	gotoxy( 0, 3 );
	printf("CP/M Max block:%4d   Disk Size:%5d kbyte", 
                pdp -> dsiz,
                (pdp -> dsiz + 1) * (spb / 8) );
	printf("   Max Track:%3d    Max Sector:%3d", maxtrack, maxsec );													
}



/*
** After getting new track/sector adresses, calc and display new
** block numbers etc.
*/


calc()
{	
	gotoxy( 0, 16 );
	cblock = ((ctrack - pdp -> tracko) * maxsec + csec) / spb ;
	printf("CP/M Block number: %4d/%x     ", cblock, cblock );
	printf("Track number: %3d      ", ctrack );
	printf("Sector number: %3d    ", csec );
}


/*
**  SELDSK call bios routine for seldsk, return whatever
**  BIOS returns in HL
*/

seldsk( drive )
char drive ;
{
	unsigned *p, d ;

	p = 1 ;
	d = *p ;
	d = d + 24 ;
	return( call( d, 0, 0, drive, 0 ) );
}



/*
**  SECTRAN calls the sectran routine in users BIOS
*/

sectran( secno )
char secno ;
{
	unsigned p ;

	ptrans = 1 ;
	p = *ptrans ;
	p += 0x2d ;            
	return( call( p, 0, 0, secno, trans ) );
}


bitmap(){;;}


/*
**  Initialize all those disk parameters
*/

idisk()
{
	trans = bdos( 25 );
	ptrans = seldsk( trans );
	trans = *ptrans ;
	pdp = bdos( 31 );
	spb = 1 << pdp -> blocks ;
	maxsec = pdp -> spt ;
	maxtrack = pdp -> tracko + ((pdp -> dsiz * spb) / maxsec) ;
	ctrack = 0 ;
	csec = 1 ;
	cblock = ((ctrack - pdp -> tracko) * maxsec + csec) / spb ;
	gsec( ctrack, csec, cbuf );
}


main()
{
	char *p ;
	unsigned g ;


	clrscreen();
	header();
	docs();
	idisk();
	calc();
	dispmax();
	gsec( ctrack, csec, cbuf );
	display( cbuf );
	for(;;){
		enter( 0, 18, " Command: ", 48, line, 0x76 );
	    ctok( tbuf, line, 1 );
		a = tbuf[0] ;
		ctok( tbuf, line, 2 );
		b = tbuf[0] ;
		ctok( tbuf, line, 3 );
		x = atoi( tbuf );
		ctok( tbuf, line, 4 );
		c = tbuf[0] ;
		ctok( tbuf, line, 5 );
		y = atoi( tbuf );
		ctok( tbuf, line, 1 );
		switch (a) {
			case 'N' : idisk(); calc(); dispmax(); break ;
			case 'B' : if(tbuf[1] == 'Y')stop();
					   else bitmap();
					   break ;
			case 'D' : if(seldsk( b - 'A' ) == 0)ermsg("NO such drive known to me");
					   else { bdos( 14, b - 'A' ); idisk(); calc(); dispmax();};
                       break ;
			case 'L' : if(b == 'T'){ ctrack = x ;
								     if(c == 'S')csec = y ;
									 else { ermsg("Illegal second parameter");
									        break;
										  }
					               }
				       else if(b == 'S'){ csec = x ;
						             if(c == 'T')ctrack = y ;
									 else { ermsg("Illegal second parameter");
								            break ;
										  }
								     }
					   else if(b == '+'){
									csec++ ;
									if(csec > maxsec){csec = 1 ; ctrack++ ; }
									}
						else if(b == '-'){
									csec-- ;
									if(csec < 1){csec = maxsec ; ctrack-- ; }
									}
					   else { ermsg("Illegal first parameter");
						      break ;
						    }
					   if(!checkpar( ctrack, csec ))break;
					   calc();
					   gsec( ctrack, csec, cbuf );
					   display( cbuf );
				       break ;
			case 'E' : if(b == 'T'){ ctrack = x ;
								     if(c == 'S')csec = y ;
									 else { ermsg("Illegal second parameter");
									        break;
										  }
					               }
				       else if(b == 'S'){ csec = x ;
						             if(c == 'T')ctrack = y ;
									 else { ermsg("Illegal second parameter");
								            break ;
										  }
								     }
					   else if(b == '+'){
									csec++ ;
									if(csec > maxsec){csec = 1 ; ctrack++ ; }
									}
						else if(b == '-'){
									csec-- ;
									if(csec < 1){csec = maxsec ; ctrack-- ; }
									}
					   else { ermsg("Illegal first parameter");
						      break ;
						    }
					   if(!checkpar( ctrack, csec ))break ;
				       calc();	
					   gsec( ctrack, csec, cbuf );
					   display( cbuf );
					   p = cbuf ;
					   g = 0 ;
					   while((g = hget( g, p )) != ERROR);	
				       break ;
			case 'W' :  if(b == 'T'){ ctrack = x ;
								     if(c == 'S')csec = y ;
									 else { ermsg("Illegal second parameter");
									        break;
										  }
					               }
				       else if(b == 'S'){ csec = x ;
						             if(c == 'T')ctrack = y ;
									 else { ermsg("Illegal second parameter");
								            break ;
										  }
								     }
					   else if(b == '+'){
									csec++ ;
									if(csec > maxsec){csec = 1 ; ctrack++ ; }
									}
						else if(b == '-'){
									csec-- ;
									if(csec < 1){csec = maxsec ; ctrack-- ; }
									}
					   else { ermsg("Illegal first parameter");
						      break ;
						    }
				       if(!checkpar( ctrack, csec ))break ;
					   calc();
					   psec( ctrack, csec, cbuf );
				       break ;		
				default : ermsg("Illegal command");
				}
			}
		p = cbuf ;
		display( p );
		g = 0 ;
		while((g = hget( g, p )) != ERROR);
	}
	
	
/*
**  A special version of putchar, uses direct BIOS calls
*/

putchar( c )
char c ;
{
	if(c == '\n')bios( 4, 0x0d );
	bios( 4, c );
	if(bios(2))stop();
}


