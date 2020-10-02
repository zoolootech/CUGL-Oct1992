/*
Header:          CUG199;
Title:           Module 10 of ged editor;
Last Updated:    12/01/87;

Description:    "PURPOSE: file buffer operations extracted and stripped
                 from stdlib1, with protection from bdos error crashes
                 removed for MSDOS";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged10.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 2.51;

References:
Endref;
*/

/*
e/qed/ged  screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981

Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
           March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

           May       1986:  qed converted to ged         (Mel Tearle)
           August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:      ged10.c

Functions: ffopen, fcreat, dflush, ffclose, eputc, egetc, funlink,
           frename
*/


#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


int ffopen(filename,iobuf)
struct iobuffer *iobuf;
char   *filename;
{
#ifndef DS
  if ( ( iobuf -> _fd = open( filename, O_RDWR | O_TEXT ) ) < 0 )
         return  FAIL;
#else
  if ( ( iobuf -> _fd = open( filename, 0 ) ) < 0 )
         return  FAIL;
#endif

iobuf -> _nleft = 0;
return  iobuf -> _fd;
}


int fcreat(name,iobuf)
char   *name;
struct iobuffer *iobuf;
{

if ( ( iobuf -> _fd = creat( name ) ) < 0 )
       return  FAIL;

iobuf -> _nextp = iobuf -> _buff;
iobuf -> _nleft = ( NSECTS * SECSIZ );

return  iobuf -> _fd;
}


/* does NOT allow more writing
 */
int dflush(iobuf)
struct  iobuffer *iobuf;
{
unsigned int i;

if ( iobuf -> _nleft == ( NSECTS * SECSIZ ) )
     return  YES;

i = NSECTS*SECSIZ - iobuf->_nleft;

if ( write( iobuf -> _fd, iobuf -> _buff, i ) != i )
     return  FAIL;

return YES;
}


int ffclose(iobuf)
struct  iobuffer *iobuf;
{
return  close( iobuf -> _fd );
}


/* stripped down version of standard putc
 */
int eputc(c,iobuf)
char    c;
struct  iobuffer *iobuf;
{
if ( iobuf -> _nleft-- )  return  *iobuf -> _nextp++ = c;

if ( ( write(iobuf -> _fd, iobuf -> _buff, NSECTS*SECSIZ ) )
       != NSECTS*SECSIZ )
       return  FAIL;

iobuf -> _nleft = ( NSECTS * SECSIZ -1 );
iobuf -> _nextp = iobuf -> _buff;

return  *iobuf -> _nextp++ = c;
}


/* the standard getc, trimmed for speed
 */
int egetc(iobuf)
struct  iobuffer  *iobuf;
{
unsigned int nbytes;

if ( iobuf -> _nleft-- )  return  *iobuf -> _nextp++;
   if ( ( nbytes = read( iobuf -> _fd,
                   iobuf -> _buff,
                   NSECTS*SECSIZ ) ) < 0 )  {
            iobuf -> _nleft++;
            return  DFAIL;
   }
   if ( nbytes == 0 ) {
        iobuf -> _nleft++;
        return  ENDFILE;
   }
iobuf -> _nleft = nbytes - 1;
iobuf -> _nextp = iobuf -> _buff;

return  *iobuf->_nextp++;
}


int funlink(name)
char *name;
{
return  unlink( name );
}


int frename(oldname,newname)
char *oldname,*newname;
{
return  rename( oldname, newname );
}


/* that's all */
