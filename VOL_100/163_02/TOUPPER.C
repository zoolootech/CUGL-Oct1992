/*
** return upper-case of c if it is lower-case, else c
*/
toupper(c) int c; {
  return(c<='z' && c>='a' ? c-32 : c);
  }
