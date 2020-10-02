/*
** return pointer to the last occurrence of c in s
*/
rindex(s, c) char *s, c; {
  char *rindex;
  rindex = 0;
  while(*s) {
    if(*s == c) rindex = s;
    s++;
    }
  return rindex;
  }
