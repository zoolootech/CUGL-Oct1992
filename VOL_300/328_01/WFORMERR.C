

/* WFORMERR.C
 *	source file for form error check function
 *
 *      This function can be named in the header of a WFORM table
 *	It loops through all the form data items,
 *		and calls any special validation functions for eachitem
 *	If any of the data is invalid, this function will return (1)
 *	else, will return (0) for all valid;
 *
 */
#include "wsys.h"

void  wform_showerror (WFORM *form, char *errmsg)
	{
	int l,t, xmax;
	char *msg;


	msg = errmsg ? errmsg : "Invalid. Press any key, then re-enter.";

	xmax = 3 + strlen (msg);

	/* locate border of window 1 over and 1 down
	 */
	wsetlocation ( WLOC_ATWIN, (form->wfx)+1, (form->wfy)+1 );

	wlocate ( &l, &t, xmax, 2 );


	wopen ( l, t, xmax, 2,
		(RED <<4) + YELLOW, DOUBLE_BORDER, (RED<<4)+YELLOW,
		WSAVE2RAM );

	w0-> winputstyle &= ~(WPUTWRAP + WPUTSCROLL);

	wputc ( 0x18 );		/* up-arrow */
	wputc ( ' ' );
	wputs ( msg );

	wgetc();

	wclose();


	return; /* wform_showerror() */
	}



/*------------- end of WFORMERR.C -------------------*/
