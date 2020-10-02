/*
** copy n characters from sour to dest (null padding)
*/
  static char *xd;

strncpy(dest, sour, n) char *dest, *sour; int n; {
  xd = dest;
  while(n-- > 0) {
    if(*xd++ = *sour++) continue;
    while(n-- > 0) *xd++ = 0;
    }
  *xd = 0;
  return (dest);
  }
