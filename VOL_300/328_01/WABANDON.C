/*! wabandon.c
 *
 *
 *    	release window storage for current window,
 *      unlink current window from linked list of windows,
 *	make sure at least one window remains open.
 *
 *
 */

#include  "wscreen.h"
#include  "wsys.h"



static void rectify (int);



void wabandon(void) {

	WINDOW *p;

	int oldpage;

	WBUTTON *btn, *next_button;




	/* free image storage area.
	 */
	if ( w0-> winsave )
		{
		wheap_free (w0-> winsave);
		w0->winsave = NULL;
		}



	/* save page # of current window
	 * 	can't reference next page if this is the last window.
	 */
	oldpage = w0->	winpage;



	/* release any allocated button memory
	 */
	for ( btn = w0-> winbutton;   btn != NULL;  btn = next_button)
		{
		next_button = btn->Bchain;
		free (btn);
		}





	/* Now either bury the fullscreen window at the bottom of the stack
	 * 	or close the indicated window
	 */
	if ( w0 == wfullscreen )
		{
		/* this is the fullscreen window - never shut it
		 */
		wbury  ();
		}
	else
		{

		/* Remove old window from chain, release window memory
		 */
		 p = w0;  		/*p points to the current window */
		w0 = (p-> winchain);
		free(p); 		/* release the (old) closed window */

		/* w0 now points to previous window, old one is gone
		 */
		rectify( oldpage );
		}
	return; /* wabandon */
	}




/* wbury ()
 *	take the indicated window and place it on bottom of chain
 *  	screen is not changed.
 *
 */
void wbury (void)
	{
	WINDOW *p;
	int oldpage;

	oldpage = w0-> winpage;

	if ( (p= w0->winchain) == NULL )	/* look for next window down on list */
		{
		/* nothing to bury window under
		 */
		return;
		}

	while ( p->winchain )
		{
		p = p-> winchain;
		}

	/* p now points to the bottom window in the chain
	 *    (the one with a NULL winchain ptr)
	 * set the chain ptr to the (old) open window
	 */
	p-> winchain = w0;

	/* now use p to save the address of the window about to be promoted
	 */
	p = w0-> winchain;
	
	
	/* now set the chain ptr in the (old) open window to NULL
	 * to show it is the (oldest) window in the chain (nothing below)
	 */
	w0-> winchain = NULL;
	
	/* now point w0 to the (new) top window on the chain
	 */
	w0 = p;

	rectify ( oldpage );

	return;		/* wbury */
	}





static void rectify ( int oldpage )
	{
	int newpage;

	newpage = w0->winpage;
	if (oldpage != newpage)
		{
		wnextpage = newpage;
		wpage_ram = wvideo_ram + wpage_size*newpage;
		}

	#ifndef TEXTONLY
		walign (oldpage);	/* align BGI graphics driver*/
	#endif /*TEXTONLY */

	if (w0-> winflag  & WFL_CURSOR)		wcursor (ON);
	else 					wcursor (OFF);


	return;		/* rectify */
	}
	
	/*------------------- end of WABANDON.C -----------------------*/