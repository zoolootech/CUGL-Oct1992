#define NOCCARGC  /* no argument count passing */

#define ABORT 3
#define PAUSE 19

#include stdio.h
/*
** Poll for console input or interruption
*/
   static int i;

poll(pause) int pause; {
  i = bdos(6,255);
  if(pause) {
    if(i == PAUSE) {
      while(!(i = bdos(6,255))) ;
      if(i == ABORT) abort(0);
      return (0);
      }
    if(i == ABORT) abort(0);
    }
  return (i);
  }
