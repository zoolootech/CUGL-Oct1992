/* wfvtime.c
 *	form entry, validate time fields
 */
#include "wsys.h"

#define BUFFSZ	9



int wfvtime ( WFORM *formentry, char *entrybuff )
	{
	int retcode =wval_time (entrybuff);


	if ( retcode != 0 )
		{
		wform_showerror (formentry, "Invalid time hh:mm:ss");
		}

	return ( retcode );
	}



/*--------------------- end WFVTIME.C ----------------------*/