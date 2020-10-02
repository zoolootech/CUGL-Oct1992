/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bgetbf.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stddef.h>*/
/*#include <string.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bgetbf - get a block field from a block file

SYNOPSIS
     #include <blkio.h>

     int bgetbf(bp, bn, offset, buf, bufsize)
     BLKFILE *bp;
     bpos_t bn;
     size_t offset;
     void *buf;
     size_t bufsize;

DESCRIPTION
     The bgetbf function reads a field from block number bn in the
     block file associated with BLKFILE pointer bp into buf.  The
     field begins offset characters from the beginning of the block
     and is bufsize characters long.  buf must point to a storage area
     at least bufsize characters long.  Block numbering starts at 1.

     bgetbf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bn or bufsize is less than 1.
     [EINVAL]       buf is the NULL pointer.
     [BEBOUND]      offset + bufsize extends beyond the
                    boundary of block bn.
     [BEEOF]        There are not bn blocks in the file.
     [BEEOF]        End of file encountered within block bn.
     [BENOPEN]      bp is not open for reading.

SEE ALSO
     bgetb, bgethf, bputbf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bgetbf(bp, bn, offset, buf, bufsize)
BLKFILE *bp;
bpos_t bn;
size_t offset;
void *buf;
size_t bufsize;
{
	int i = 0;
	size_t bufno = 0;

	/* validate arguments */
	if (!b_valid(bp) || (bn < 1) || (buf == NULL) || (bufsize < 1)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open for reading */
	if (!(bp->flags & BIOREAD)) {
		errno = BENOPEN;
		return -1;
	}

	/* check if block boundary is crossed */
	if ((offset + bufsize) > bp->blksize) {
		errno = BEBOUND;
		return -1;
	}

	/* check if not bn blocks in file */
	if (bn >= bp->endblk) {
		errno = BEEOF;
		return -1;
	}

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		if (b_ugetf(bp, bn, offset, buf, bufsize) == -1) {
			BEPRINT;
			return -1;
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
		/* read block from file */
		if (b_get(bp, bufno) == -1) {
			BEPRINT;
			return -1;
		}
	}

	/* copy from block buffer into buf */
	memcpy(buf, ((char *)b_blkbuf(bp, bufno) + offset), bufsize);

	/* move block buffer bufno to most recently used end of list */
	if (b_mkmru(bp, bufno) == -1) {
		BEPRINT;
		return -1;
	}

	errno = 0;
	return 0;
}
