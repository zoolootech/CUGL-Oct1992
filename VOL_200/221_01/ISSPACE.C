#include stdio.h
/*
**  check if space, new line or tab
**  return 1 if true else 0
*/
isspace(c) char c; {
  return((c==' ') || (c=='\t') || (c=='\n'));
  }