/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bops.c	1.2 - 89/10/31" */

#include <bool.h>
#include <errno.h>
/*#include <stddef.h>*/
/*#include <stdlib.h>*/
/*#include <string.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     b_alloc - allocate memory for block file

SYNOPSIS
     #include "blkio_.h"

     int b_alloc(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_alloc function allocates the memory needed by bp.  The
     memory is initialized to all zeros.  A call to b_alloc should
     normally be followed by a call to b_initlist to construct the
     linked list for LRU replacement.

     b_alloc will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [ENOMEM]       Enough memory is not available for
                    allocation by the calling process.
     [BENOPEN]      bp is not open.

SEE ALSO
     b_free, b_initlist.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_alloc(bp)
BLKFILE *bp;
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check for memory leak */
	if ((bp->block_p != NULL) || ((bp->blkbuf != NULL) && !(bp->flags & BIOUSRBUF))) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}
#endif

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		errno = 0;
		return 0;
	}

	/* allocate buffer storage (one extra block structure for header) */
	bp->block_p = (block_t *)calloc(bp->bufcnt + 1, sizeof(block_t));
	if (bp->block_p == NULL) {
		BEPRINT;
		errno = ENOMEM;
		return -1;
	}
	if (!(bp->flags & BIOUSRBUF)) {
		bp->blkbuf = calloc((size_t)1, bp->hdrsize + bp->bufcnt * bp->blksize);
		if (bp->blkbuf == NULL) {
			BEPRINT;
			free(bp->block_p);
			bp->block_p = NULL;
			errno = ENOMEM;
			return -1;
		}
	}

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_block_p - pointer to block structure

SYNOPSIS
     #include "blkio_.h"

     block_t *b_block_p(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     b_block_p returns a pointer to the ith block structure in the
     buffer list of block file bp.  If bp is not a valid open block
     file or i is not in the range [0..bp->bufcnt] the results are
     undefined.  b_block_p is a macro.

------------------------------------------------------------------------------*/
/* b_block_p is defined in blkio_.h */

/*man---------------------------------------------------------------------------
NAME
     b_blkbuf - pointer to block buffer

SYNOPSIS
     #include "blkio_.h"

     void *b_blkbuf(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     b_blkbuf returns a pointer to the ith buffer in the buffer list
     of block file bp.  A value of zero for i indicates the header
     buffer.  If bp is not a valid BLKFILE pointer to an open file or
     i is not in the range [0..bp->bufcnt] the results are undefined.
     b_blkbuf is a macro.

------------------------------------------------------------------------------*/
/* b_blkbuf is defined in blkio_.h */

/*man---------------------------------------------------------------------------
NAME
     b_free - free memory allocated for block file

SYNOPSIS
     #include "blkio_.h"

     void b_free(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_free function frees all memory allocated for block file bp.
     If bp has a user-supplied buffer storage area, it is disconnected
     from bp but is not freed.  On return from b_free, bp->block_p and
     bp->blkbuf will be NULL.

SEE ALSO
     b_alloc.

------------------------------------------------------------------------------*/
void b_free(bp)
BLKFILE *bp;
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		return;
	}
#endif

	/* free memory */
	if (!(bp->flags & BIOUSRBUF)) {		/* if not user-supplied buf */
		if (bp->blkbuf != NULL) {		/* free buffer */
			free(bp->blkbuf);
		}
	}
	bp->blkbuf = NULL;			/* disconnect buffer */
	if (bp->block_p != NULL) {
		free(bp->block_p);
		bp->block_p = NULL;
	}

	return;
}

/*man---------------------------------------------------------------------------
NAME
     b_get - get block from block file

SYNOPSIS
     #include "blkio_.h"

     int b_get(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     The b_get function reads a block from block file bp into the ith
     buffer for that block file.  b_get reads the block identified by
     the block number field of the block structure associated with
     buffer i (i.e., b_block_p(bp, i)->bn); the zeroth buffer is
     always used for the header.  The read flag is set and all others
     cleared for buffer i.

     b_get will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       i is not in the range [0..bp->bufcnt].
     [BEEOF]        End of file occured before end of block.
     [BENBUF]       bp is not buffered.

SEE ALSO
     b_put.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_get(bp, i)
BLKFILE *bp;
size_t i;
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if file is not buffered */
	if (bp->bufcnt == 0) {
		BEPRINT;
		errno = BENBUF;
		return -1;
	}

	/* validate arguments */
	if ((i > bp->bufcnt)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if block number set */
	if ((b_block_p(bp, i)->bn == 0) && (i != 0)) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}

	/* check if block needs to be written to disk */
	if (b_block_p(bp, i)->flags & BLKWRITE) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}
#endif

	/* read block from file */
	if (i == 0) {
		if (b_ugetf(bp, (bpos_t)0, (size_t)0, b_blkbuf(bp, (size_t)0), bp->hdrsize) == -1) {
			BEPRINT;
			return -1;
		}
	} else {
		if (b_ugetf(bp, b_block_p(bp, i)->bn, (size_t)0, b_blkbuf(bp, i), bp->blksize) == -1) {
			BEPRINT;
			return -1;
		}
	}

	/* set read flag and clear all others */
	b_block_p(bp, i)->flags = BLKREAD;

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_initlist - build linked list

SYNOPSIS
     #include "blkio_.h"

     int b_initlist(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_initlist function builds the linked list of buffers for
     block file bp.  The buffer contents are deleted in the process.
     A call to b_initlist should normally follow a call to b_alloc.

     b_initlist will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid block file.

SEE ALSO
     b_alloc.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_initlist(bp)
BLKFILE *bp;
{
	size_t i = 0;

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}
#endif

	/* initialize head and tail of list */
	bp->most = 0;
	bp->least = 0;

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		errno = 0;
		return 0;
	}

	/* initialize linked list */
	bp->most = bp->bufcnt;
	bp->least = 1;
	for (i = 1; i <= bp->bufcnt; i++) {
		b_block_p(bp, i)->bn = 0;
		b_block_p(bp, i)->flags = 0;
		b_block_p(bp, i)->more = i + 1;
		b_block_p(bp, i)->less = i - 1;
	}
	b_block_p(bp, bp->most)->more = 0;
	b_block_p(bp, bp->least)->less = 0;

	/* initialize block structure for header */
	b_block_p(bp, (size_t)0)->bn = 0;
	b_block_p(bp, (size_t)0)->flags = 0;
	b_block_p(bp, (size_t)0)->more = 0;
	b_block_p(bp, (size_t)0)->less = 0;

	/* scrub buffer storage area */
	memset(bp->blkbuf, 0, bp->hdrsize + bp->bufcnt * bp->blksize);

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_mkmru - make most recently used block

SYNOPSIS
     #include "blkio_.h"

     int b_mkmru(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     The b_mkmru function moves the ith block in the buffer list to
     the most recently used end of the buffer list.

     b_mkmru will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid block file.
     [EINVAL]       i is not in the range [1..bp->bufcnt].
     [BENBUF]       bp is not buffered.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_mkmru(bp, i)
BLKFILE *bp;
size_t   i;
{
	size_t more = 0;
	size_t less = 0;

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if file is not buffered */
	if (bp->bufcnt == 0) {
		BEPRINT;
		errno = BENBUF;
		return -1;
	}

	/* validate arguments */
	if ((i < 1) || (i > bp->bufcnt)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif

	/* check block addresses */
	more = b_block_p(bp, i)->more;
	less = b_block_p(bp, i)->less;
	if ((more > bp->bufcnt) || (less > bp->bufcnt)) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}

	/* check if already most recently used */
	if (more == 0) {
		errno = 0;
		return 0;
	}

	/* remove block i from linked list */
	b_block_p(bp, more)->less = less;
	if (less != 0) {
		b_block_p(bp, less)->more = more;
	} else {
		bp->least = more;
	}

	/* connect ith block as most recently used */
	b_block_p(bp, i)->more = 0;
	b_block_p(bp, i)->less = bp->most;
	b_block_p(bp, bp->most)->more = i;
	bp->most = i;

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_put - put block into block file

SYNOPSIS
     #include "blkio_.h"

     int b_put(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     The b_put function writes a block from the ith buffer of block
     file bp to the file.  b_put writes to the block identified by
     the block number field of the block structure associated with
     buffer i (i.e., b_block_p(bp, i)->bn); the zeroth buffer is
     always used for the header.  If the write flag is not set,
     nothing is written.  After writing, the write flag is cleared.

     b_put will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid block file.
     [EINVAL]       i is not in the range [0..bp->bufcnt].
     [BENBUF]       bp is not buffered.

SEE ALSO
     b_get.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_put(bp, i)
BLKFILE *bp;
size_t i;
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if file is not buffered */
	if (bp->bufcnt == 0) {
		BEPRINT;
		errno = BENBUF;
		return -1;
	}

	/* validate arguments */
	if (i > bp->bufcnt) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif

	/* check if block doesn't need to be written to disk */
	if (!(b_block_p(bp, i)->flags & BLKWRITE)) {
		errno = 0;
		return 0;
	}

	/* check for inconsistent read flag setting */
	if (!(b_block_p(bp, i)->flags & BLKREAD)) {
		BEPRINT;
		errno = BEPANIC;
		return 0;
	}

	/* write block to disk */
	if (i == 0) {
		if (b_uputf(bp, (bpos_t)0, (size_t)0, b_blkbuf(bp, (size_t)0), bp->hdrsize) == -1) {
			BEPRINT;
			return -1;
		}
	} else {
		if (b_uputf(bp, b_block_p(bp, i)->bn, (size_t)0, b_blkbuf(bp, i), bp->blksize) == -1) {
			BEPRINT;
			return -1;
		}
	}

	/* clear all but read flag */
	b_block_p(bp, i)->flags = BLKREAD;

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_valid - validate block file pointer

SYNOPSIS
     #include "blkio_.h"

     bool b_valid(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_valid function determines if bp is a valid BLKFILE pointer.
     If valid, the TRUE is returned.  If not, then FALSE is returned.

------------------------------------------------------------------------------*/
bool b_valid(bp)
CONST BLKFILE *bp;
{
	if ((bp < biob) || (bp > (biob + BOPEN_MAX - 1))) {
		return FALSE;
	}
	if (((char *)bp - (char *)biob) % sizeof(*biob) != 0) {
		return FALSE;
	}

	return TRUE;
}
