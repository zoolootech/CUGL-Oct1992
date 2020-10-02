/************ Simple benchmark program for THREAD.C ***********/
#include <time.h>
#include "thread.h"
#define N 40000
void base() {}
void bench(Thread id) { for (;;) ThJump(id); }
main()
{   Thread id;
    unsigned n=N;
    clock_t b,t;
    b= clock();
    for (n=0; n < N; n++) base();
    b= clock() - b;
    ThInit(2,1024);
    id= ThNew(bench);
    t= clock();
    for (n=0; n < N; n++) ThJump(id);
    t= clock() - t - b;
    printf("%f jumps/sec",(2.0*N)/((float)t/CLK_TCK));
}
