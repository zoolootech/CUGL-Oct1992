/*
**  convert n in characters in s
*/
itoa(n, s) char *s; int n; {
  int sign;
  char *c;

  if((sign=n)<0) /* record sign */
    n = -n;      /* make n positive */
  c=s;
  do {            /* generate digits in reverse order */
    *s++ = n % 10 + '0';  /* get next digit */
  } while ((n = n/10) > 0);  /* delete it */
  if(sign<0)
    *s++ = '-';
  *s = '\0';
  reverse(c);
}
