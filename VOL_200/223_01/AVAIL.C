#define NOCCARGC  /* no argument count passing */
/*
** Return the number of bytes of available memory.
** In case of a stack overflow condition, if 'abort'
** is non-zero the program aborts with an 'S' clue,
** otherwise zero is returned.
*/

external char *zzmem

avail(aburt) int aburt; {
  char x;
  if(&x < zzmem) {
    if(aburt) abort('M');
    return (0);
    }
  return (&x - zzmem);
  }

