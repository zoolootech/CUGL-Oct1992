/* WMSFLUSH.C - flush mouse of keys held down.
 */
#include "wsys.h"

#define ANY_PRESS (WMS_LEFT_PRS | WMS_RIGHT_PRS | WMS_CENTER_PRS) 

void wmouse_flush (void)
	{
	
	do 
		{
		wmouse.wms_internal =0;
		wmouse_location ();
		}
	while ( 0 != (wmouse.wms_used && ANY_PRESS) );

	return;		/* wmouse_flush() */
	}
