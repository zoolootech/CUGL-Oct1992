/*
** put a word to the stream
*/
#define FILE int
#include "streamio.h"
extern fputc();
 
putw(w, stream) int w;  FILE *stream; {
  if(fputc((w >> 8) & 255, stream) == EOF) return EOF;
  if(fputc(w & 255, stream) == EOF) return EOF;
  return w;
  }
