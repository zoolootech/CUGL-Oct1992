
/*! wrelocate()
 *	move a window to a new location
 *	the window must have been created with WSAVE2RAM.
 *
 *	RETURNS: 0 if successful
 *		-1 if failure. Can fail because:
 *			wndow was openned with WSAVE2NULL - cannot restore
 *			new address out of bounds
 *			not enough memory to save images of screen
 *
 */
#include "wsys.h"



int wrelocate (int x, int y)
	{
	WHEAP *background, *contents;

	int borderwidth;        /* size of border   */
	int ht, wt;		/* height and width */

	borderwidth = w0-> winbox  ?  2 : 0;

	if (	w0-> winsave == NULL
	   )
		{
		/* can't move a window without a save area
		 */
		return (-1);
		}
	ht = w0->winymax  +borderwidth;
	wt = w0->winxmax  +borderwidth;

	/* check that new position is inside screen bounds
	 */
	if ( x < borderwidth )  x = borderwidth;
	if ( y < borderwidth )  y = borderwidth;
	if ( x+ wt  > wxabsmax )
		{
		x = wxabsmax - wt -1;
		}
	if ( y+ ht > wyabsmax )
		{
		y = wyabsmax - ht -1;
		}

	background = w0->winsave;	/* underlying screen data */
	w0->winsave = NULL;		/* prevent wsave from re-using */

	/* save current window contents
	 */
	wsave();
	contents = w0-> winsave;
	if ( !contents )
		{
		/* not enough memory to perform move */
		return (-1);
		}


	/* restore old background
	 */
	w0->winsave = background;
	wrestore ();
	wheap_free (background);	/* release far memory */
	w0-> winsave = NULL;


	/* move window
	 */
	w0->winleft =x;
	w0->wintop  =y;

	/* save background in new screen area
	 */
	wsave();
	background = w0->winsave;
	if ( ! background )
		{
		/* same amount of memeory that was freed last time
		 */
		werror ('W', "WRELOCATE-internal");
		}

	/* now put window image to screen
	 */
	w0->winsave = contents;
	wrestore ();
	wheap_free (contents);

	w0->winsave = background;

	return (0);	/* wrelocate - successful */
	}




