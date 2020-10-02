/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"buops.c	1.2 - 89/10/31" */

#include "blkio_.h"

/* check host definition */
#ifndef HOST
/*#error HOST macro not defined.*/
#endif
#if HOST != UNIX && HOST != MSDOS
/*#error Invalid HOST macro definition.*/
#endif

#include <errno.h>
#if HOST == UNIX
#include <fcntl.h>	/* open() macro definitions */
#include <unistd.h>	/* lseek() macro definitions */
int close(/*int fd*/);	/* system call declarations */
long lseek(/*int fd, long offset, int whence*/);
int open(/*char *path, int flags, ...*/);
int read(/*int fd, char *buf, unsigned n*/);
int write(/*int fd, char *buf, unsigned n*/);
#define MODE	(0666)	/* file permissions for new files */
#elif HOST == MSDOS
#if MSDOSC == TURBOC
#include <fcntl.h>	/* open() macro definitions */
#include <io.h>		/* system call declarations, lseek() macro defs */
#define MODE	(0x100 | 0x080)		/* file permissions for new files */
#elif MSDOSC == MSC
#include <fcntl.h>	/* open() macro definitions */
#include <io.h>		/* system call declarations, lseek() macro defs */
#define MODE	(0x100 | 0x080)		/* file permissions for new files */
#elif MSDOSC == MSQC
#include <fcntl.h>	/* open() macro definitions */
#include <io.h>		/* system call declarations, lseek() macro defs */
#define MODE	(0x100 | 0x080)		/* file permissions for new files */
#endif	/* #if MSDOSC == TURBOC */
#endif	/* #if HOST == MSDOS */

/*man---------------------------------------------------------------------------
NAME
     b_uclose - unbuffered close block file

SYNOPSIS
     #include "blkio_.h"

     int b_uclose(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_uclose function closes the file associated with the BLKFILE
     pointer bp.

     b_uclose will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [BENOPEN]      bp is not open.

SEE ALSO
     b_uopen.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_uclose(bp)
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
#endif

	/* close file */
#if HOST == UNIX
	if (close(bp->fd.ifd) == -1) {
		BEPRINT;
		return -1;
	}
#elif HOST == MSDOS
	if (close(bp->fd.ifd) == -1) {
		BEPRINT;
		return -1;
	}
#endif

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_uendblk - unbuffered find end block

SYNOPSIS
     #include "blkio_.h"

     int b_uendblk(bp, endblk_p)
     BLKFILE *bp;
     bpos_t *endblk_p;

DESCRIPTION
     The b_uendblk function finds the block number of the first block
     past the end of file of the file associated with BLKFILE pointer
     bp.  This value is returned in the storage location pointed to by
     endblk_p.  Blocks in buffer storage and not yet written to the
     file are not counted, so this function should normally be used
     when the file is first opened or preceded by a call to bsync.

     If the file does not end on a block boundary, the result is the
     same as if the partial block (or header) at the end of the file
     did not exist.

     b_uendblk will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [BENOPEN]      bp is not open.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_uendblk(bp, endblk_p)
BLKFILE *bp;
bpos_t *endblk_p;
{
#if HOST == UNIX
	long pos = 0;
#elif HOST == MSDOS
	long pos = 0;
#endif

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

#if HOST == UNIX || HOST == MSDOS
	/* find position of end of file */
	pos = lseek(bp->fd.ifd, (long)0, SEEK_END);
	if (pos == -1) {
		BEPRINT;
		return -1;
	}

	/* check if empty file (or incomplete header) */
	if (pos < bp->hdrsize) {
		*endblk_p = 0;
		errno = 0;
		return 0;
	}

	/* find length past end of header */
	pos -= bp->hdrsize;

	/* set return argument */
	*endblk_p = (pos / bp->blksize) + 1;
#endif

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_ugetf - unbuffered get field from block file

SYNOPSIS
     #include "blkio_.h"

     int b_ugetf(bp, bn, offset, buf, bufsize)
     BLKFILE *bp;
     bpos_t bn;
     size_t offset;
     void *buf;
     size_t bufsize;

DESCRIPTION
     The b_ugetf function reads bufsize characters from block number
     bn of the block file associated with BLKFILE pointer bp into the
     buffer pointed to by buf.  A value of 0 for bn indicates the file
     header.  The read starts offset characters from the beginning of
     the block.  The sum of the offset and the bufsize must not exceed
     the header size if bn is equal to zero, or the block size if bn
     is not equal to zero.

     b_ugetf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       buf is NULL.
     [EINVAL]       bufsize is less than 1.
     [BEBOUND]      offset + bufsize extends across a block
                    boundary.
     [BEEOF]        Block bn is past the end of file.
     [BEEOF]        End of file encountered within block bn.
     [BENOPEN]      bp is not open.

SEE ALSO
     b_uputf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_ugetf(bp, bn, offset, buf, bufsize)
BLKFILE *bp;
bpos_t bn;
size_t offset;
void *buf;
size_t bufsize;
{
#if HOST == UNIX
	long pos = 0;
	int nr = 0;
#elif HOST == MSDOS
	long pos = 0;
	int nr = 0;
#endif

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp) || (buf == NULL) || (bufsize < 1)) {
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

	/* check if block boundary is crossed */
	if (bn == 0) {
		if ((offset + bufsize) > bp->hdrsize) {
			BEPRINT;
			errno = BEBOUND;
			return -1;
		}
	} else {
		if ((offset + bufsize) > bp->blksize) {
			BEPRINT;
			errno = BEBOUND;
			return -1;
		}
	}
#endif

#if HOST == UNIX
	/* read from file into buffer */
	if (bn == 0) {			/* header */
		pos = 0;
	} else {			/* block */
		pos = bp->hdrsize + (bn - 1) * bp->blksize;
	}
	pos += offset;
	if (lseek(bp->fd.ifd, pos, SEEK_SET) == -1) {
		BEPRINT;
		return -1;
	}
	nr = read(bp->fd.ifd, (char *)buf, (unsigned)bufsize);
	if (nr == -1) {
		BEPRINT;
		return -1;
	}
	if (nr != bufsize) {
		BEPRINT;
		errno = BEEOF;
		return -1;
	}
#elif HOST == MSDOS
	/* read from file into buffer */
	if (bn == 0) {			/* header */
		pos = 0;
	} else {			/* block */
		pos = bp->hdrsize + (bn - 1) * bp->blksize;
	}
	pos += offset;
	if (lseek(bp->fd.ifd, pos, SEEK_SET) == -1) {
		BEPRINT;
		return -1;
	}
	nr = read(bp->fd.ifd, buf, (unsigned)bufsize);
	if (nr == -1) {
		BEPRINT;
		return -1;
	}
	if (nr != bufsize) {
		BEPRINT;
		errno = BEEOF;
		return -1;
	}
#endif

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_uopen - unbuffered open block file

SYNOPSIS
     #include "blkio_.h"

     int b_uopen(bp, filename, type)
     BLKFILE *bp;
     const char *filename;
     const char *type;

DESCRIPTION
     The b_uopen function opens the physical file associated with the
     BLKFILE pointer bp.

     b_uopen will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       filename or type is the NULL pointer.

SEE ALSO
     b_uclose.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_uopen(bp, filename, type)
BLKFILE *bp;
CONST char *filename;
CONST char *type;
{
#if HOST == UNIX
	int oflag = 0;
	int fd = 0;
#elif HOST == MSDOS
	int oflag = 0;
	int fd = 0;
#endif

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp) || (filename == NULL) || (type == NULL)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif

	/* open file */
#if HOST == UNIX
	oflag = 0;
	if (strcmp(type, BF_READ) == 0) {
		oflag |= O_RDONLY;
	} else if (strcmp(type, BF_RDWR) == 0) {
		oflag |= O_RDWR;
	} else if (strcmp(type, BF_CREATE) == 0) {
		oflag |= O_RDWR | O_CREAT | O_EXCL;
	} else if (strcmp(type, BF_CRTR) == 0) {
		oflag |= O_RDWR | O_CREAT | O_TRUNC;
	} else {
		errno = EINVAL;
		return -1;
	}
	fd = open(filename, oflag, MODE);
	if (fd == -1) {
		if ((errno != EACCES) && (errno != EEXIST) && (errno != ENOENT)) BEPRINT;
		return -1;
	}
	bp->fd.ifd = fd;
#elif HOST == MSDOS
	oflag = O_BINARY;
	if (strcmp(type, BF_READ) == 0) {
		oflag |= O_RDONLY;
	} else if (strcmp(type, BF_RDWR) == 0) {
		oflag |= O_RDWR;
	} else if (strcmp(type, BF_CREATE) == 0) {
		oflag |= O_RDWR | O_CREAT | O_EXCL;
	} else if (strcmp(type, BF_CRTR) == 0) {
		oflag |= O_RDWR | O_CREAT | O_TRUNC;
	} else {
		errno = EINVAL;
		return -1;
	}
	fd = open(filename, oflag, (unsigned)MODE);
	if (fd == -1) {
		if ((errno != EACCES) && (errno != EEXIST) && (errno != ENOENT)) BEPRINT;
		return -1;
	}
	bp->fd.ifd = fd;
#endif

	errno = 0;
	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_uputf - unbuffered put field to block file

SYNOPSIS
     #include "blkio_.h"

     int b_uputf(bp, bn, offset, buf, bufsize)
     BLKFILE *bp;
     bpos_t bn;
     size_t offset;
     const void *buf;
     size_t bufsize;

DESCRIPTION
     The b_uputf function writes bufsize characters from the buffer
     pointed to by buf to block number bn of the block file associated
     with BLKFILE pointer bp.  A value of zero for bn indicates the
     file header.  The write starts offset characters from the
     beginning of the block.  The sum of offset bufsize must not
     exceed the header size if bn is equal to zero, or the block size
     if bn is not equal to zero.

     b_uputf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       buf is the NULL pointer.
     [EINVAL]       bufsize is less than 1.
     [BEBOUND]      offset + bufsize extends beyond the block
                    boundary.
     [BEEOF]        Partial block being written and block bn
                    is past the end of file.
     [BEEOF]        Complete block being written and block bn
                    is more than 1 past the end of file.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     b_ugetf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
int b_uputf(bp, bn, offset, buf, bufsize)
BLKFILE *bp;
bpos_t bn;
size_t offset;
CONST void *buf;
size_t bufsize;
{
#if HOST == UNIX
	long pos = 0;
	int nw = 0;
#elif HOST == MSDOS
	long pos = 0;
	int nw = 0;
#endif

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp) || (buf == NULL) || (bufsize < 1)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOWRITE)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if block boundary is crossed */
	if (bn == 0) {
		if ((offset + bufsize) > bp->hdrsize) {
			BEPRINT;
			errno = BEBOUND;
			return -1;
		}
	} else {
		if ((offset + bufsize) > bp->blksize) {
			BEPRINT;
			errno = BEBOUND;
			return -1;
		}
	}
#endif

#if HOST == UNIX || HOST == MSDOS
	/* write buffer to file */
	if (bn == 0) {			/* header */
		pos = 0;
	} else {			/* block */
		pos = bp->hdrsize + (bn - 1) * bp->blksize;
	}
	pos += offset;
	if (lseek(bp->fd.ifd, pos, SEEK_SET) == -1) {
		BEPRINT;
		return -1;
	}
	nw = write(bp->fd.ifd, (char *)buf, (unsigned)bufsize);
	if (nw == -1) {
		BEPRINT;
		return -1;
	}
	if (nw != bufsize) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}
#elif HOST == MSDOS
	/* write buffer to file */
	if (bn == 0) {			/* header */
		pos = 0;
	} else {			/* block */
		pos = bp->hdrsize + (bn - 1) * bp->blksize;
	}
	pos += offset;
	if (lseek(bp->fd.ifd, pos, SEEK_SET) == -1) {
		BEPRINT;
		return -1;
	}
	nw = write(bp->fd.ifd, buf, (unsigned)bufsize);
	if (nw == -1) {
		BEPRINT;
		return -1;
	}
	if (nw != bufsize) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}
#endif

	errno = 0;
	return 0;
}
