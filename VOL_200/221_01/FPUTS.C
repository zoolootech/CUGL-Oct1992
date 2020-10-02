#include stdio.h
fputs(s, iop)  /* put string s in file iop */
char *s;
int iop;
{
  int c;

  while(c = *s++)
    putc(c, iop);
}

