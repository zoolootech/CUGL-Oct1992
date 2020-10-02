/*
** ferror.c	by 	F. A. Scacchitti	11/22/84
**
**	Returns true only if a file error has occurred.
**
*/

#include <stdio.h>

   static char c;

ferror(fd) int fd; {

   c = *(fd + 40);
   return(c == EOF ? FALSE : c);

}

