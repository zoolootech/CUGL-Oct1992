/*
** sprintf(s, format [,arg] ... ) -- string formatter
**        operates as described by Kernighan & Ritchie
**        only d, o, x, c, s, and u specs are supported.
*/
 
extern int CCARGC(), _format();
 
sprintf(args) int args; {
  int argc, *argv, *ctl;
  char *s;
  argc = CCARGC() - 1; /* fetch arg count from CX reg */
  argv = &args; /* address of first argument */
  s = argv[argc--]; /* get address of string */
  ctl = argv[argc]; /* get address of format string */
  _format(ctl, argc, argv, zapc, &s);
  *s = 0;
  }
 
static zapc(c, s) char c; int *s; {
  char *string;
  string = *s; /* get string address */
  *string++ = c;
  *s = string;
  return c;
  }
