/*
** get the unformatted parameter list from the DOS PSP
** return list in argument and length as function value
*/
 
extern int *_stkbase; /* play a game with this */
extern int peekc(); /* peek at specified segment and offset */
 
getparm(parms) char *parms; {
  int length, count, psp;
  char *ptr1, *ptr2;
  psp = _stkbase[1]; /* paragraph address of DOS PSP */
  ptr1 = 128; /* address of byte count in PSP */
  length = count = peekc(psp, ptr1++);
  ptr2 = parms;
  while(count--) *ptr2++ = peekc(psp, ptr1++);
  *ptr2 = 0;
  return length;
  }
