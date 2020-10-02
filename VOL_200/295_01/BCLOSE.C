/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bclose.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <string.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bclose - close a block file

SYNOPSIS
     #include <blkio.h>

     int bclose(bp)
     BLKFILE *bp;

DESCRIPTION
     The bclose function causes any buffered data for the block file
     associated with BLKFILE pointer bp to be written out, and the
     block file to be closed.

     bclose will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [BENOPEN]      bp is not open.

SEE ALSO
     bexit, bopen, bsync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bclose(bp)
BLKFILE *bp;
{
	/* validate arguments */
	if (!b_valid(bp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		errno = BENOPEN;
		return -1;
	}

	/* synchronize file with buffers */
	if (bsync(bp) == -1) {
		BEPRINT;
		return -1;
	}

	/* close file */
	if (b_uclose(bp) == -1) {
		BEPRINT;
		return -1;
	}

	/* free memory allocated for block file */
	b_free(bp);

	/* scrub slot in biob table then free it */
	memset(bp, 0, sizeof(*biob));
	bp->flags = 0;

	errno = 0;
	return 0;
}
