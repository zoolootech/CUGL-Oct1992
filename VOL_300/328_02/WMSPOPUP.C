/*  WMSPOPUP.C
 *		install a function to be executed 
 *		whenever the mouse center button is pressed
 *		the function can return a new keyvalue which is inserted into the
 *		keyboard stream ( ie: allow selection of a function key by mouse )
 */	
#include "wsys.h"


static int installed = 0;
static int reentrant = 0;
static int handler ( int );
static int (*old_handler) (int) = NULL;
static int (*save_func) (void) = NULL;

void wmspopup ( int (*func)(void) )
	{
	if ( ! installed ) 
		{
		installed = 1;
		old_handler = wkeytrap;
		wkeytrap 	= handler; 
		}
	save_func   = func;
	return;	/* wmspopup () */
	}	
	
	
static int handler ( int key )
	{
	if ( key == MOUSE && ( wmouse.wms_used & WMS_CENTER_PRS ) 
		&& ! reentrant && save_func != NULL  )
		{
		reentrant  =1;
		key = (*save_func)();
		reentrant = 0;
		}
		
	/*	chain to any other key handlers
	 */
	if ( old_handler != NULL )
		{
		key = old_handler (key );
		}
	return ( key );			/* end of handler */
	}	
/*---------------- end of WMSPOPUP.C -----------------*/	
