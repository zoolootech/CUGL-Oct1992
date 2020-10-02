/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bsync.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stddef.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bsync - synchronize a block file

SYNOPSIS
     #include <blkio.h>

     int bsync(bp)
     BLKFILE *bp;

DESCRIPTION
     The bsync function causes the block file associated with BLKFILE
     pointer bp to be synchronized with its buffers; any buffered data
     which has been written to the buffers is written to the file.
     The header, if it has been modified, is written out last.  The
     block file remains open and the buffers retain their contents.
     If bp is open read-only or is not buffered, there will be no data
     to synchronize and bsync will return a value of zero immediately.

     bsync will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [BENOPEN]      bp is not open.

SEE ALSO
     bexit, bflush, bputb.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bsync(bp)
BLKFILE *bp;
{
	int i = 0;
#ifdef DEBUG
	bpos_t endblk = 0;
#endif

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

	/* synchronize block in each buffer */
	for (i = 1; i <= bp->bufcnt; i++) {
		if (b_put(bp, (size_t)i) == -1) {
			BEPRINT;
			return -1;
		}
	}

	/* synchronize header */
	if (b_put(bp, (size_t)0) == -1) {
		BEPRINT;
		return -1;
	}

#ifdef DEBUG
	/* check block count */
	if (b_uendblk(bp, &endblk) == -1) {
		BEPRINT;
		return -1;
	}
	if (bp->endblk != endblk) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}
#endif

	errno = 0;
	return 0;
}
