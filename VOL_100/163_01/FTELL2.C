/*
** determine current stream position
**
** ptrname == 0 -- return byte offset mod 512
** ptrname != 0 -- return sector offset
*/
#define FILE int
#define _BUFSIZE 512
#include <streamio.h>
 
extern int errno, _seek();
 
ftell2(stream, ptrname) FILE *stream; int ptrname; {
  int errsave, lowoff, lowpart, highoff;
  errsave = errno;  /* in case someone else wants it preserved */
  errno = 0;  /* assure no prior error */
  lowoff = _seek(stream[_IOB_FD], 0, 1);  /* current byte position */
  highoff = _seek(stream[_IOB_FD], 0, 4);  /* current sector position */
  if(errno) return -1;  /* return if error */
  errno = errsave;  /* restore prior error (if any) */
  lowpart = lowoff % 512;  /* part not also reflected in highoff */
  if(stream[_IOB_FLAG] & _DIRTY) {  /* if buffer in write mode */
    lowoff += (_BUFSIZE - stream[_IOB_CNT]);  /* add number of bytes buffered */
    lowpart += (_BUFSIZE - stream[_IOB_CNT]);  /* again */
    highoff += lowpart / 512;  /* handle any "carry" */
    }
  else {  /* either read mode or unbuffered */
    lowoff -= stream[_IOB_CNT];  /* subtract off number of bytes buffered */
    lowpart -= stream[_IOB_CNT];  /* again */
    highoff += (lowpart-511) / 512;  /* handle any "borrow" */
    }
  return ptrname ? highoff : lowoff;  /* return requested value */
  }
