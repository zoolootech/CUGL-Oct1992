/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"bopen.c	1.2 - 89/10/31" */

#include <errno.h>
/*#include <stddef.h>*/
/*#include <string.h>*/
#include "blkio_.h"

/* block file control structure table definition */
BLKFILE biob[BOPEN_MAX];

/*man---------------------------------------------------------------------------
NAME
     bopen - open a block file

SYNOPSIS
     #include <blkio.h>

     BLKFILE *bopen(filename, type, hdrsize, blksize, bufcnt)
     const char *filename;
     const char *type;
     size_t hdrsize;
     size_t blksize;
     size_t bufcnt;

DESCRIPTION
     The bopen function opens the file named by filename as a block
     file.  A pointer to the BLKFILE structure associated with file is
     returned.

     filename points to a character string that contains the name of
     the file to be opened.

     type is a character string having one of the following values:

          "r"            open for reading
          "r+"           open for update (reading and writing)
          "w+"           truncate or create for update
          "c"            create for update

     If type is "r" or "r+" and the file does not exist, bopen will
     fail.  If type is "c" and the file already exists, bopen will
     fail.

     hdrsize is the size of the file header.  If there is no file
     header, specify a value of 0 for hdrsize.

     blksize is the size of the blocks.

     bufcnt is the number of blocks to for which to create buffer
     storage.  For unbuffered operation, specify a value of 0 for
     bufcnt.

     bopen will fail if one or more of the following is true:

     [EEXIST]       type is "c" and the named file exists.
     [EINVAL]       filename or type is the NULL pointer.
     [EINVAL]       type is not "r", "r+", "w+", or "c".
     [EINVAL]       blksize is 0.
     [ENOENT]       type is "r" or "r+" and the named file
                    does not exist.
     [BEMFILE]      The maximum number of block files is
                    already open.

SEE ALSO
     bclose.

DIAGNOSTICS
     bopen returns a NULL pointer on failure, and errno is set to
     indicate the error.

NOTES
     If the file being opened does not end on a block boundary, the
     trailing partial block is ignored; blkio considers the end of the
     last complete block as the end of the file.

------------------------------------------------------------------------------*/
BLKFILE *bopen(filename, type, hdrsize, blksize, bufcnt)
CONST char *filename;
CONST char *type;
size_t hdrsize;
size_t blksize;
size_t bufcnt;
{
	BLKFILE *bp = NULL;
	int terrno = 0;

	/* validate arguments */
	if ((filename == NULL) || (type == NULL) || (blksize == 0)) {
		errno = EINVAL;
		return NULL;
	}

	/* find free slot in biob table */
	for (bp = biob; bp < (biob + BOPEN_MAX); bp++) {
		if (!(bp->flags & BIOOPEN)) {
			break;		/* found */
		}
	}
	if (bp > (biob + BOPEN_MAX - 1)) {
		errno = BEMFILE;
		return NULL;		/* no free slots */
	}

	/* set biob flags */
	if (strcmp(type, BF_READ) == 0) {
		bp->flags = BIOREAD;
	} else if (strcmp(type, BF_RDWR) == 0) {
		bp->flags = BIOREAD | BIOWRITE;
	} else if (strcmp(type, BF_CREATE) == 0) {
		bp->flags = BIOREAD | BIOWRITE;
	} else if (strcmp(type, BF_CRTR) == 0) {
		bp->flags = BIOREAD | BIOWRITE;
	} else {
		errno = EINVAL;
		return NULL;
	}

	/* open file */
	if (b_uopen(bp, filename, type) == -1) {
		if ((errno != EEXIST) && (errno != ENOENT)) BEPRINT;
		memset(bp, 0, sizeof(*biob));
		bp->flags = 0;
		return NULL;
	}

	/* initialize */
	bp->hdrsize = hdrsize;
	bp->blksize = blksize;
	bp->bufcnt = bufcnt;
	bp->endblk = 0;
	bp->most = 0;
	bp->least = 0;
	bp->block_p = NULL;
	bp->blkbuf = NULL;
	if (b_uendblk(bp, &bp->endblk) == -1) {
		BEPRINT;
		terrno = errno;
		b_uclose(bp);
		memset(bp, 0, sizeof(*biob));
		bp->flags = 0;
		errno = terrno;
		return NULL;
	}
	/* allocate memory for bp */
	if (b_alloc(bp) == -1) {
		BEPRINT;
		terrno = errno;
		b_uclose(bp);
		memset(bp, 0, sizeof(*biob));
		bp->flags = 0;
		errno = terrno;
		return NULL;
	}
	/* initialize buffer storage */
	if (b_initlist(bp) == -1) {
		BEPRINT;
		terrno = errno;
		b_free(bp);
		b_uclose(bp);
		memset(bp, 0, sizeof(*biob));
		bp->flags = 0;
		errno = terrno;
		return NULL;
	}

	errno = 0;
	return bp;
}
