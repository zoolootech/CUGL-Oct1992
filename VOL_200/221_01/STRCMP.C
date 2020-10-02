/*
**  compare string s against t
**  returns <0 if s<t, 0 if s==t, >0 if s>t
*/
strcmp(s, t) char *s, *t; {
  for(; *s==*t; ++s, ++t)
    if(*s == '\0') return 0;
  return(*s - *t);
}
