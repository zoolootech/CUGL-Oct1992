/*
**  returns string length of str
*/
strlen(str) char *str; {
int i;
  i=0;
  while(*str++) ++i;
  return i;
  }
