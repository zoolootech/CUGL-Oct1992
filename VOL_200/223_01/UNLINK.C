/*	
** unlink(name) char *name;		by F.A.Scacchitti  10/86
*/

#define NOCCARGC
#include <stdio.h>

static FILE fd;

unlink(name) char *name; {

   if((fd = fopen(name, "r")) != NULL){
      bdos(19,fd);
      freeio(fd);
      return(TRUE);
   }else{
      return(FALSE);
   }
}

