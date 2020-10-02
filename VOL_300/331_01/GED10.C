/*
HEADER:         CUG999.10;
TITLE:          GED (nee QED) screen editor -- part 10;
DATE:           10/10/86;

DESCRIPTION:    "Disc interface functions";
FILENAME:       GED10.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
*/

/*
     e/qed/ged  screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle

    FILE:      ged10.c

    FUNCTIONS: fopen1, fcreat, creatb, dflush, fclose, eputc, egetc, funlink,
               frename, lseek1

    PURPOSE:   file buffer operations extracted and stripped from stdlib1,
               with protection from bdos error crashes added

*/

/* These I/O functions are relatively low level.  They could be replaced
 * by higher level and simpler functions, but there are timing consideratons
 * which should be evaluated first.  Microsoft provides some additional
 * error information which is not utilized here.    g.o.
 */

#include <stdio.h>
#include "ged.h"

/* Open an existing file for character input (writing not allowed).
 * The text file is opened in the binary mode.
 */

int fopen1(filename,iobuf)  /* returns file handle */
struct iobuffer *iobuf;
char   *filename;
{
    int i;

    i = open(filename, 0x8000);   /* read only, binary */
    iobuf -> fdes = i;
    if (i < 0) {
        return  FAIL;
    }
    else {
        iobuf -> nleft = 0;
        return  i;
    }
}


/* create a file to be used for reading and writing characters.  truncate
 * an existing file.
 * read and write permission, binary mode.  Evidently compiler dependent.
 * Note that the system call for binary and text I/O are the same.
 */
int fcreat(name,iobuf)
char   *name;
struct iobuffer *iobuf;
{
    if ( ( iobuf -> fdes = open(name, 0x8302, 0x180)) < 0 )
        return  FAIL;
    iobuf -> nextp = iobuf -> buff;
    iobuf -> nleft = ( NSECTS * SECSIZ );
    return  iobuf -> fdes;
}

/* create a file to be used for reading and writing binary blocks.  truncate an
 * existing file.  read and write permission, binary mode.
 * returns file handle.
 */
int creatb(name)
char   *name;
{
    int fd;
    if ( ( fd = open(name, 0x8302, 0x180)) < 0 )
        return  FAIL;
    else
        return  fd;
}


/* The seek is relative to the beginning of the file.
 * Included here for consolidation purposes to simplify disc i/o
 * compiler conversion.
 */
long int lseek1(fd, loc)
int fd;
long int loc;
{
    long int lseek();

    return lseek( fd, loc, 0 );
}


/* does not allow more writing
 */
int dflush(iobuf)
struct  iobuffer *iobuf;
{
    int  i;

    if ( iobuf -> nleft == ( NSECTS * SECSIZ ) )
        return  YES;
    i = NSECTS*SECSIZ - iobuf->nleft;
    if ( write( iobuf -> fdes, iobuf -> buff, i ) != i )
        return  FAIL;
    return YES;
}


int fclose(iobuf)
struct iobuffer *iobuf;
{
    return  close( iobuf -> fdes );
}


/* stripped down version of standard putc
 */
eputc(c,iobuf)
char    c;
struct  iobuffer *iobuf;
{
    if ( iobuf -> nleft-- )
        return  *iobuf -> nextp++ = c;
    if ( ( write(iobuf -> fdes, iobuf -> buff, NSECTS*SECSIZ)) != NSECTS*SECSIZ )
        return  FAIL;
    iobuf -> nleft = ( NSECTS * SECSIZ -1 );
    iobuf -> nextp = iobuf -> buff;

    return  *iobuf -> nextp++ = c;
}


/* the standard getc, trimmed for speed
 */
int egetc(iobuf)
struct iobuffer  *iobuf;
{
    int nbytes;

    if ( iobuf -> nleft-- )
        return  *iobuf -> nextp++;
    if ( (nbytes = read(iobuf -> fdes, iobuf -> buff, NSECTS*SECSIZ)) == -1 )  {
        iobuf -> nleft++;
        return  DFAIL;
    }
    if ( nbytes == 0 )  {
        iobuf -> nleft++;
        return  ENDFILE;
    }
    iobuf -> nleft = nbytes - 1;
    iobuf -> nextp = iobuf -> buff;

    return  *iobuf->nextp++;
}

/* Erase a disc file */
funlink(name)
char *name;
{
/* if (dskcheck(setjmp(dskerr)))  return FAIL; */
    return  unlink( name );
}


frename(oldname,newname)
char *oldname,*newname;
{
/* if (dskcheck(setjmp(dskerr)))  return FAIL; */
    return  rename( oldname, newname );
}
