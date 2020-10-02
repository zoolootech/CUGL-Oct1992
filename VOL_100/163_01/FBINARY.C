/*
** set stream to binary mode (CRMOD off)
*/
#include <sgtty.h>
#define FILE int
#define EOF (-1)
extern fileno(), _ioctl();
 
fbinary(stream) FILE stream; {
  int fd;
  char sgttyb[SG_SIZE];
  if((fd = fileno(stream)) == 0) return EOF;
  _ioctl(fd, TIOCGETP, sgttyb);
  sgttyb[SG_FLGS] &= ~'\020'; /* turn off CR/LF conversion */
  _ioctl(fd, TIOCSETP, sgttyb);
  }
