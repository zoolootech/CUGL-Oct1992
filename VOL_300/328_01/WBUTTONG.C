
/* wbutton_getptr()
 * 	this function scans the list of buttons
 *	looking for a match to specified value.
 *	returns NULL if not found or inactive.
 *	returns ptr to button if found
 */
#include "wtwg.h"

#include <ctype.h>


WBUTTON *wbutton_getptr ( int uval )
	{
	WBUTTON *Bptr;
	
	for ( 	Bptr = w0->winbutton;
		Bptr;
		Bptr = Bptr->Bchain )
		{

		if ( uval == Bptr->Bval )
			{
			if (  ( Bptr -> Bstyle  & WBTN_ACTIVE  ) == 0 )
				{
				/* test to see if button is temp inactive
				 */
				Bptr = NULL;
				}
			break;
			}

		}

	return (Bptr);		/* wbutton_getptr */
	}

