/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bgetb.c	1.2 - 89/10/31" */

/*#include <stddef.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bgetb - get a block from a block file

SYNOPSIS
     #include <blkio.h>

     int bgetb(bp, bn, buf)
     BLKFILE *bp;
     bpos_t bn;
     void *buf;

DESCRIPTION
     The bgetb function reads block number bn from the block file
     associated with BLKFILE pointer bp.  buf must point to a storage
     area at least as large as the block size for bp.  Block numbering
     starts at 1.

     bgetb will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bn is less than 1.
     [EINVAL]       buf is the NULL pointer.
     [BEEOF]        There are not bn blocks in the file.
     [BENOPEN]      bp is not open for reading.

SEE ALSO
     bgetbf, bgeth, bputb.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bgetb(bp, bn, buf)
BLKFILE *bp;
bpos_t bn;
void *buf;
{
	return bgetbf(bp, bn, (size_t)0, buf, bp->blksize);
}
