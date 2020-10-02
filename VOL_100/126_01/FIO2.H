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

/* FOR EXPLANATION, SEE FIO3.H */

/* THIS INCLUDE SHOULD BE THE FIRST EXECUTABLE CODE IN MAIN;
   IT ZEROS ALL bufuse ELEMENTS, INDICATING THAT NO BUFFERS ARE ASSIGNED
   TO FILES INITIALLY. */

for(bufuse[0]=1;bufuse[0]<=NIOBUFS;bufuse[bufuse[0]++]=0);

bufuse[0] -= 1;
	/* THIS LEAVES bufuse[0] SET AT NIOBUFS FOR USE BY FOPEN, FCLOSE */
       | **
** |                                                               | **
**