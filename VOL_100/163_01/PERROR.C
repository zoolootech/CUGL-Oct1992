/*
** print string s, followed by a colon and the message corresponding to
** the current value of _errno
*/
extern int *stderr, fputs(), fputc(), errno, sys_nerr, sys_errlist[];
perror(s) char *s; {
  fputs(s, stderr);
  fputs(":  ", stderr);
  if(errno < sys_nerr)
    fputs(sys_errlist[errno], stderr);
  else
    fputs("Undefined error", stderr);
  fputc('\n', stderr);
  }
