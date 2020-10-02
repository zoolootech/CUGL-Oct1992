/* WBUTTONA.C
 *
 *	contains  wbutton_activate () and wbutton_inactivate ()
 *
 *	routines to temporarily acivate and inactivate a button.
 */
#include "wsys.h"



static void W_NEAR do_redraw ( WBUTTON *Bptr, int redraw );

#define REDRAW_BUTTON   -1


void wbutton_inactivate (int uval, int redraw)
	{
	WBUTTON *Bptr;

	Bptr = wbutton_getptr ( uval );

	if ( (NULL == Bptr) )
		{
		return;
		}

	do_redraw ( Bptr, redraw );

	Bptr-> Bstyle &= WBTN_ACTIVE;	/* inactivate */

	return;	/* wbutton_inactivate () */
	}





void wbutton_activate ( int uval )
	{

	WBUTTON *Bptr;

	uval = toupper (uval);


	for ( 	Bptr = w0->winbutton;
		Bptr;
		Bptr = Bptr->Bchain )

		{

		if ( Bptr-> Bval == uval )
			{

			do_redraw ( Bptr , REDRAW_BUTTON );


			break;		/* quit for() loop */
			}


		}


	return;		/* wbutton_activate */
	}



static void W_NEAR do_redraw ( WBUTTON *Bptr,  int redraw )
	{

	char *save_text;

	char new_attr;

	save_text = Bptr-> Btext;


	/* draw button in button attr or in window attr ?
	 */
	new_attr = (redraw== REDRAW_BUTTON) ? wbuttonattr : wgetattr ();


	if ( ! redraw )
		{
		/* need to erase button
		 * so setup a 'blank' button.
		 */
		Bptr-> Btext = NULL;
		}
	if ( Bptr-> Bstyle & WBTN_BOX )
		{
		/* erase the box first,
		 * then shrink the size counters
		 * so that wbutton_draw will work
		 * in text area only.
		 */
		wbutton_frame ( Bptr, NO_BORDER, new_attr);

		}
	wbutton_draw ( Bptr, new_attr );


	Bptr->Btext = save_text;

	return;	/* redraw */
	}