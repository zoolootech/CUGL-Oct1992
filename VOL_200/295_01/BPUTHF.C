/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bputhf.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stddef.h>*/
/*#include <string.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bputhf - put a header field into a block file

SYNOPSIS
     #include <blkio.h>

     int bputhf(bp, offset, buf, bufsize)
     BLKFILE *bp;
     size_t offset;
     const void *buf;
     size_t bufsize;

DESCRIPTION
     The bputhf function writes the contents of buf into a field of
     the header of the block file associated with BLKFILE pointer bp.
     The field begins offset characters from the beginning of the
     header and is bufsize characters long.  buf must point to a
     storage area at least bufsize characters long.

     bputhf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bufsize is less than 1.
     [EINVAL]       buf is the NULL pointer.
     [BEBOUND]      offset + bufsize extends beyond the
                    boundary of the header.
     [BEEOF]        Attempt to write a field before the
                    complete header has been written.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     bgethf, bputh, bputbf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

NOTES
     When the header is created,  the entire header must be written,
     so offset must have a value of 0 and bufsize must be equal to the
     block size.

------------------------------------------------------------------------------*/
int bputhf(bp, offset, buf, bufsize)
BLKFILE *bp;
size_t offset;
CONST void *buf;
size_t bufsize;
{
	/* validate arguments */
	if (!b_valid(bp) || (buf == NULL) || (bufsize < 1)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open for reading */
	if (!(bp->flags & BIOWRITE)) {
		errno = BENOPEN;
		return -1;
	}

	/* check if block boundary is crossed */
	if ((offset + bufsize) > bp->hdrsize) {
		errno = BEBOUND;
		return -1;
	}

	/* check if incomplete header */
	if ((offset != 0) || (bufsize != bp->hdrsize)) {
		if (bp->endblk < 1) {
			errno = BEEOF;
			return -1;
		}
	}

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		if (b_uputf(bp, (bpos_t)0, offset, buf, bufsize) == -1) {
			BEPRINT;
			return -1;
		}
		if (bp->endblk < 1) {
			bp->endblk = 1;
		}
		errno = 0;
		return 0;
	}

	/* check if buffer is not loaded */
	if (!(b_block_p(bp, (size_t)0)->flags & BLKREAD)) {
		if ((offset != 0) || (bufsize != bp->hdrsize)) {
			if (b_get(bp, (size_t)0) == -1) {	/* read block from file */
				if (errno != BEEOF) BEPRINT;
				return -1;
			}
		}
	}

	/* copy from buf into block buffer and set flags */
	memcpy(((char *)b_blkbuf(bp, (size_t)0) + offset), buf, bufsize);
	b_block_p(bp, (size_t)0)->flags = BLKREAD | BLKWRITE;

	/* adjust endblk */
	if (bp->endblk < 1) {
		bp->endblk = 1;
	}

	errno = 0;
	return 0;
}
