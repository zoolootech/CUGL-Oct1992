/*  001  14-Feb-87  ovstr.c

        Some OV specific string routines.

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include "ov.h"


/*****************************************************************************
                             M U S T A L L O C
 *****************************************************************************/

char * ALTCALL
mustalloc(len)         /* allocate memory or die */
int len;
{
   char *mp;

   if (mp = (char *) malloc(len))      /* try to allocate, return addr if ok */
      return(mp);

   /* Can't allocate, time to die! The following routines better not call us! */

   reset_tty();
   putstr("\r\nOverView ran out of memory!\r\n");
   exit();
}

/*****************************************************************************
                             M U S T D U P
 *****************************************************************************/

char * ALTCALL
mustdup(sp)    /* duplicate a string or die */
char *sp;
{
   char *nsp;

   nsp = mustalloc(strlen(sp)+1);
   strcpy(nsp,sp);
   return(nsp);
}

/*****************************************************************************
                           M U S T N D U P
 *****************************************************************************/

char * ALTCALL
mustndup(sp,n)         /* duplicate part of a string or die! */
char *sp;
int n;
{
   char *nsp;

   nsp = mustalloc(n+1);
   *nsp = '\0';
   strncat(nsp,sp,n);
   return(nsp);
}
