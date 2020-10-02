/* wflush ()
 *
 *	this routine flushes the keyboard/mouse
 */

#include "wsys.h"


void wflush (void)
	{

	if  (wpipein)
		{
		/* redirection is in effect - don't flush
		 */
		return;
		}

	while ( wready_kbd() )
		{
		wread_kbd();
		}

	wmouse_location();
	wmouse.wms_used = wmouse.wms_internal= 0;



	return;		/*wflush*/
	}