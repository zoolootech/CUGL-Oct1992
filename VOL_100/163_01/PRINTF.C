/*
** printf(format [,arg] ... ) -- formatted print
**        operates as described by Kernighan & Ritchie
**        only d, o, x, c, s, and u specs are supported.
*/
 
extern int *stdout, CCARGC(), _format(), fputc();
 
printf(args) int args; {
  int argc, *argv, *ctl;
  argc = CCARGC() - 1; /* fetch arg count from CX reg */
  argv = &args; /* address of first argument */
  ctl = argv[argc]; /* get address of format string */
  _format(ctl, argc, argv, fputc, stdout);
  }

