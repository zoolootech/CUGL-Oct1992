#define NOCCARGC  /* no argument count passing */
/*
** reverse string in place 
*/
  static char *j;
  static int c;

reverse(s) char *s; {
  j = s + strlen(s) - 1;
  while(s < j) {
    c = *s;
    *s++ = *j;
    *j-- = c;
    }
  }

