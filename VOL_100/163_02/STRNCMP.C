/*
** compare s1 and s2 for length n
** result is <, ==, or > 0 as s1 is <, ==, > s2
*/
strncmp(s1, s2, n) char *s1, *s2; int n; {
  while(n--) {
    if(*s1 > *s2) return 1;
    if(*s1 < *s2++) return -1;
    if(!*s1++) return 0;
    }
  return 0;
  }
