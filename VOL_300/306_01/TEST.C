/************** Simple test program for THREAD.C **************/
#include "thread.h"
static void test(Thread id)
{ printf("test: id %d, jump to %d\n",ThId(),id);
  ThJump(id);
  printf("test: id %d, exit to %d\n",ThId(),ThNext());
}
main()
{ int i;
  ThInit(3,1024);
  for (i=1; i <= 9; i++) {
    printf("main: loop %d\n",i);
    printf("main: id %d, made id %d\n",ThId(),ThNew(test));
    printf("main: id %d, made id %d\n",ThId(),ThNew(test));
    printf("main: id %d, from id %d\n",ThId(),ThJump(ThNext()));
  }
  ThFree();
}
