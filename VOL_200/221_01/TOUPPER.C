#include stdio.h
/*
**  convert c to uppercase
*/
toupper(c) char c; {
  if(islower(c) & isalpha(c)) return(c-32);
  else return c;
  }
