/*
** fprintf(stream, format [,arg] ... ) -- formatted print
**        operates as described by Kernighan & Ritchie
**        only d, o, x, c, s, and u specs are supported.
*/
 
#define FILE int
extern int CCARGC(), fputc(), _format();
 
fprintf(args) int args; {
  int argc, *argv, *ctl;
  FILE *stream;
  argc = CCARGC() - 1; /* fetch arg count from CX reg */
  argv = &args; /* address of first argument */
  stream = argv[argc--]; /* get address of stream */
  ctl = argv[argc]; /* get address of format string */
  _format(ctl, argc, argv, fputc, stream);
  }
