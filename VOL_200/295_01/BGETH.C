/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bgeth.c	1.2 - 89/10/31" */

/*#include <stddef.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bgeth - get the header from a block file

SYNOPSIS
     #include <blkio.h>

     int bgeth(bp, buf)
     BLKFILE *bp;
     void *buf;

DESCRIPTION
     The bgeth function reads the header from the block file
     associated with BLKFILE pointer bp.  buf must point to a storage
     area at least as large as the header size for bp.

     bgeth will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid block file.
     [EINVAL]       buf is NULL.
     [BEEOF]        bp is empty.
     [BEEOF]        End of file encountered within header.
     [BENOPEN]      bp is not open for reading.

SEE ALSO
     bgetb, bgethf, bputh.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bgeth(bp, buf)
BLKFILE *bp;
void *buf;
{
	return bgethf(bp, (size_t)0, buf, bp->hdrsize);
}
