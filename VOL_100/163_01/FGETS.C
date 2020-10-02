/*
** get a string from a stream file
** terminate with \n\0
*/
extern int fgetc();
 
fgets(s, n, stream) char *s; int n, *stream; {
  int ch;
  char *str;
  str=s; /* save original value */
  while(--n) {
    ch=fgetc(stream);
    if(ch<0) {
      *s='\0';
      return 0;
      }
    *s++=ch;
    if(ch=='\n') break;
    }
  *s='\0';
  return str;
  }
