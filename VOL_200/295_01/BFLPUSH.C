/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bflpush.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stddef.h>*/
/*#include <string.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bflpush - push block onto free list

SYNOPSIS
     #include <blkio.h>

     int bflpush(bp, bn_p)
     BLKFILE *bp;
     const bpos_t *bn_p;

DESCRIPTION
     The bflpush function puts the block number pointed to by bn_p
     into the free list of the block file associated with BLKFILE
     pointer bp.  If the value pointed to by bn_p is one block past
     the end of the file, it is accepted but not actually added to the
     list.

     bflpush will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bn_p is the NULL pointer.
     [EINVAL]       bn_p points to a value less than 1.
     [BEEOF]        bp is empty.
     [BEEOF]        bn_p points to a value farther than
                    one block past the end of file.
     [BENFL]        bp does not have a free list.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     bflpop.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

NOTES
     To use the free list functions in the blkio library, the first
     element of the file header must be the free list head with type
     bpos_t.  This must be initialized to 0 immediately after the file
     is created and not accessed afterward except using bflpush and
     bflpush.  Also, the block size must be at least sizeof(bpos_t).

------------------------------------------------------------------------------*/
int bflpush(bp, bn_p)
BLKFILE *bp;
CONST bpos_t *bn_p;
{
	bpos_t oldflhno = 0;
	bpos_t newflhno = 0;
	void *buf = NULL;

	/* validate arguments */
	if (!b_valid(bp) || (bn_p == NULL)) {
		errno = EINVAL;
		return -1;
	}
	if (*bn_p < 1) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOWRITE)) {
		errno = BENOPEN;
		return -1;
	}

	/* check if no free list */
	if ((bp->hdrsize < sizeof(oldflhno)) || (bp->blksize < sizeof(oldflhno))) {
		errno = BENFL;
		return -1;
	}

	/* check if header not yet written */
	if (bp->endblk < 1) {
		errno = BEEOF;
		return -1;
	}

	/* check if block past endblk */
	if (*bn_p > bp->endblk) {
		errno = BEEOF;
		return -1;
	}

	/* check if block is endblk */
	if (*bn_p == bp->endblk) {
		errno = 0;
		return 0;
	}

	/* get block number of current free list head */
	if (bgethf(bp, (size_t)0, &oldflhno, sizeof(oldflhno)) == -1) {
		BEPRINT;
		return -1;
	}

	/* link to rest of free list */
	newflhno = *bn_p;
	buf = calloc((size_t)1, bp->blksize);
	if (buf == NULL) {
		BEPRINT;
		errno = ENOMEM;
		return -1;
	}
	memcpy(buf, &oldflhno, sizeof(oldflhno));
	if (bputb(bp, newflhno, buf) == -1) {
		BEPRINT;
		free(buf);
		buf = NULL;
		return -1;
	}
	free(buf);
	buf = NULL;

	/* write new free list head number to header */
	if (bputhf(bp, (size_t)0, &newflhno, sizeof(newflhno)) == -1) {
		BEPRINT;
		return -1;
	}

	errno = 0;
	return 0;
}
