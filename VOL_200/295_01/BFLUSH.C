/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bflush.c	1.2 - 89/10/31" */

#include <errno.h>
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bflush - flush a block file

SYNOPSIS
     #include <blkio.h>

     int bflush(bp)
     BLKFILE *bp;

DESCRIPTION
     The bflush function causes any buffered data for the block file
     associated with BLKFILE pointer bp to be written to the file and
     the buffers to be emptied.  The header, if it has been modified,
     is written out last.  The block file remains open.  If  bp is
     open read-only or is not buffered, there will be no data to flush
     and bflush will return a value of zero immediately.

     bflush should be called immediately before a block file is
     unlocked.  lockb does this automatically.

     bflush will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [BENOPEN]      bp is not open.

SEE ALSO
     bexit, bputb, bsync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bflush(bp)
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

	/* check if not open for writing */
	if (!(bp->flags & BIOWRITE)) {
		errno = 0;
		return 0;
	}

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		errno = 0;
		return 0;
	}

	/* synchronize file with buffers */
	if (bsync(bp) == -1) {
		BEPRINT;
		return -1;
	}

	/* empty the buffers */
	if (b_initlist(bp) == -1) {
		BEPRINT;
		return -1;
	}

	errno = 0;
	return 0;
}
