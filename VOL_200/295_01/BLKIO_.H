/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"blkio_.h	1.2 - 89/10/31" */

/*man---------------------------------------------------------------------------
NAME
     blkio_.h - private header for blkio library

SYNOPSIS
     #include "blkio_.h"

DESCRIPTION
     This is a private header file included by the blkio library
     modules.

------------------------------------------------------------------------------*/
#ifndef BLKIO_H_	/* prevent multiple includes */
#define BLKIO_H_

/*#define DEBUG		/* switch to enable debugging for blkio library */

/* The HOST macro specifies the host operating system.  Currently supported   */
/* systems are:                                                               */
#define UNIX	(1)	/* UNIX */
#define MSDOS	(2)	/* MS-DOS */
#define HOST	MSDOS

/* The MSDOSC macro specifies the MS-DOS C compiler being used.  Currently    */
/* supported compilers are:                                                   */
#define TURBOC	(1)	/* Borland Turbo C */
#define MSC	(2)	/* Microsoft C */
#define MSQC	(3)	/* Microsoft Quick C */
#define MSDOSC	TURBOC

#include <bool.h>
#include "blkio.h"

/* tables */
extern BLKFILE biob[BOPEN_MAX];	/* BLKFILE control struct table declaration */

/* BLKFILE bit flags */
#define BIOOPEN		  (03)	/* open status bits */
#define BIOREAD		  (01)	/* block file is open for reading */
#define BIOWRITE	  (02)	/* block file is open for writing */
#define BIOUSRBUF	  (04)	/* user supplied buffer */
#define BIOERR		(0100)	/* error has occurred on this block file */

/* block_t bit flags */
#define BLKREAD		  (01)	/* block can be read */
#define BLKWRITE	  (02)	/* block needs to be written to disk */
#define BLKERR		(0100)	/* error has occurred on this block */

/* function declarations */
#if __STDC__ == 1
int	b_alloc(BLKFILE *bp);
#define	b_blkbuf(BP, N) ((void *)(					\
		(char *)(BP)->blkbuf +					\
		((N) == 0 ? 0 : (BP)->hdrsize + ((N) - 1) * (BP)->blksize)\
))
#define	b_block_p(BP, N) ((block_t *)(					\
		(char *)(BP)->block_p +	(N) * sizeof(block_t)		\
))
void	b_free(BLKFILE *bp);
int	b_get(BLKFILE *bp, size_t i);
int	b_initlist(BLKFILE *bp);
int	b_mkmru(BLKFILE *bp, size_t i);
int	b_put(BLKFILE *bp, size_t i);
bool	b_valid(const BLKFILE *bp);

int	b_uclose(BLKFILE *bp);
int	b_uendblk(BLKFILE *bp, bpos_t *endblk_p);
int	b_ugetf(BLKFILE *bp, bpos_t bn, size_t offset, void *buf, size_t bufsize);
int	b_uopen(BLKFILE *bp, const char *filename, const char *type);
int	b_uputf(BLKFILE *bp, bpos_t bn, size_t offset, const void *buf, size_t bufsize);
#else
int	b_alloc();
#define	b_blkbuf(BP, N) ((void *)(					\
		(char *)(BP)->blkbuf +					\
		((N) == 0 ? 0 : (BP)->hdrsize + ((N) - 1) * (BP)->blksize)\
))
#define	b_block_p(BP, N) ((block_t *)(					\
		(char *)(BP)->block_p +	(N) * sizeof(block_t)		\
))
void	b_free();
int	b_get();
int	b_initlist();
int	b_mkmru();
int	b_put();
bool	b_valid();

int	b_uclose();
int	b_uendblk();
int	b_ugetf();
int	b_uopen();
int	b_uputf();
#endif	/* #if __STDC__ == 1 */

/* block file open types */
#define BF_READ		("r")
#define BF_RDWR		("r+")
#define BF_CRTR		("w+")
#define BF_CREATE	("c")

#ifdef DEBUG
#define	BEPRINT {							\
	fprintf(stderr, "blkio error line %d of %s.  errno = %d.\n",	\
		__LINE__, __FILE__, errno);				\
}
#else
#define BEPRINT
#endif

#endif	/* #ifndef BLKIO_H_ */
