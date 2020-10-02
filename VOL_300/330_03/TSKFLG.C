/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKFLG.C - CTask - Flag handling routines.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany
*/

#include "tsk.h"
#include "tsklocal.h"


/*
   create_flag - initialises flag.
*/

flagptr Globalfunc create_flag (flagptr flg TN(byteptr name))
{
#if (TSK_DYNAMIC)
   if (flg == LNULL)
      {
      if ((flg = tsk_palloc (sizeof (flag))) == LNULL)
         return LNULL;
      flg->flags = F_TEMP;
      }
   else
      flg->flags = 0;
#endif

   tsk_init_qhead (&flg->wait_set, TYP_FLAG);
   tsk_init_qhead (&flg->wait_clear, TYP_FLAG);
   flg->state = 0;

#if (TSK_NAMED)
   tsk_add_name (&flg->name, name, TYP_FLAG, flg);
#endif

   return flg;
}

/*
   delete_flag - kills all processes waiting for flag
*/

void Globalfunc delete_flag (flagptr flg)
{
   CRITICAL;

   CHECK_EVTPTR (flg, TYP_FLAG, "Delete Flag");

   C_ENTER;

   tsk_kill_queue (&(flg->wait_set));
   tsk_kill_queue (&(flg->wait_clear));
   flg->state = 0;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&flg->name);
#endif

#if (TSK_DYNAMIC)
   if (flg->flags & F_TEMP)
      tsk_pfree (flg);
#endif
}


/*
   wait_flag_set  - Wait until flag is != 0. If flag is != 0 on
                    entry, the task continues to run.
*/

int Globalfunc wait_flag_set (flagptr flg, dword timeout)
{
   CRITICAL;

   CHECK_EVTPTR (flg, TYP_FLAG, "Wait Flag Set");

   C_ENTER;
   if (flg->state)
      {
      C_LEAVE;
      return 0;
      }
   GLOBDATA current_task->retptr = LNULL;
   tsk_wait (&flg->wait_set, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}


/*
   wait_flag_clear - Wait until flag is == 0. If flag is == 0 on
                     entry, the task continues to run.
*/

int Globalfunc wait_flag_clear (flagptr flg, dword timeout)
{
   CRITICAL;

   CHECK_EVTPTR (flg, TYP_FLAG, "Wait Flag Clear");

   C_ENTER;
   if (!flg->state)
      {
      C_LEAVE;
      return 0;
      }

   GLOBDATA current_task->retptr = LNULL;
   tsk_wait (&flg->wait_clear, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}


/*
   set_flag - Set flag to 1. If there are tasks waiting for the
              set state, all tasks in the queue are made eligible.
*/

void Globalfunc set_flag (flagptr flg)
{
   CRITICAL;

   CHECK_EVTPTR (flg, TYP_FLAG, "Set Flag");

   C_ENTER;
   flg->state = 1;
   tsk_runable_all (&flg->wait_set);
   C_LEAVE;
}


/*
   clear_flag - Set flag to 0. If there are tasks waiting for the
                clear state, all tasks in the queue are made eligible.
*/

void Globalfunc clear_flag (flagptr flg)
{
   CRITICAL;

   CHECK_EVTPTR (flg, TYP_FLAG, "Clear Flag");

   C_ENTER;
   flg->state = 0;
   tsk_runable_all (&flg->wait_clear);
   C_LEAVE;
}


/*
   clear_flag_wait_set - Set flag to 0, then wait for set state.
*/

int Globalfunc clear_flag_wait_set (flagptr flg, dword timeout)
{
   clear_flag (flg);
   return wait_flag_set (flg, timeout);
}


/*
   check_flag - return current flag state.
*/

int Globalfunc check_flag (flagptr flg)
{
   CHECK_EVTPTR (flg, TYP_FLAG, "Check Flag");

   return flg->state;
}



