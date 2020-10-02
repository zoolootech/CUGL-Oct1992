/*
** feof.c	by 	F. A. Scacchitti	11/22/84
**
**	Returns true only if end of file is reached.
**
*/

#include <stdio.h>

   static char c;

feof(fd) int fd; {

   c = *(fd + 40);
   return(c == EOF ? TRUE : FALSE);

}

