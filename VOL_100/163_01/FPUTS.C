/*
** put a string to a stream file
*/
extern int fputc();
 
fputs(s, stream) char *s; int *stream; {
  char ch, *str;
  str=s;
  while(ch=*s++) {
    if(fputc(ch, stream) == -1) return -1;
    }
  return str;
  }
