/*
** test for error on stream
*/
#define FILE int
#include "streamio.h"
 
ferror(stream) FILE *stream; {
  return ((stream[_IOB_FLAG] & _ERR) != 0);
  }
