/* WBUTTOND.C
 *
 *	contains wbutton_delete () -- remove a button from the list.
 *
 */
#include "wsys.h"






void wbutton_delete (int uval, int redraw)
	{
	WBUTTON *Bptr, **chain;



	chain =  & (w0->winbutton );	/* address of chain */

	for ( 	Bptr = w0->winbutton;
		Bptr;
		chain = &(Bptr->Bchain),  Bptr = Bptr->Bchain )
		{
		if ( Bptr->Bval == uval )
			{

			if ( 0== redraw )
				{
				/* need to erase button
				 * so setup a 'blank' button.
				 * 		in the large model, *(NULL) is not necessarily 0
				 */
				Bptr-> Btext = " ";
				}

			if ( Bptr-> Bstyle & WBTN_BOX )
				{
				/* erase the box first,
				 * then shrink the size counters
				 * so that wbutton_draw will work
				 * in text area only.
				 */
				wbutton_frame ( Bptr, NO_BORDER, wgetattr() );

				++(Bptr->Bx);
				++(Bptr->By);
				--(Bptr->Bxend);
				--(Bptr->Byend);
				}

			wbutton_draw ( Bptr, wgetattr() );

			/* unlink button from chain */
			*chain  = Bptr->Bchain;

			free ( Bptr );


			break;		/* found button -- quit */

			}


		}


	return;	/* wbutton_delete () */
	}

