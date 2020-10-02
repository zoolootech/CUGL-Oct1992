/*
** push a character back into an input stream
*/
#include <streamio.h>
 
ungetc(c, stream) char c; int *stream; {
  if(stream[_IOB_FLAG]&_UNGET) return EOF;
  stream[_IOB_FLAG]=(stream[_IOB_FLAG]&255)|(c<<8)|_UNGET;
  return c&255;
  }
