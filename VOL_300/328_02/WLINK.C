/* wunlink(), wlink()
 *	unlink the current window from the chain of windows.
 *		the image of the window is saved (pointer winsave).
 *		the screen is not changed.
 *      this function is used just before changing modes
 *			   or just before wcloseall
 *	allows subsequent wlink to re-establish this window
 *      	when you come back to the window's mode.
 *
 *		RETURNS: pointer to the window if successful, NULL if error
 *
 *
 *	wlink opens the specified window, restores its image to the screen
 *		parameter = window pointer obtained by wunlink ()
 *		RETURNS: 0 if successful, -1 on error .
 *
 *	do NOT place a text-mode window on a graphics mode screen
 *		or vice-versa.
 *
 *	example: 	winit ('T');
 *                      wopen (... );
 *			wputs ("A bunch of stuff...);
 *			my_textw0indow = wunlink ();
 *			init ('G');
 *				do graphics things...
 *			winit ('T');
 *			wlink ( my_textw0indow );
 *
 *
 */
#include "wsys.h"



/* local function to define a blank window that sits on top of
 * affected area of screen
 */
static void overlay ( void );


WINDOW 	*wunlink (void)

	{

	WINDOW *oldw;
	WHEAP  *olds;




	/* cannot unlink the only window or the fullscreen window
	 */
	if ( w0-> winchain == NULL || w0 == wfullscreen )
		{
		return (NULL);
		}


	oldw = w0;
	olds = w0->winsave;


	/* place a new window on top of the current window
	 * the save area of the new window will have the contents of
	 * the current values of the current window
	 */
	overlay ();


	/* swap the save area pointers
	 * and close the old window ( will restore screen if needed )
	 */

	oldw->winsave = w0 -> winsave;	/* contents of window to unlink */
	w0  ->winsave = olds;		/* screen prior to window */

	wclose();

	w0 = w0-> winchain; 		/* unlink */

	return (oldw);

	}




void    wlink (WINDOW  *neww)

	{
	WHEAP *news;


	/* always place new window in current page */
	neww-> winpage = w0->winpage;

	news = neww-> winsave;



	neww-> winchain = w0;	/* link into chain */
	w0 = neww;



	overlay ();

	/* swap save area pointers and restore
	 * screen to show new window contents
	 */
	neww-> winsave = w0-> winsave;
	w0->winsave = news;



	wclose();






	return;		/* wlink -- successful */
	}







/* overlay()
 * static fucntion to create a dummy window
 * in the screen area of the window of interest.
 */
static void overlay (void)
	{
	int r, l, t, b, extra, extra2;


	extra  =  ( w0->winbox ) ? 1: 0;
	extra2 = 2*extra;

	l = w0-> winleft    - extra;
	t = w0-> wintop     - extra;
	r = w0->winxmax + extra2;
	b = w0->winymax + extra2;

	wopen ( l, t, r, b, 0x07, NO_BORDER, 0x07, WSAVE2RAM );

	return;	/* overlay */
	}



/*------------------ WLINK.C -------------------------*/