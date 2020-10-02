/*  001  29-Jun-86  dosfile.h

        Header file for use with file.c.

        Copyright (c) 1986 by Blue Sky Software.  All rights reserved.
*/


struct search_block {                  /* dos structure for file searching */
   char context[21];                   /* dos context usage */
   unsigned char attrib;               /* file entry attributes */
   unsigned time;                      /* file time */
   unsigned date;                      /* file date */
   unsigned long size;                 /* file size */
   char fn[13];                        /* dos file name */
};
