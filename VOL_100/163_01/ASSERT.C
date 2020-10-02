extern int *stderr;
extern int fputs(), abort();
/*
** assert -- program verification
*/
assert(expression) int expression; {
  if(!expression) { /* expression should be true -- false is error */
    fputs("Assertion failed\n", stderr);
    abort(2); /* use abort to get traceback */
    }
  }
