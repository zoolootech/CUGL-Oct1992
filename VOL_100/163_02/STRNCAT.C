/*
** append s2 to s1, truncating at n characters if necessary
*/
strncat(s1, s2, n) char *s1, *s2; int n; {
  char *strncat;
  strncat = s1;
  while(*s1) s1++;
  while(n--) if(!(*s1++ = *s2++)) break;
  return strncat;
  }
