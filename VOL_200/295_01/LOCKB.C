/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"lockb.c	1.2 - 89/10/31" */

/*#include <stddef.h>*/
#include "blkio_.h"

#include <errno.h>
#if HOST == UNIX
#include <fcntl.h>
int fcntl(/*int fd, int cmd, ...*/);
#elif HOST == MSDOS

#endif

/*man---------------------------------------------------------------------------
NAME
     lockb - block file record locking

SYNOPSIS
     #include <blkio.h>

     int lockb(bp, ltype, start, len)
     BLKFILE *bp;
     int ltype;
     bpos_t start;
     bpos_t len;

DESCRIPTION
     The lockb function will allow segments of a block file to be
     locked.  bp is the BLKFILE pointer for the file to be locked.

     ltype indicates the target status of the lock.  The lock types
     available are:

       B_UNLCK unlock block file segment
       B_RDLCK lock block file segment for reading
       B_WRLCK lock block file segment for reading and writing
       B_RDLKW lock block file segment for reading (wait)
       B_WRLKW lock block file segment for reading and writing (wait)

     For the lock types which wait, lockb will not return until the
     lock is available.  For the lock types which do not wait, if the
     lock is unavailable because of a lock held by another process  a
     value of -1 is returned and errno set to EAGAIN.

     start is the first block to lock.  len is the number of
     contiguous blocks including and following block start to be
     locked or unlocked.

     The buffers are flushed before unlocking.

     lockb will fail if one or more of the following is true:

     [EAGAIN]       ltype is B_RDLCK and the file segment
                    to be locked is already write locked
                    by another process, or ltype is B_WRLCK
                    and the file segment to be locked is
                    already read or write locked by another
                    process.
     [EINVAL]       bp is is not a valid BLKFILE pointer.
     [EINVAL]       ltype is not one of the valid lock
                    types.
     [BENOPEN]      bp is not open.
     [BENOPEN]      ltype is B_RDLCK or B_RDLKW and bp is
                    not opened for reading or ltype is
                    B_WRLCK or B_WRLKW and bp is not open
                    for writing.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int lockb(bp, ltype, start, len)
BLKFILE *bp;
int ltype;
bpos_t start;
bpos_t len;
{
#if HOST == UNIX
	int cmd = 0;
	struct flock lck;
#elif HOST == MSDOS

#endif

	/* validate arguments */
	if (!b_valid(bp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		errno = BENOPEN;
		return -1;
	}

	/* set lock flags */
	switch (ltype) {
	case B_RDLCK:
		if (!(bp->flags & BIOREAD)) {
			errno = BENOPEN;
			return -1;
		}
#if HOST == UNIX
		cmd = F_SETLK;
		lck.l_type = F_RDLCK;
#elif HOST == MSDOS

#endif
		break;
	case B_RDLKW:
		if (!(bp->flags & BIOREAD)) {
			errno = BENOPEN;
			return -1;
		}
#if HOST == UNIX
		cmd = F_SETLKW;
		lck.l_type = F_RDLCK;
#elif HOST == MSDOS

#endif
		break;
	case B_WRLCK:
		if (!(bp->flags & BIOWRITE)) {
			errno = BENOPEN;
			return -1;
		}
#if HOST == UNIX
		cmd = F_SETLK;
		lck.l_type = F_WRLCK;
#elif HOST == MSDOS

#endif
		break;
	case B_WRLKW:
		if (!(bp->flags & BIOWRITE)) {
			errno = BENOPEN;
			return -1;
		}
#if HOST == UNIX
		cmd = F_SETLKW;
		lck.l_type = F_WRLCK;
#elif HOST == MSDOS

#endif
		break;
	case B_UNLCK:
		/* flush buffers */
		if (bflush(bp) == -1) {
			BEPRINT;
			return -1;
		}
#if HOST == UNIX
		cmd = F_SETLK;
		lck.l_type = F_UNLCK;
#elif HOST == MSDOS

#endif
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	/* lock block file */
#if HOST == UNIX
	if (start == 0) {
		lck.l_whence = 0;
	} else {
		lck.l_whence = bp->hdrsize + (start - 1) * bp->blksize;
	}
	lck.l_start = 0;
	lck.l_len = bp->blksize * len;
	lck.l_sysid = 0;
	lck.l_pid = 0;
	if (fcntl(bp->fd.ifd, cmd, &lck) == -1) {
		/* new versions of fcntl will use EAGAIN */
		if (errno == EACCES) errno = EAGAIN;
		if (errno != EAGAIN) BEPRINT;
		return -1;
	}
#elif HOST == MSDOS

#endif

	errno = 0;
	return 0;
}
