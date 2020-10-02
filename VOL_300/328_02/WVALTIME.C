/* wvaltime.c
 *	validate a time string in form of hh:mm:ss or hh:mm
 *
 *	RETURNS: 0 if time is valid. nonzero otherwise.
 *			alters the time string by removing extra digits, sapces, etc...
 *
 *		adds missing 'ss' field if not provided originally.
 *			(ie: buffer must be at least 9 bytes.)
 *
 */
#include "wsys.h"


int wval_time ( char *time )
	{
	int retcode =1;
	unsigned int h=100,m=100, s=0;	/* NOTE: h,m req'd by default setting */
	char *ptr = time;
		
	wdtparse ( &h, &ptr, ':' );
	wdtparse ( &m, &ptr, ':' );
	wdtparse ( &s, &ptr, ':' );
	
	if 	(   h<24  && m<60 && s<60 )   
		{
		retcode = 0;
		sprintf ( time, "%u:%u:%2.2u", h,m,s);
		}
	

	return (retcode);		/* wval_time ()*/
	}
	
/*--------------- end of WVALTIME.C --------------------*/