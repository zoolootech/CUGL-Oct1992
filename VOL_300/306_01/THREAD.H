/*********** THREAD.H COPYRIGHT 1989 GREGORY COLVIN ************
This program may be distributed free with this copyright notice.
***************************************************************/
#include <setjmp.h>             /* defines jmp_buf            */
#include <assert.h>             /* defines assert             */
#include <stdio.h>              /* defines stderr for assert  */
#include <stdlib.h>             /* defines abort for assert   */

typedef int Thread;             /* index into thread table    */
typedef struct {
    jmp_buf init;               /* state of thread for init   */
    jmp_buf jump;               /* state of thread for jump   */
    char *stack;                /* top of stack for thread    */
    Thread free;                /* next thread on free list   */
} *ThTabl;
extern ThTabl Threads;          /* table of threads           */
extern Thread ThCurr;           /* current thread             */

int    ThInit(int n,int size);  /* create n size byte threads */
Thread ThNew(void(*f)(Thread)); /* fork and exec new thread   */
Thread ThNext(void);            /* find next runnable thread  */
Thread ThJump(Thread id);       /* jump to another thread     */
void   ThExit(Thread id);       /* exit to another thread     */
void   ThFree(void);            /* free the thread table      */

#define ThProbe()               /* don't blow your stack     */\
{ char p;                                                      \
  assert(Threads[ThCurr].stack < &p);                          \
}
#define ThId()                  /* identify yourself         */\
  ThCurr
#define ThLive(id)              /* is this thread alive?     */\
  !Threads[(id)].free
