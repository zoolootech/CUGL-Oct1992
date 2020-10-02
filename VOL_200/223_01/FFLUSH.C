
/*
** fflush.c	by	F. A. Scacchitti	9/15/84
*/

#define WRITE 1
#define OPEN 15

#include <stdio.h>

static int unused, charptr;
static char mode, record, extent;

fflush(fd) int fd; {


   mode = *(fd + 33);
   if(mode != WRITE) return(NULL);

   charptr = *(fd + 36);
   unused  = *(fd + 38);
   extent  = *(fd + 12);
   record  = *(fd + 32);

   if(fclose(fd) != NULL || bdos(OPEN,fd) < NULL) return(EOF);

   *(fd + 36) = charptr;
   *(fd + 38) = unused;
   *(fd + 12) = extent;
   *(fd + 32) = record;
   *(fd + 33) = mode;

   return(NULL);

}

