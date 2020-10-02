/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bexit.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stdlib.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bexit - block file exit

SYNOPSIS
     #include <blkio.h>

     void bexit(status)
     int status;

DESCRIPTION
     The bexit function is for use with the blkio library in place of
     exit.  It closes all open block files, which writes the contents
     of the buffers to the files, then calls exit.

SEE ALSO
     bclose.

------------------------------------------------------------------------------*/
void bexit(status)
int status;
{
	BLKFILE *bp = NULL;

	/* close all open block files */
	for (bp = biob; bp < (biob + BOPEN_MAX); bp++) {
		if (bp->flags & BIOOPEN) {
			if (bclose(bp) == -1) {
				BEPRINT;
			}
		}
	}

	exit(status);
}
