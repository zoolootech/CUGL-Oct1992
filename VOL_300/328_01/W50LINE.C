/* w50line.c - control EGA/VGA display in text modes to 43/50 line 
 *
 *	USAGE:  w50line ( ON_OFF );
 *			PARAMETER:	ON_OFF = if 0, restore to 25 line mode
 *									non-0, set to 50/43 line
 *	RETURNS:  int is50  = 0 if in 25 line mode,
 *						  non-0 if in 50/43 line mode
 *	ALSO:	changes winymax to 42,49, or 24 as needed.
 *			changes the size of wfullscreen, the initial fullscreen window.
 *			moves the cursor to within a 25-line window if needed.
 *			changes parameters governing multiple video pages.
 *			allows for 2 top protected lines if pulldown menus are installed.
 *
 *	WARNINGS:	When switching back to 25-line mode, this function 
 *				does not check any currently open windows
 *				to make sure they are inside the 25-line screen.
 *
 *  This function has no effect in graphics modes or on HERCULES cards. 
 *
 *	ACKNOWLEDGEMENT: The logic for switching to 43/50 line mode comes
 *		from DOS POWER TOOLS by Somerson. 
 *
 */
#include "wsys.h"


static void clear50(void);	/* prototype for atexit func */
static char exit_installed 	=0; 

static char is50			=0;

#define  BIOS_CURSOR_EMULATION   *( (unsigned char far *) 0x0487L )

#define BIOS_PAGE_SIZE  *( (int far *) 0x044CL )

int w50line ( int want50 )
	{
	int  nlines;
	unsigned char save_BIOS_CURSOR;
	
	PSEUDOREGS
	
	
	if ( 
		( wmonitor == 'E'  || wmonitor == 'V' )
	#ifndef TEXTONLY
		/* do nothing if in a graphics mode */
		&&   ( wmode == 'T' )
	#endif  /* TEXTONLY */
		)
			{
			if ( want50 )
				{
				if ( !exit_installed )
					{
					/* install exit function only once per program
					 */
					exit_installed = ON;					
					atexit (clear50);
					}
					
				/*set 8-byte font
				 */
				_BX = 0;			/* load font */
				_AX = 0x1112;
				INTERRUPT ( 0x10 );
				
											
				wcurscanln = 0x0607;

				/* force EGA cursor to correct size
				 *	A fairly complex situation here.
				 *		with 'cursor emulation' ON, 
				 *			EGA card converts row 6 to row 12 (for CGA compat.)
				 *		so have to turn OFF cursor emulation, then set cur. row
				 *
				 */
				save_BIOS_CURSOR = BIOS_CURSOR_EMULATION;
				BIOS_CURSOR_EMULATION |= 0x01;	/* turn OFF emulation*/
				_CX = 0x0600;					/* cursor size request */
				_AH = 1;
				INTERRUPT ( 0x10 );
				BIOS_CURSOR_EMULATION = save_BIOS_CURSOR;	/* restore emul. */
				outport ( 0x03B4, 0x0714 );
				
				
				/* set number of screen lines
				 * dividing BIOS_PAGE_SIZE by 80 (=#columns) doesn't work
				 *	for some reason in VGA BIOS page size is 64 bytes too big.
				 */
				nlines = ( wmonitor == 'V' ) ? 50 : 43;
				
				/* setup windows globals
				 */					
				wyabsmax = nlines -1;	
				
				/* adjust size of wfullscreen to allow for top menu
				 */
				wfullscreen->winymax = nlines-1 - (wfullscreen->wintop);
				
				/* clear lower half of screen to fullscreen color
				 */
				wdefine ( 0, 25, wxabsmax+1, nlines-25, 0, 0, 0);
				wsetattr ( wfullscreen-> winattr );
				wclear ();
				wabandon();
				 
				/* setup multiple pages if this is a 'paging' program
				 */
				if ( wlastpage )
					{
					/* have to change the PAGE size to reflect smaller chars.
					 * also, for reasons totally unclear to me,
					 * the pages in 50-line mode are 32 chars =64 bytes longer.
					 * ie, 64 bytes are unused in VGA RAM at end of each page.
					 * and the pages in 43-line mode are something else.
					 *
					 * Therefore, get the page size form the BIOS storage area.
					 */
					wlastpage = 3;		/* pages 0, 1, 2, 3 = 4 total */
					wpage_size = BIOS_PAGE_SIZE;
					}
				is50 = ON;
				}
			else
				{
				/* don't want50 */
#if 0					
			/* OLD-FASHIONED WAY TO SET 25 LINE MODE
			 */
				/*set BIOS to 25 line mode
				 */
				_BX = 0;
				_AX = 0x1111;
				INTERRUPT (0x10);
				
				wcurscanln = 0x0C0D;		/* cursor scan lines */
		
				/* force EGA/VGA cursor to correct size
				 */
				_CX = 0x0C0D;			/* cursor size request */
				_AX = 1;
				INTERRUPT ( 0x10 );
				outport ( 0x03B4, 0x0D14 );
#endif /* 0 */
			/* NEW, easier way to restore 25-line mode
			 */
				/* revert video using BIOS mode select
				 * MODE=3, bit 7 of AL =1 means save video contents
				 */
				_AX = 0x0083;	
				INTERRUPT ( 0x10 );
				
				nlines   = 25; 
				wyabsmax = 24;
				
				/* wfullscreen may have been shrunk by 2 lines by wpulldown() 
				 */
				wfullscreen->winymax = 24 - (wfullscreen-> wintop);
				if ( w0 -> winy > 24 )
					{
					/* make sure cursor is inside 24-line boundary
					 */
					wgoto ( -1, 24 );
					}
				if ( w0 != wfullscreen )
					{
					/* make sure wfullscreen is also inside 24-line boundary
					 */
					if (wfullscreen-> winy > (wfullscreen->winymax) )
						{
						wfullscreen-> winy = (wfullscreen->winymax);
						}
					}
				
				/* reset multiple pages if this is a 'paging' program
				 */
				if ( wlastpage )
					{
					wlastpage = 7;		/* pages 0..7  = 8 total */
					wpage_size = 4096;
					}
				is50 = OFF;
				}
						
			/* the cursor was turned 'ON' during the above manipulation.
			 * if the cursor was 'OFF' prior to calling this routine, 
			 * it should be turned 'OFF' again.
			 */
			if ( (w0-> winflag & WFL_CURSOR) == 0 )
				{
				wcursor (OFF);
				}
						
			/* change mouse mvt limits to match nlines.
			 */
			if ( wmouse.wms_present )
				{
				_DX = (nlines*8)-1;
				_CX = 0;
				_AX = 0x08;
				INTERRUPT ( 0x33 );
				}
										
			}		/* end if wmonitor== 'E' or 'V' and wmode=='T' */
			
	return (is50);		/* w50line() */
	}
	
static void clear50 (void)
	{
	if ( is50 )
		{
		w50line ( OFF );
		}
	return;		/* clear50() - called only atexit */
	}
	