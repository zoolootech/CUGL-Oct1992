/* whide.c
 *	contains whide() and wshow()
 *
 * 	These routines cover up the current window with the prior screen
 *	and then restore it, without releasing the window
 *
 *	They must be called in pairs, whide() first, then wshow()
 *		without an intermediate call to wopen() or similar functions
 *
 *	PARAMETERS:  	whide()-- none.
 *			wshow()-- takes the pointer returned by whide.
 *
 *	RETURNS:	whide()-- a void far *, to pass to wshow later.
 *			wshow()-- none.
 */
#include "wsys.h"



WHEAP  *whide (void)
	{
	WHEAP  *oldimage;
	WHEAP  *newimage;

	/* can't hide if no old save area
	 */
	if ( NULL ==  (oldimage = w0->winsave) )
		{
		return (NULL);
		}

	w0->winsave = NULL;
	wsave ();		/* get current image */

	newimage = w0->winsave;

	w0->winsave = oldimage;


	wrestore ();

	return (newimage);	/* whide */
	}







void wshow ( WHEAP  *newimage )
	{
	WHEAP *oldimage;


	if ( NULL == (oldimage = w0->winsave) )
		{
		return;
		}


	w0->winsave = newimage;
	wrestore ();


	w0->winsave = oldimage;

	if ( newimage )
		{
		wheap_free ( newimage );
		}


	return;		/* wshow */
	}