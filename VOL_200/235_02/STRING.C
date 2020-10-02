/*  003  14-Feb-87  string.c

        Common string related functions.

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#ifndef NULL
#define NULL (0)
#endif

/*****************************************************************************
                            S T R C P Y F I L L
 *****************************************************************************/

char *
strcpyfill(to,from,fldlen,fillch)      /* copy a string with char fill */
char *from;
register char *to;
int fldlen, fillch;
{
   char *to_where;
   register int len;

   to_where = to;
   len = strlen(from);

   if (len < fldlen) {
      strncpy(to,from,len);
      for (to+=len; len < fldlen; len++)
         *to++ = fillch;
   } else
     strncpy(to,from,fldlen);

   return(to_where);
}
