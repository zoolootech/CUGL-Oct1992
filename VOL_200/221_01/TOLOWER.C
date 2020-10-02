/*
**  convert c to lower case
*/
tolower(c) char c; {
  if(isupper(c) & isalpha(c)) return (c+32);
  else return c;
  }
