// WILDPP.CPP	generate a Vlist of file names from a wildcard.
// 				PARM: spec = ptr to filename wildcards. Includes path
//					  attr = file attribute mask, see TurboC++ findfirst()
//				RETURNS: ptr to a new Vlist, or empty list if not found.
#include <stdlib.h>
#include <dir.h>
#include <dos.h>
#include <string.h>

#include "wtwg.h"


#include "dblib.h"

Vlist  *wildfile ( char * wc, int file_attr )
	{
	char far *dta;
	struct ffblk ffb;
	int done;
	Vlist *vl = new Vlist;

	dta = getdta();
	for (done= findfirst (wc,&ffb,file_attr); !done;	done= findnext (&ffb) )
			{
			vl->push ( ffb.ff_name );
			}
	setdta (dta);
	return (vl);		// wildfile_pp
	}
	
	

//----------------- end of WILDFILE.CPP
