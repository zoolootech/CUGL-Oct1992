/* WBUTTONM.C
 *
 *	contains  wbutton_mark ()
 *
 *	function to place a tickmark onscreen next to a button.
 *
 */
#include "wsys.h"



void wbutton_mark ( int uval, char mark )
	{
	unsigned char ux, uy;
	WBUTTON *Bptr;

	int true_y;


	Bptr = wbutton_getptr ( uval );


	if ( Bptr )
		{
		true_y = Bptr->By;

		if ( Bptr->Bstyle & WBTN_BOX )
			{
			/* 2 lines for this button;
			 */
			++true_y;
			}
		ux = wherex(); 		/* save caller text position*/
		uy = wherey();
		wgoto ( (Bptr-> Bx) -1, true_y);
		wputc (mark);

		wgoto ( ux,uy );	/* restore caller text pos. */


		}





	return;	/* wbutton_mark () */
	}



