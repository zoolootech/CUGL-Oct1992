/*
** copy s2 to s1, truncating or null padding as necessary to create a string
** n characters long
*/
strncpy(s1, s2, n) char *s1, *s2; int n; {
  char *strncpy;
  strncpy = s1;
  while(n--) if(!(*s1++ = *s2++)) break;
  while(n-- > 0) *s1++ = 0;
  return strncpy;
  }
