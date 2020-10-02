/* WHPLJ2.C
 * screen dump routine for laserjet II and other HP PCL-III printers.
 *	NOTES: 
 *		1) routine reads one byte (=8pixels) directly at a time from the screen
 *			(most commercial drivers seem to read 1 pixel at a time)
 *		2) writes to printer using compaction mode 0
 *			NOTE: version 2 coming - uses compaction mode 2 for speed
 *					currently in debug status
 *		3) senses TEXT or GRAPHICS mode, prints accordingly
 *		4) on color monitors in GRAPHICS modes dithering is same as for hercules:
 *				any pixel with either GREEN or BLUE is printed
 *					ie: GREEN, CYAN, YELLOW, PURPLE are;   RED is not.
 *		5) routine can handle SVGA up to 800*600 16 color.
 *				(larger screens require more programming logic
 *				 because compaction counts are stored in signed char)
 *		6) VGA 50 line text mode: entire screen is printed
 *		7) multiple page programs: printing is the active page
 *			(not necessarily the display page)
 *			NOTE: limitation of DOS PrtScrn always prints page 0.
 *		8) when compiling, define DOUBLE_ROWS to print each row twice.
 *
 *	usage: pass routine a FILE pointer. Use stdprn to get DOS printer
 *			Use a pointer to an open BINARY mode output file 
 *				to save a set of HPLJ commands to a disk file
 *			then execute dos command:  TYPE  FILENAME > LPT1:
 */
#include "wscreen.h"
#include "wsys.h"

/* HP PCL-III command codes		( LaserJet II, DeskJet, compatibles ) 
 * notes: 	1) resolution is set to 100 DPI
 *				which fills an 8" width in SVGA mode
 *			2) maxline memory toggle is setup to accomodate SVGA mode
 *					(maxline is not a PCL-III code 
 *					 but is supported on L-J II, III, and D-J )
 */
#define GR_END			"\x1B" "*rB"
#define GR_RESOLUTION  	"\x1B" "*t100R"
#define GR_MAXLINE		"\x1B" "*r800S"  
#define GR_MODE			"\x1B" "*b0M" 
#define GR_START		"\x1B" "*r0A"
#define GR_TRANSFER		"\x1B" "*b%iW"		/* %i = number of bytes */
#define GR_RESET		"\x1B" "E"
#define GR_PAGE			"\x0C"

#define  MAX_NB	  256		/* max number of row bytes, way more than SVGA */

#ifndef TEXTONLY
	/* graphics row print routine */
	static void	rowprint ( FILE *f, int nb, signed char *rowdata );
	static void	rowprint ( FILE *f, int nb, signed char *rowdata )
		{
		fprintf ( f, GR_TRANSFER, nb );	
		fwrite  ( rowdata, nb, 1, f );				
		return;
		}		/* rowprint () */
	
#endif	/* ! TEXTONLY */



	 
void whplj_dump ( FILE *f )
	{
	int row, col;				/* MUST NOT be unsigned */
	unsigned char far *screen;
	
#ifndef TEXTONLY
	unsigned char far *screenrow;

	unsigned int bank;				/* for HERCULES graphics bank register */

	signed char image_byte;

	/*  rowdata holds the row image.
	 */
	signed char rowdata[MAX_NB];	/* more than enough for 800 px SVGA */ 

#endif		/* ! TEXTONLY graphics definitions */
	
	
	
	if ( wmode == 'T' )		/* TEXT MODE SCREEN DUMP */
		{
		screen 	= wpage_ram;		/* current text video page address */
		row 	= wyabsmax;			/* =24,42, or 49 if 25,43, or 50 line mode*/
		do  {
			col = 79;
			do  {
				fputc ( *screen, f );
				screen +=2;					/* skip attribute byte */
				}	while ( --col >= 0 );
			
			fputs ( "\r\n", f );		/* new line */
			}	while ( --row >= 0 );
		}


#ifndef TEXTONLY
	else
		{
		maxcol = (wpxabsmax+1)/8;	/* #pixels per row / by pixels per byte */

		fputs ( GR_END GR_RESOLUTION GR_MAXLINE GR_MODE GR_START, f );
			
		screenrow = wpage_ram;		/* note: hercules row 0 is bank 0 */
		bank =0;	
	
		for ( row=0; row <=wpyabsmax; ++row )	/* note <= comparison */
			{
			for ( col=0; col<maxcol; ++col )
				{
				screen = screenrow + col;
				
				/* READ SCREEN BYTE at row, col. */
				if ( wmonitor == 'H' )			/* read screen for Herc. */
					{
					image_byte =  screen [bank];
					}
				else							/* read screen for VGA */
					{
					EGA_OUT (4, 0);		/* read color BLUE */
					image_byte = *screen;	
					EGA_OUT (4, 1 );	/* read color GREEN */
					image_byte |= *screen;
					}							/* end read screen portion */	
				rowdata [col] = image_byte;
				}
			rowprint ( f, col, rowdata );	/* col contains actual num bytes */	
				#ifdef DOUBLE_ROWS
				  rowprint ( f, col, rowdata );  
				#endif
			
			/* advance to next row on screen */
			if ( wmonitor == 'H' )
				{
				bank += 0x2000;
				if ( bank==0x8000 )
					{
					bank = 0;
					screenrow += 90;
					}
				}
			else
				{
				/* EGA/VGA */
				screenrow += wegarowsize;	/* 80 for EGA/VGA, 100 for SVGA */
				}
	
			}					/* end for ( row... ) */
		fputs ( GR_END, f );
		}						/* end ...else graphics mode... */
#endif	/* graphics mode */
	
	fputs ( GR_PAGE GR_RESET, f );
	
	return;		/* whplj_dump() */
	}
	
/*------------------- end WHPLJ.C ------------------------------*/