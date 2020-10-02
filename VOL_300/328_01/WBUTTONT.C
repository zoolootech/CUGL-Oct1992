/* wbuttont.c
 *    CONTAINS wbutton_test ()
 *
 *	tests a keystroke (which may be a mouse)
 *	returns same if keystrke is an active button
 *	returns 0 if not a button.
 *
 *
 */
#include "wsys.h"

int wbutton_test ( int uval )
	{
	WBUTTON *Bptr;

	Bptr = wbutton_getptr ( uval );

	uval = Bptr ?  Bptr-> Bval  :  0;

	return (uval);		/* wbutton_test */
	}

