/* WFVFILE.C
 *		form validation routine for file names
 *		allows wildcards (list of matching files will be displayed to user).
 *
 *		PARAMETERS: WFORM *form = ptr to form entry table for this data item
 *					char *buffer= ptr to filename.ext string, at least 13 bytes.
 *		RETURNS: 0 if filename is valid.
 *				 -1 if filename not valid.
 */
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>
#include "wtwg.h"
#include "dblib.h"
 
 
int 	wfvfile (WFORM *form, char *buffer)
	{
	int retcode = 0;
	struct stat statbuf;
	
	if ( strcspn (buffer, "*?") )
		{
		wildfile_pick ( buffer, 0 );
		}
	
	if (  ( *buffer == 0 ) || ( -1 == stat (buffer, &statbuf) ) )
		{
		wform_showerror ( form, "file not found" );
		retcode = -1;
		}
	return (retcode);	/* wfvfile() */
	}