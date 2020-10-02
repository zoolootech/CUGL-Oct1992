/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

/* SEE FIO3.H FOR EXPLANATION */

/* FOR 2 IO FILES SIMULTANEOUSLY, NIOBUFS=2. THIS MEANS _iob[0] THRU
   _iob[1].  HOWEVER, NIOBUFS IS STORED IN bufuse[0], THUS THE FIRST
   BUFFER _iob[0] CORRESPONDS TO bufuse[1]. */

char _iob[NIOBUFS][BUFSIZ];
int bufuse[NIOBUFS+1];
      | **
** |       holder:  