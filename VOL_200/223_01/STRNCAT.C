/*
** concatenate n bytes max from t to end of s 
** s must be large enough
*/
  static char *xd;

strncat(s, t, n) char *s, *t; int n; {
  xd = s;
  --s;
  while(*++s) ;
  while(n--) {
    if(*s++ = *t++) continue;
    return(xd);
    }
  *s = 0;
  return(xd);
  }
