/*
** test for EOF on stream
*/
#define FILE int
#include "streamio.h"
 
feof(stream) FILE *stream; {
  return ((stream[_IOB_FLAG] & _EOF) != 0);
  }
