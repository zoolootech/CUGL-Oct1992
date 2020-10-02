/*********** THREAD.C COPYRIGHT 1989 GREGORY COLVIN ************
This program may be distributed free with this copyright notice.
***************************************************************/
#include "thread.h"

       ThTabl Threads=0;        /* table of threads           */
       Thread ThCurr;           /* currently executing thread */
static int    N_Threads;        /* number of threads in table */
static Thread Free;             /* first thread on free list  */
static char  *Stack;            /* bottom of stack for init   */
static void (*Root)(Thread);    /* for temporary use for exec */

static void init(int n,int size)        /* recurse for ThInit */
{ Thread id;
  if (!Stack) {                         /* start new thread   */
    Stack= (char *)&size;               /* at top of stack    */
    if (id= setjmp(Threads[n].init)) {  /* set entry point    */
      (*Root)(id);                      /* call root function */
      ThExit(ThNext());                 /* fell off thread    */
    }
  }
  if (Stack - (char *)&size < size)     /* if not enough stack*/
    init(n,size);                       /* push more stack    */
  else {                                /* done with a thread */
    Threads[n].stack= (char *)&size;    /* save top of stack  */
    Stack= 0 ;                          /* flag for new thread*/
    if (n < N_Threads) {                /* if not done        */
      Threads[n].free= n+1;             /* link to free list  */
      init(++n,size);                   /* push more stack    */
    }
  }
  Threads[n].free = 0;                  /* end of free list   */
  longjmp(Threads[1].jump,1);           /* return to ThInit   */
}
int ThInit(int n,int size)      /* create n size byte threads */
{ assert (!Threads);                    /* must be free       */
  assert (n >= 2);                      /* must be plural     */
  Threads=                              /* create table       */
    (ThTabl)calloc(n,sizeof *Threads);
  if (!Threads)
    return 0;                           /* not enough memory  */
  Threads--;                            /* will index from 1  */
  if (setjmp(Threads[1].jump)){         /* become thread 1    */
    Threads[1].free= 0;                 /* take off free list */
    return n;                           /* return to thread 1 */
  }
  N_Threads= n, ThCurr= 1, Free= 2;     /* initialize globals */
  init(1,size);                         /* initialize stack   */
}

Thread ThNew(void(*f)(Thread))  /* fork and exec new thread   */
{ Thread parent, fork;
  ThProbe();                            /* stack probe        */
  assert(f);                            /* prevent crash      */
  fork= Free;                           /* fork to free thread*/
  if (fork == 0)
    return 0;                           /* error, none free   */
  Free= Threads[fork].free;             /* take off free list */
  Threads[fork].free= 0;
  parent= ThCurr;                       /* current is parent  */
  ThCurr= fork;                         /* will run fork next */
  if (setjmp(Threads[parent].jump))     /* put parent to sleep*/
    return fork;                        /* parent is awake    */
  Root= f;                              /* who to call        */
  longjmp(Threads[fork].init,parent);   /* exec root in init  */
}

Thread ThNext(void)             /* find next runnable thread  */
{ Thread id= ThCurr;                    /* start from self    */
  do id %= N_Threads;                   /* circle round table */
  while (Threads[++id].free);           /* until we find life */
  return id;                            /* next might be self */
}

Thread ThJump(Thread id)        /* jump to another thread     */
{ Thread jumper, caller;
  ThProbe();                            /* stack probe        */
  assert(id > 0 && id <= N_Threads);    /* prevent crash      */
  assert(ThLive(id) && id != ThCurr);   /* no go dead or self */
  caller= ThCurr;                       /* where we came from */
  ThCurr= id;                           /* where we are going */
  if (jumper=setjmp(Threads[caller].jump))
    return jumper;                      /* return who jumped  */
  longjmp(Threads[id].jump,caller);     /* context switch     */
}

void ThExit(Thread id)          /* exit to another thread     */
{ ThProbe();                            /* stack probe        */
  assert(id > 0 && id <= N_Threads);    /* prevent crash      */
  assert(ThLive(id) && id != ThCurr);   /* no go dead or self */
  Threads[ThCurr].free= Free;           /* put on free list   */
  Free= ThCurr;                         /* where we came from */
  ThCurr= id;                           /* where we are going */
  longjmp(Threads[id].jump,Free);       /* context switch     */
}

void ThFree()                   /* free the thread table      */
{   assert (Threads);                   /* prevent crash      */
    free(Threads+1);                    /* goodbye            */
    Threads= 0, N_Threads= 0;           /* can init again     */
}
