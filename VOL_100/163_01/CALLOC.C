/*
** allocate and clear nelem elements of size elsize
*/
extern int malloc();
 
calloc(nelem, elsize) int nelem, elsize; {
  int size;
  char *calloc, *ptr;
  ptr = calloc = malloc(size = nelem * elsize);
  if(calloc) while(size--) *ptr++ = 0;
  return calloc;
  }
