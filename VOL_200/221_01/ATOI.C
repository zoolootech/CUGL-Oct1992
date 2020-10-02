extern int ccmult();
/*
**  convert s to integer
*/
atoi(s) char *s; {
int n, sign;

  sign=n=0;
  if(*s == '+' || *s == '-')  /* sign */
    if(*s++ == '-') sign = -sign;
  while(*s) 
    n = 10 * n + *s++ - '0';
  if(sign) return(-n);
  return n;
}
