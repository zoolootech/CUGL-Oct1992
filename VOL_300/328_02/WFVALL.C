/* WFORMCHK.C
 *	This file contains wfvall
 *
 *      This function can be named in the header line of a WFORM table
 *	It loops through the table and calls all the data validation funcs
 *	If any data is invalid, it returns (1) else (0)
 *
 *
 */
#include "wsys.h"

int wfvall (WFORM *form, char *buffer)
	{
	int rc =0;

	while  ( (form)->wflabel != NULL )
		{

		if ( form->wfvalidate != NULL )
			{
			rc += (*form->wfvalidate)(form, buffer);
			}

		/* move to next spot in buffer
		 * ( data for next form line
		 */
		buffer += form->wflen;


		++form;
		}


	return (rc);	/* wfvall */
	}


/*-------------------- end WFORMCHK.C ------------------------------*/