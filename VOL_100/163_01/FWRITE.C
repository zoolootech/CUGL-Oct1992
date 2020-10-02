/*
** binary write to a stream file
** note that ioctl should be used to reset CRMOD before using this function
*/
#define FILE int
#define EOF (-1)
extern int fputc();
 
fwrite(ptr, size, nitems, stream) char *ptr; int size, nitems, *stream; {
  int icount, bcount;
  icount = nitems;
  while(icount--) {
    bcount = size;
    while(bcount--)
      if(fputc(*ptr++, stream) == EOF) return 0;
    }
  return nitems;
  }
