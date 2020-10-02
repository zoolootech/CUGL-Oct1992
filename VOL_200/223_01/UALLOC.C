
/*
** ------------ Memory Allocation
*/

/*
** Allocate n bytes of (possibly zeroed) memory.
** Entry: n = Size of the items in bytes.
**    clear = "true" if clearing is desired.
** Returns the address of the allocated block of memory
** or NULL if the requested amount of space is not available.
*/

#include <stdio.h>

extern char *zzmem;
  static char *oldptr;

Ualloc(n, clear) char *n; int clear; {
  if(n < avail(YES)) {
    if(clear) pad(zzmem, NULL, n);
    oldptr = zzmem;
    zzmem += n;
    return (oldptr);
    }
  return (NULL);
  }
