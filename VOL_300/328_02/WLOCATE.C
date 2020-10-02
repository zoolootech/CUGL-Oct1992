
/*! wlocate ()
 *
 *      assistance in locating a new window.
 *	can locate window at a specified x,y offset from a specified point.
 *
 *	specification is made in the global variable struct wlocation
 *	can specify offsets from any on of:
 *	     	center of screen (default)    WLOC_CENTER
 *	     	top left of screen            WLOC_ATXY
 *           	top left of current window    WLOC_ATWIN
 *              current cursor position       WLOC_ATCUR
 *	can specify x,y offsets which may be positive or negative.
 *	if location places any edge of window out of screen,
 *      	location is adjusted as needed.
 *      After each call, the wlocation structure is reset to CENTER fo screen.
 *
 *	PARAMETERS:
 *		aleft, atop = pointers to unsinged char
 *				which will contain the new location
 *		xmax, ymax  = size of desired window.
 */
#include "wsys.h"



void wlocate ( int *aleft, int *atop, int xmax, int ymax )
	{
	int x, y;
	register int leftmost, topmost;

	switch ( wlocation.wloc_type )
		{
	case ( WLOC_ATXY ):
		x = y = 0;
		break;
	case ( WLOC_ATWIN ):
		x = w0-> winleft;
		y = w0-> wintop;
		break;
	case ( WLOC_ATCUR ):
		x = w0-> winleft + w0->winx;
		y = w0-> wintop +  w0->winy;
		break;
	default:
		/* the default is WLOC_CENTER = center of screen */
		x = ( wxabsmax - xmax )/2;
		y = ( wyabsmax - ymax )/2;
		break;

		}	/* end swtich */

	x += wlocation.wloc_x +1;
	y += wlocation.wloc_y +1;


	/* check window to make sure it fits in screen
	 * allow room for borders
	 */
	if ( x<1 ) x =1;
	if ( y<1 ) y =1;

	leftmost = wxabsmax - xmax -3;
	x = min (x,leftmost);

	topmost = wyabsmax - ymax -3;
	y = min (y, topmost);

	*aleft = x;
	*atop  = y;


	/* reset wlocation so next call defaults to CENTER */
	wlocation.wloc_type = wlocation.wloc_x = wlocation.wloc_y =0;


	return;	/* wlocate */
	}
