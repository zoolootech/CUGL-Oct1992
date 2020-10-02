extern int isalpha(), iscntrl();
/*
** ispunct -- true if argument is neither ASCII control or alphabetic
*/
ispunct(c) char c; {
  if(isalpha(c) || iscntrl(c)) return 0;
  else return 1;
  }
