/* WFVPATH.C
 *		form entry function to validate data paths
 *		may be either NULL or C:\path\
 *		must end in '\\'
 *		specified path must exist 
 */
#include <stdlib.h>

#include "wtwg.h"
#include "dblib.h"


int wfvpath (WFORM *fm, char *buf)
	{
	int retcode;
	retcode = validate_path ( buf );
	if ( -1 == retcode )
		{
		wform_showerror ( fm, "Path not found" );
		}
	return (retcode);
	}
	
	
	/*----------------------- WFVPATH.C -----------------------------*/
	