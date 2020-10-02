/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bputbf.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stddef.h>*/
/*#include <string.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bputbf - put a block field into a block file

SYNOPSIS
     #include <blkio.h>

     int bputbf(bp, bn, offset, buf, bufsize)
     BLKFILE *bp;
     bpos_t bn;
     size_t offset;
     const void *buf;
     size_t bufsize;

DESCRIPTION
     The bputbf function writes the contents of buf into a field of
     block number bn of the block file associated with BLKFILE pointer
     bp.  The field begins offset characters from the beginning of the
     block and is bufsize characters long.  buf must point to a
     storage area at least bufsize characters long.  Block numbering
     starts at 1.

     bputbf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bn or bufsize is less than 1.
     [EINVAL]       buf is the NULL pointer.
     [BEBOUND]      offset + bufsize extends beyond the
                    boundary of block bn.
     [BEEOF]        Partial block being written and block
                    bn is past the end of file.
     [BEEOF]        Complete block being written and block
                    bn is more than 1 past the end of file.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     bgetbf, bputb, bputhf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

NOTES
     Whenever a new block is created at the end of the file, the
     entire block must be written, i.e., offset must have a value of 0
     and bufsize must be equal to the block size.

------------------------------------------------------------------------------*/
int bputbf(bp, bn, offset, buf, bufsize)
BLKFILE *bp;
bpos_t bn;
size_t offset;
CONST void *buf;
size_t bufsize;
{
	int i = 0;
	size_t bufno = 0;

	/* validate arguments */
	if (!b_valid(bp) || (bn < 1) || (buf == NULL) || (bufsize < 1)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open for writing */
	if (!(bp->flags & BIOWRITE)) {
		errno = BENOPEN;
		return -1;
	}

	/* check if block boundary is crossed */
	if ((offset + bufsize) > bp->blksize) {
		errno = BEBOUND;
		return -1;
	}

	/* check if past end of file */
	if ((offset != 0) || (bufsize != bp->blksize)) {
		if (bn >= bp->endblk) {
			errno = BEEOF;
			return -1;
		}
	} else {
		if (bn > bp->endblk) {
			errno = BEEOF;
			return -1;
		}
	}

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		if (b_uputf(bp, bn, offset, buf, bufsize) == -1) {
			BEPRINT;
			return -1;
		}
		if (bp->endblk <= bn) {
			bp->endblk = bn + 1;
		}
		errno = 0;
		return 0;
	}

	/* search buffer list for block */
	for (i = 1; i <= bp->bufcnt; i++) {
		if ((b_block_p(bp, (size_t)i)->bn == bn)
			       && (b_block_p(bp, (size_t)i)->flags & BLKREAD)) {
			bufno = i;
			break;
		}
	}

	/* if not found, use least recently used buffer */
	if (bufno == 0) {
		bufno = bp->least;
		if (b_put(bp, bufno) == -1) {	/* flush previous contents */
			BEPRINT;
			return -1;
		}
		b_block_p(bp, bufno)->flags = 0;
		b_block_p(bp, bufno)->bn = bn;
		if ((offset != 0) || (bufsize != bp->blksize)) {
			/* read block from file */
			if (b_get(bp, bufno) == -1) {
				if (errno != BEEOF) BEPRINT;
				return -1;
			}
		}
	}

	/* copy from buf into block buffer and set flags */
	memcpy(((char *)b_blkbuf(bp, bufno) + offset), buf, bufsize);
	b_block_p(bp, bufno)->flags = BLKREAD | BLKWRITE;

	/* adjust endblk */
	if (bp->endblk <= bn) {
		bp->endblk = bn + 1;
	}

	/* move block buffer bufno to most recently used end of list */
	if (b_mkmru(bp, bufno) == -1) {
		BEPRINT;
		return -1;
	}

	errno = 0;
	return 0;
}
