/* VLDTPATH.C - contains validate_path()
 *		general purpos function to validate data paths
 *		PARM may be either NULL or C:\path\
 *		specified path must exist and not be empty.
 *		RETURNS: -1 if not valid, 0 if valid.
 */
#include <stdlib.h>
#include <string.h>
#include <dir.h>

#include "dblib.h"


int validate_path ( char *path )
	{	
	int retcode = 0;
	char far *savedta; 
	struct ffblk ffb;

	String *fullnm = make_fullfilename ( path, "*.*" );
	
	savedta = getdta ();
	if ( -1 == findfirst ( (*fullnm), &ffb, FA_DIREC ) )
		{
		errno =0;
		retcode = -1;
		}
	delete fullnm;	
	setdta (savedta);
	return (retcode);
	}
	
	
	/*----------------------- VLDTPATH.CPP -----------------------------*/
		
	
	
	
