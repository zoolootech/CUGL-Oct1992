/* wfvdate.c
 *	form entry validation of dates and times
 *
 *	wfvdate may be placed in a WFORM description table
 *      PARAMETERS: ptr to form description entry
 *		    ptr to buff containing user input.
 *			buff must be larg enough for mm/dd/yyyy = 11 bytes.
 *	RETURNS: 0 if date is valid.
 *
 *
 */

#include "wsys.h"


int wfvdate ( WFORM *form, char *entrybuff )
	{
	int retcode = wval_date ( entrybuff );	
	if ( retcode != 0 )
		{
		wform_showerror ( form, "Invalid date, should be mm/dd/yyyy" );
		}
	return (retcode);		/* wfvdate */
	}
/*------------------- end of WFVDATE.C -----------------*/