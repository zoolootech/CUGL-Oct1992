/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKCNT.C - CTask - Counter handling routines.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   Version 2.1 adds the dec_counter operation, and changes inc_counter
   to return the new value of the counter state (previously, it returned
   nothing).
*/

#include "tsk.h"
#include "tsklocal.h"

/*
   create_counter - initialises counter.
*/

counterptr Globalfunc create_counter (counterptr cnt TN(byteptr name))
{
#if (TSK_DYNAMIC)
   if (cnt == LNULL)
      {
      if ((cnt = tsk_palloc (sizeof (counter))) == LNULL)
         return LNULL;
      cnt->flags = F_TEMP;
      }
   else
      cnt->flags = 0;
#endif

   tsk_init_qhead (&cnt->wait_set, TYP_COUNTER);
   tsk_init_qhead (&cnt->wait_clear, TYP_COUNTER);
   cnt->state = 0;

#if (TSK_NAMED)
   tsk_add_name (&cnt->name, name, TYP_COUNTER, cnt);
#endif

   return cnt;
}


/*
   delete_counter - kills all processes waiting for counter
*/

void Globalfunc delete_counter (counterptr cnt)
{
   CRITICAL;

   CHECK_EVTPTR (cnt, TYP_COUNTER, "Delete Counter");

   C_ENTER;
   tsk_kill_queue (&(cnt->wait_set));
   tsk_kill_queue (&(cnt->wait_clear));
   cnt->state = 0L;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&cnt->name);
#endif

#if (TSK_DYNAMIC)
   if (cnt->flags & F_TEMP)
      tsk_pfree (cnt);
#endif
}


/*
   clear_counter  - Sets counter to zero. All tasks waiting for
                    Counter zero are made eligible.
*/

void Globalfunc clear_counter (counterptr cnt)
{
   CRITICAL;

   CHECK_EVTPTR (cnt, TYP_COUNTER, "Clear Counter");

   C_ENTER;
   cnt->state = 0L;
   tsk_runable_all (&cnt->wait_clear);
   C_LEAVE;
}


/*
   wait_counter_set  - Wait until counter is != 0. If counter is != 0 on
                       entry, the counter is decremented and the task
                       continues to run. If the counter is decremented to 
                       zero, tasks waiting for zero are made eligible.
*/

int Globalfunc wait_counter_set (counterptr cnt, dword timeout)
{
   CRITICAL;

   CHECK_EVTPTR (cnt, TYP_COUNTER, "Wait Counter Set");

   C_ENTER;
   if (cnt->state)
      {
      if (!--cnt->state)
         tsk_runable_all (&cnt->wait_clear);
      C_LEAVE;
      return 0;
      }

   GLOBDATA current_task->retptr = LNULL;
   tsk_wait (&cnt->wait_set, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}

/*
   wait_counter_clear - Wait until counter is == 0. If counter is == 0 on
                       entry, the task continues to run.
*/

int Globalfunc wait_counter_clear (counterptr cnt, dword timeout)
{
   CRITICAL;

   CHECK_EVTPTR (cnt, TYP_COUNTER, "Wait Counter Clear");

   C_ENTER;
   if (!cnt->state)
      {
      C_LEAVE;
      return 0;
      }

   GLOBDATA current_task->retptr = LNULL;
   tsk_wait (&cnt->wait_clear, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}


/*
   inc_counter - Increment counter. If there are tasks waiting for the
                 set state, the first task in the queue is made eligible.
                 Returns new value of counter state.
*/

dword Globalfunc inc_counter (counterptr cnt)
{
   dword st;
   CRITICAL;

   CHECK_EVTPTR (cnt, TYP_COUNTER, "Inc Counter");

   C_ENTER;
   if (cnt->wait_set.first->kind & Q_HEAD)
      {
      st = ++cnt->state;
      C_LEAVE;
      return st;
      }
   tsk_runable ((tcbptr)cnt->wait_set.first);
   C_LEAVE;
   return 0L;
}


/*
   dec_counter - Decrement counter. If there are tasks waiting for the
                 zero state, all are made eligible.
                 Returns new value of counter state.
*/

dword Globalfunc dec_counter (counterptr cnt)
{
   dword st;
   CRITICAL;

   CHECK_EVTPTR (cnt, TYP_COUNTER, "Dec Counter");

   C_ENTER;
   if (cnt->state)
      {
      if ((st = --cnt->state) == 0)
         tsk_runable_all (&cnt->wait_clear);
      C_LEAVE;
      return st;
      }
   C_LEAVE;
   return 0L;
}


/*
   check_counter - return current counter state.
*/

dword Globalfunc check_counter (counterptr cnt)
{
   CHECK_EVTPTR (cnt, TYP_COUNTER, "Check Counter");
   return cnt->state;
}



/*
   set_counter  - Sets counter to given value. Depending on the value,
                  tasks waiting for counter zero or counter set are made
                  eligible.
*/

void Globalfunc set_counter (counterptr cnt, dword val)
{
   CRITICAL;

   CHECK_EVTPTR (cnt, TYP_COUNTER, "Set Counter");

   C_ENTER;
   while (val && !(cnt->wait_set.first->kind & Q_HEAD))
      {
      tsk_runable ((tcbptr)cnt->wait_set.first);
      val--;
      }
   if (!val)
      tsk_runable_all (&cnt->wait_clear);
   cnt->state = val;
   C_LEAVE;
}

