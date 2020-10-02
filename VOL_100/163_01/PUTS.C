/*
** put a string to "stdout"
*/
extern int *stdout, fputs(), fputc();
 
puts(s) char *s; {
  if(fputs(s, stdout) == -1) return -1;
  if(fputc('\n', stdout) == -1) return -1;
  return s;
  }
