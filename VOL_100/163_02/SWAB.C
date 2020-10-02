#include <errno.h>
extern int abort();
/*
** copy nbytes between from and to, swapping odd and even bytes in the process
*/
swab(from, to, nbytes) char *from, *to; int nbytes; {
  char temp;
  if(nbytes & 1) abort(EINVAL); /* nbytes must be even */
  nbytes >>= 1; /* get count in halfwords */
  while(nbytes--) {
    temp = *from++; /* temp allows copy to self */
    *to++ = *from++;
    *to++ = temp;
    }
  }
