/*
** isascii -- true if argument is valid ASCII character
*/
isascii(c) char c; {
  if(c <= 127) return 1;
  else return 0;
  }
