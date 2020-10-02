/* demo use of pages in the windows system
 * This simple program writes output to each available page
 * and the turns the pages on at a time to show the available ranges.
 *
 * Demo works in text or graphics modes, also on EGA/VGA works in 43/50 lines
 *
 */
#include <stdlib.h>
#include <string.h>

#include "wtwg.h"

main ()
	{
	int pagecolor;

	char buffer[40];

	int n;
	char mode;

	
	memset ( buffer, 0, sizeof (buffer) );

	winit ( 'T' );
	
	
	mode = wpromptc (NULL, "MODE",  "Text", "Graphics", NULL );
	if ( mode == ESCAPE )
		{exit(00);}

	/* initialize and tell system that paging will be required
	 *  	(this could as well have been first init call)
	 *
	 * NOTE: automatically clears video ram on higher pages,
	 *		for hercules selects 'FULL' video mode enabling page 0 & 1
	 *		for VGA graphics, selects medium resolution 640x350 2 page mode
	 */
	winit_pages (mode);



	/* if text-mode, demonstrate how to use 50-line mode
	 */
	if ( mode == 'T' )
		{
		if ( wmonitor == 'V' )
			{
			n = wpromptc (NULL, "RUN IN 50-LINE MODE ?", "YES", "NO", NULL );
			}
		else
		if ( wmonitor == 'E' )
			{
			n = wpromptc (NULL, "RUN IN 43-LINE MODE ?", "YES", "NO", NULL );
			}
		else 
			{
			n = 'N';
			}
		
		if ( n == 'Y' )
			{
			w50line (ON);
			/* NOTE that mouse mvt is adjusted to match size of screen.
			 * 		size of wfullscreen is adjusted, and lower half is cleared. 
			 */
			}
		else
		if ( n == ESCAPE )
			{
			exit (0);
			}
		
		}


	/* open 1 window on each page
	 */
	for ( n = 0 ; n <= wlastpage; ++n )
		{
		wnextpage = n;		
	
		/* clear fullscreen on this page
		 */	
		wdefine ( 0,0, wxabsmax +1, wyabsmax +1, n<<4, NO_BORDER, 0 );
		wclear ();

		if ( wmonitor == 'H' )
			{
			/* two pages in hercules,
			 * show page 0 in normal video,
			 * page 1 in reverse
			 */
			pagecolor =  ( n )?  0x70 : 0x07;
			}
		else
			{
			pagecolor = 1+ n;
			}


		wopen ( 10+(3*n), 3, 40, 5,
			pagecolor, SINGLE_BORDER, pagecolor, 0 );
		wgoto ( 2, 3 );
		wprintf ("This window is on page #%d", n);

		if ( ( wmode == 'G' || wmonitor == 'H' )  && n == 1 )
			{
			wgoto ( 2,4 );
			wputs ("NOTE no mouse on second page (1)" );
			}
		wabandon();		/* leave the boxed message onscreen */
		wabandon();		/* abandon the full screen window on this page */
						/* NOTE: current window is wfullscreen (on page 1) */
		
		
		}

	/* we are now on the highest avail page
	 * need to restore output to page 0.
	 * still have not flipped actual video page, page 0 is displayed.
	 */
	wnextpage	=	0;

	wpromptc (NULL, "Weve written on all the pages but haven't\n"
			"changed the display off of page 0 yet",
			NULL);

	for ( n = 0 ; n <= wlastpage; ++n )
		{
		/* changes the displayed page but does not change the
		 *	output page (where wputs() etc write to
		 */
		wturnpage (n);

		if ( n == wlastpage )
			{
			/* demonstrate getting input on highest page 
			 * First change 'output' video page to match display page.
			 * Then select prompt location for bottom left of window
			 * independent of mode (25-line, 43- or 50-line or graphics)
			 */
			wnextpage = n;
			wsetlocation ( WLOC_ATXY, 1, wyabsmax-10 );
			
			buffer[0] =0;
			wprompts ( "DEMO INPUT", "TYPE SOMETHING HERE", 
						buffer, sizeof (buffer) );
					
			}
		else
			{
			/* just pause for any key...
			 */
			wgetc();
			}
		}
	/* write on page 0 while highest page # is still being displayed
	 * by reoppening a window on page 0, output is automatically change 
	 * to that page
	 */
	wreopen (wfullscreen);
	wclear ();
	wgoto ( 20, 10 );
	wputs ("When on highest video page, you typed:" );
	wgoto ( 20, 11 );
	wputc ( '>' );
	wputs ( buffer );
	wputc ( '<' );
	wgoto ( 20, 13 );
	wputs ( "End of demo. Press any key to restore mode and exit... ");
	
	/* screen still displays highest page number - haven't flipped page
	 */

	/* change physical display to show page 0
	 */
	wturnpage (0);

	wgetc();	/* wait */

	/* automatic video cleanup (restores from graphics or 50-line modes...
	 */
	return (0);
	}