/*
** isspace -- true if argument is ASCII space, tab, carriage return,
**            newline (line feed), or form feed  (ie, "white space")
*/
isspace(c) char c; {
  if(c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f') return 1;
  else return 0;
  }
