#define NOCCARGC  /* no argument count passing */
/*
** atoi(s) - convert s to integer.
*/
  static int sign, n;

atoi(s) char *s; {
  while(isspace(*s)) ++s;
  sign = 1;
  switch(*s) {
    case '-': sign = -1;
    case '+': ++s;
    }
  n = 0;
  while(isdigit(*s)) n = 10 * n + *s++ - '0';
  return (sign * n);
  }
