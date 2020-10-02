/*
** get a word from the stream
*/
#define FILE int
#include "streamio.h"
extern fgetc();
 
getw(stream) FILE *stream; {
  char hi, lo;
  if((hi=fgetc(stream)) == EOF) return EOF;
  if((lo=fgetc(stream)) == EOF) return EOF;
  return (hi << 8) & (lo & 255);
  }
