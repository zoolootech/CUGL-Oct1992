
/*! wreopen ()
 *
 *	makes the desired window the active window
 *	Does NOT check that no other window overlaps this one
 *	you can get screen gibberrish if you switch between
 *	overlapping windows
 *
 *  	The previous window becomes the second window on the chain.
 */


#include "wscreen.h"
#include "wsys.h"





void  wreopen(WINDOW *Wnew)
	{
	WINDOW *p;

	int oldpage, newpage;

	if ( w0 == Wnew )
		{
		return;
		}

	/* find the window that 'points' to the desired new window
	 */
	for (p = w0; p->winchain != Wnew && p !=NULL; p= p->winchain)
		{}


	if (p==NULL)
		{
		werror('W', "WREOPEN-window not found");
		}


	/* now p points to the window immediately more recent than 'new' one
	 */
	p->winchain   	= Wnew->winchain;	/* remove new from chain */
	Wnew->winchain   = w0;
	w0	        = Wnew;


	newpage = w0->		winpage;
	oldpage = w0->winchain->winpage;




	if (oldpage != newpage)
		{
		wpage_ram = wvideo_ram + wpage_size*newpage;
		}


	#ifndef TEXTONLY
                /* align BGI graphics driver
                 */
                walign (oldpage);
	#endif /* ! TEXTONLY */

	return;	/* wreopen */
	}
