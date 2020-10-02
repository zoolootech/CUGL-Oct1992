/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bgethf.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stddef.h>*/
/*#include <string.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bgethf - get a header field from a block file

SYNOPSIS
     #include <blkio.h>

     int bgethf(bp, offset, buf, bufsize)
     BLKFILE *bp;
     size_t offset;
     void *buf;
     size_t bufsize;

DESCRIPTION
     The bgethf function reads a field from the header in the block
     file associated with BLKFILE pointer bp into buf.  The field
     begins offset characters from the beginning of the header and is
     bufsize characters long.  buf must point to a storage area at
     least bufsize characters long.

     bgethf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bn or bufsize is less than 1.
     [EINVAL]       buf is the NULL pointer.
     [EINVAL]       offset + bufsize extends beyond the
                    boundary of the header.
     [BEEOF]        bp is empty.
     [BEEOF]        End of file encountered within header.
     [BENOPEN]      bp is not open for reading.

SEE ALSO
     bgeth, bgetbf, bputhf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bgethf(bp, offset, buf, bufsize)
BLKFILE *bp;
size_t offset;
void *buf;
size_t bufsize;
{
	/* validate arguments */
	if (!b_valid(bp) || (buf == NULL) || (bufsize < 1)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open for reading */
	if (!(bp->flags & BIOREAD)) {
		errno = BENOPEN;
		return -1;
	}

	/* check if block boundary is crossed */
	if ((offset + bufsize) > bp->hdrsize) {
		errno = EINVAL;
		return -1;
	}

	/* check if header not yet written */
	if (bp->endblk < 1) {
		errno = BEEOF;
		return -1;
	}

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		if (b_ugetf(bp, (bpos_t)0, offset, buf, bufsize) == -1) {
			BEPRINT;
			return -1;
		}
		errno = 0;
		return 0;
	}

	/* check if buffer is not loaded */
	if (!(b_block_p(bp, (size_t)0)->flags & BLKREAD)) {
		b_block_p(bp, (size_t)0)->bn = 0; /* read header from file */
		if (b_get(bp, (size_t)0) == -1) {
			BEPRINT;
			return -1;
		}
	}

	/* copy from block buffer into buf */
	memcpy(buf, ((char *)b_blkbuf(bp, (size_t)0) + offset), bufsize);

	errno = 0;
	return 0;
}
