/*
** reposition a stream file
*/
#define FILE int
#include <streamio.h>
extern int fflush(), _seek(), free();
 
fseek2(stream, offset, ptrname) FILE *stream; int offset, ptrname; {
  fflush(stream);  /* flush any write data */
  if(stream[_IOB_CNT]) {  /* free read buffer if present */
    free(stream[_IOB_BASE]);
    stream[_IOB_CNT] = 0;
    }
  stream[_IOB_FLAG] &= ~(_UNGET);
  _seek(stream[_IOB_FD], offset, ptrname);
  return 0;
  }
