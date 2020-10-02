/*
** binary read from a stream file
** note that ioctl should be used to reset CRMOD before calling this function
*/
#define FILE int
#define EOF (-1)
extern int fgetc();
 
fread(ptr, size, nitems, stream) char *ptr; int size, nitems; FILE *stream; {
  int icount, bcount;
  icount = nitems;
  while(icount--) {
    bcount = size;
    while(bcount--)
      if((*ptr++ = fgetc(stream)) == EOF) return 0;
    }
  return nitems;
  }
