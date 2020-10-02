/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bputb.c	1.2 - 89/10/31" */

/*#include <stddef.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bputb - put a block into a block file

SYNOPSIS
     #include <blkio.h>

     int bputb(bp, bn, buf)
     BLKFILE *bp;
     bpos_t bn;
     const void *buf;

DESCRIPTION
     The bputb function writes the block pointed to by buf into block
     number bn of the block file associated with BLKFILE pointer bp.
     buf must point to a storage area at least as large as the block
     size for bp.

     bputb will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bn is less than 1.
     [EINVAL]       buf is the NULL pointer.
     [BEEOF]        There are fewer than bn - 1 blocks in
                    the file.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     bgetb, bputbf, bputh.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bputb(bp, bn, buf)
BLKFILE *bp;
bpos_t bn;
CONST void *buf;
{
	return bputbf(bp, bn, (size_t)0, buf, bp->blksize);
}
