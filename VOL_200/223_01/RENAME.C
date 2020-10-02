/*	
** rename(old,new) char *old, *new;	by F.A.Scacchitti  9/11/84
*/

#define NOCCARGC
#include <stdio.h>

   static int fcbloc;

rename(old,new) char *old, *new; {

   if ((fcbloc = grabio()) == NULL) return(ERR);
   fcb(fcbloc, old);
   fcb(fcbloc + 16, new);
   freeio(fcbloc);

   if(bdos(23,fcbloc) >= 0) return(NULL);
   return(ERR);
}

