/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bsetbuf.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stddef.h>*/
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bsetbuf - assign buffering to a block file

SYNOPSIS
     #include <blkio.h>

     int bsetbuf(bp, buf)
     BLKFILE *bp;
     void *buf;

DESCRIPTION
     The bsetbuf function causes the storage area pointed to by buf to
     be used by the block file associated with BLKFILE pointer bp
     instead of an automatically allocated buffer area.  If buf is the
     NULL pointer, bp will be completely unbuffered.  Otherwise, it
     must point to a storage area of size no less than

          header size + block size * buffer count

     bsetbuf may be called at any time after opening the block file,
     before and after it is read or written; the buffers are flushed
     before installing the new buffer area.

     bsetbuf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [BENBUF]       bp is unbuffered and buf is not the NULL
                    pointer.
     [BENOPEN]      bp is not open.

SEE ALSO
     bopen, bsetvbuf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int bsetbuf(bp, buf)
BLKFILE *bp;
void *buf;
{
	/* validate arguments */
	if (!b_valid(bp)) {
		errno = EINVAL;
		return -1;
	}
	if ((bp->bufcnt == 0) && (buf != NULL)) {
		errno = BENBUF;
		return -1;
	}

	if (buf == NULL) {
		return bsetvbuf(bp, buf, bp->blksize, (size_t)0);
	}

	return bsetvbuf(bp, buf, bp->blksize, bp->bufcnt);
}
