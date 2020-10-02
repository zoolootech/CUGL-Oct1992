/* wbutton.c
 *--------------------- 'BUTTON'  functions -----------------------------*
 *
 *	buttons are selectable choices onscreen
 *		associated with either mouse or keyboard action.
 *
 *
 *	wbutton_add () 		- add a button to the list.
 *				     install button handler if not yet done
 *				     add new button to linked list
 *				     draws the button onscreen.
 *
 *				  OPTIONS:
 *					WBTN_BOX = draw a box around button
 *
 * 	wbutton_frame () and
 *	wbutton_draw  ()        - draw the button onscreen, draw box
 *				  internal utilities.
 *				  also called by
 *					wbutton_mark, _inactivate(), etc...
 *
 *	event_handler () 	- keyboard trap routine.
 *
 *	METHOD:
 *		see definition of WBUTTON above.
 *
 *
 *	these routines keep linked lists list of currently active buttons
 *
 *	one list is maintianed for each window.
 *
 *	when a button is added, wbutton_add()
 *		allocates memory for WBUTTON struct and adds to linked list.
 *		draws button onscreen.
 *
 */
#include "wsys.h"


void wbutton_add (char *utext, int ux, int uy, int ulen,
			int uval,
			unsigned char ustyle )
	{
	WBUTTON 	*Bptr;


	/* allocate storage and place new button in linked list
	 */
	Bptr = wmalloc ( sizeof (WBUTTON), "wbutton_add" );
	Bptr->Bchain = w0->winbutton;
	w0->winbutton = Bptr;


	Bptr-> Btext = utext;
	_NORMALIZE ( Bptr-> Btext );

	Bptr-> Bval  = uval;
	Bptr-> Bstyle= ustyle;

	Bptr-> Bx    = ux;
	Bptr-> By    = uy;


	Bptr->Blen   = ulen;

	/* compute location of end of button
	 * note that ulen includes terminal NULL.
	 * but stored value in WBUTTON strutcure is just the displayed bytes.
	 *
	 * so subtract 1 for terminal NULL .
	 */
	Bptr-> Bxend = ux + ulen -1;
	Bptr-> Byend = uy;


	wbutton_draw ( Bptr, wbuttonattr );

	if ( ustyle & WBTN_BOX )
		{
		/* draw a box around button
		 */
		--(Bptr->Bx);
		--(Bptr->By);
		++(Bptr->Bxend);
		++(Bptr->Byend);

		wbutton_frame ( Bptr, SINGLE_BORDER, wreverseattr(wbuttonattr) );

		}



	Bptr-> Bstyle |= WBTN_ACTIVE + WBTN_BUTTON;


	return;	/* wbutton_add */
	}




void wbutton_draw ( WBUTTON *Bptr, unsigned char new_attr )
	{


	unsigned char user_attr, user_x, user_y;

	int   	len;

	user_attr = wgetattr();		/* save attribute & position */
	user_x    = wherex();
	user_y    = wherey();

	wsetattr ( new_attr );

	/* number of onscreen bytes,
	 * which doesn;t count terminal NULL
	 *
	 *
	 */
	len = (Bptr-> Bxend) - (Bptr-> Bx);

	wgoto ( Bptr->Bx, Bptr->By );
	
	wputfl ( Bptr->Btext, len );	/* fixed len put */
	
	wsetattr ( user_attr );		/* restore ol attribute */
	wgoto ( user_x, user_y );


	return;	/* wbutton_draw */
	}



void wbutton_frame
	 ( WBUTTON *Bptr, unsigned char type, unsigned char color )
	{
	int l, t, r, b;

	l = (w0-> winleft) + (Bptr->Bx);
	t = (w0-> wintop ) + (Bptr->By);
	r = (w0-> winleft) + (Bptr->Bxend)  -1;
	b = (w0-> wintop) + (Bptr->Byend);

	wframe (l, t, r, b, type, color );
	}


/*---------------- end of WBUTTON.C ----------------------*/
