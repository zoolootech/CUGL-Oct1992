/*  007  17-Jan-87  file.c

        Functions to work on/with DOS files.

        Copyright (c) 1987 by Blue Sky Software.  Alll rights reserved.
*/

#include <dos.h>
#include "dosfile.h"

#ifndef NULL
#define NULL (0)
#endif

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

static struct search_block search_blk;


/******************************************************************************
 **                            N X T F I L E                                 **
 *****************************************************************************/

struct search_block *
nxtfile(fn,mask,firstp)        /* find the first/next file matching fn and */
char *fn;                      /*   the attribute mask */
unsigned mask;
int *firstp;
{

#define SEARCH_FIRST    0x4E00
#define SEARCH_NEXT     0x4F00

   union REGS r;

   bdos(0x1a,(unsigned int)&search_blk,0);   /* set the DTA addr for search's */

   r.x.ax = *firstp ? SEARCH_FIRST : SEARCH_NEXT;  /* fisrt/next flag */
   *firstp = FALSE;

   r.x.cx = mask;                      /* caller specifies file search mask */
   r.x.dx = (int) fn;                  /* the file name to search for */
   intdos(&r,&r);                      /* let dos do the looking */

   if (r.x.cflag)                      /* cflag means error (no more files) */
      return(NULL);                    /*   return NULL if no name */
   else
      return(&search_blk);             /* otherwise, return search block addr */

}


/*****************************************************************************
                         A L L O C _ S I Z
 *****************************************************************************/

unsigned long
alloc_siz(used,clustersiz)     /* calc the allocated size of a file */
unsigned long used;
unsigned int clustersiz;
{
   return( (used + clustersiz - 1) & ~((long)(clustersiz-1)) );
}
