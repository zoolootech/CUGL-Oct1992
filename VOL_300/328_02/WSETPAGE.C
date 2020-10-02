/*! wsetvisualpage() and winit_pages()
 *
 *	support for multiple video pages.
 *
 *	functions like borland's setvisualpage,
 *	but runs in either text or graphics modes.
 *
 * 	also, this routine turns off the mouse on high pages in Hercules
 *		( some mouse drivers can handle Hercules monitor page 1,
 *		  but not all can, and technique is non-documented
 *		  hence dangerous )
 *	restores the mouse when page is switched back to page 0.
 *
 *	pass a page number,
 *	that page will be displayed.
 *
 *	Window output will still be written to the current window
 * 	which may or moy not be on the displayed page
 *
 *	If the requested page is out of bounds,
 *	the closest fit (0 or highest page) is used
 *
 *
 */


#include "wscreen.h"
#include "wsys.h"



static unsigned char mouse_present =0;


/* METHOD:
 *	Hercules: hercules control port at 0x3B8 contains:
 *		bit 7 (=hi, 0x80) = page assignment 0=0, 1=1
 *		bit 5 ( 0x20 )    = blink enable (always on).
 *                  3 ( 0x08 )    = video enable/disable
 *		    1 ( 0x02 )    = mode  ( 1= graphics )
 *		    0 ( 0x01 )    = hi-res control (always on)
 *	        unfortunately the port is write-only, so
 * 		need to construct appropriate bit values before setting.
 *	EGA/VGA:
 *		ROM BIOS sets displayed papge in either text or graphics mode
 */
#define  HCONTROL 0x29	/* blink enable, video enabel, hi-res all ON */


void wsetvisualpage (int page)
	{
	char herc_byte;		/* hercules control byte */



	PSEUDOREGS


	if (page <0)		page=0;
	if (page >wlastpage) 	page = wlastpage;


	if ( wmode == 'G' || wmonitor == 'H' )
		{
		/* in EGA graphics or any mode of hercules
		 * turn off the mouse on higher pages.
		 *
		 * MOUSE driver only supports higher pages
		 * in text mode on EGA/VGA
		 */
		wmouse.wms_present = ( page ) ? 0 : mouse_present;
		}




	if (wmonitor  == 'H')
		{

		/* build hercules control byte to set page.
		 * without affecting graphics mode, blink, or video enable
		 */
		herc_byte  =  HCONTROL;

		if ( page  == 1 )
			{
			herc_byte |= 0x80;
			}

		if ( wmode == 'G' )
			{
			herc_byte |= 0x02;
			}


		outp ( 0x3B8, herc_byte );

		}
        else
		{
		/* EGA or VGA */

		_AL = page;
		_AH = 5;
		INTERRUPT (0x10);

		}

	return;	/* wsetvisualpage() */
	}

static char installed_atexit = 0;
static void restore_pages_atexit (void);

/* winit_pages ()
 *
 */
void winit_pages ( char mode )
	{
	int n, x, y;




	wneedpage = ON;
	winit ( mode );

	if ( ! installed_atexit )
		{
		/* only install the atexit function once,
		 * to make sure system is on page 0 when terminates
		 */
		installed_atexit = 1;
 		atexit ( restore_pages_atexit );
		}


	if ( mode == 'T' )
		{

		/* setup default for EGA/VA
		 */
		wpage_size = 4096;
		wlastpage  = 7;		/* pages 0,1,2,3... */

		if ( wmonitor == 'H' )
			{
			wpage_size = 0x8000;
			wlastpage  = 1;		/* enable pages 0 & 1 */
			}
		else
		if ( wmonitor == 'M' )
			{
			wlastpage =0;	/* disable paging */
			}


		} /* end initializing newmode = T */

#ifndef TEXTONLY
	/* graphics mode init routine */

	else    /*( mode == 'G' ) */
		{
		wpage_size = 0x8000;
		wlastpage  = 1;
		}	/* end initializing newmode = 'G' */

#endif /* end ifdef TEXTONLY */







	/* clear video RAM in the higher memory pages
	 */
	x = wxabsmax;
	y = wyabsmax;
	for ( n=1; n <= wlastpage; ++n )
		{
		wnextpage = n;
		wdefine (0,0, x,y, 0x07, NO_BORDER, 0);
		wclear();
		wabandon();
		}
	wnextpage = 0;

	/* save mouse status so it can be turned on/off
	 * in hercules monitors when not on page 0
	 */
	mouse_present = wmouse.wms_present;

	return;		/* winit_pages () */
	}


static void restore_pages_atexit (void)
	{
	wsetvisualpage (0);
	
	return;		/* restore_pages_atexit */
	}