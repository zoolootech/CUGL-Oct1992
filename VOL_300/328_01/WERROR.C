/*! werror
 *	fatal error handler
 */

#include "wscreen.h"
#include "wsys.h"



#include <stdio.h>


void	werror ( int errcode, char *msg )
	{
	#ifndef TEXTONLY
		/* if in graphics mode, need to restore CRT
		 */
                 wtextmode ();
	#endif



	perror ( msg );

	exit (errcode);

	return; /* werror */
	}
