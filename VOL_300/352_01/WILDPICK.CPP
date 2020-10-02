// WILDPICK.CPP	   routine to allow user to select a file from a list
//					the list is generated from a wildcard spec.
// 				PARM: spec =ptr to filename wildcards. may include drive & path
//					 attr  = file attribute mask, see TurboC++ findfirst()
//				RETURNS: void
//				SIDE EFFECT: the string indicated by parm 1 (spec) is replaced.
//							 the new contents are the selected file name
//							 (which may be up to 8+1+3+1 bytes).
//							 the drive:path name is left out.
//							 stirng set to 0 if no file selected.
//				example:    char  wildcard[8+1+3+1] = "*.cpp";
//							wildfile_pick ( wildcard, 0 );
//							... wildcard now contains name of file selected.
//
#include <stdlib.h>
#include <dir.h>
#include <dos.h>
#include <string.h>

#include "wtwg.h"
#include "dblib.h"

void  wildfile_pick (char *spec, int attr   )
	{
	int npick;
	char  *choice;


	Vlist *vl = wildfile (spec, attr);		// make list of matching file

	if ( vl->isEmpty() )
		{
		wpromptc ( spec, "ABOVE FILE NOT FOUND", NULL );
		spec[0]= 0;			/* null in caller area */
		}
	else
		{
		npick =wpicklist ( spec, (char**) (*vl) );		// NOTE cast to array.
		choice = (char *)  ( (*vl)[npick] );			// pick up string ptr.
		if ( choice == NULL )
			{
			spec[0] =0;	
			} 
		else
			{
			strcpy ( spec, choice );
			}
		}
		
	delete vl;	// delete vlist created by wildfile().
	
	return;		// wildfile_pick()
	}

//----------------- end of WILDPICK.CPP
