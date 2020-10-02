/*
   --- Version 2.2 90-10-12 10:34 ---

   TSKUTIL.C - CTask - Utilitiy routines.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   This file is new with version 2.1.
   Version 2.1 separates the functions previously collected in tskmain.c into
         tskmain.c - Front ends for installation/removal
         tskinst.c - Install/Remove main kernel
         tskgrp.c  - Create/remove groups
         tsktask.c - Task creation and deletion
         tsktutl.c - Task utilities (get/set priority etc.)
         tskutil.c - General utilities (preemption, t_delay)
*/

#include "tsk.h"
#include "tsklocal.h"


/*
   preempt_off
      Turns off task preemption (will stay off until explicitly enabled).
*/

void Globalfunc preempt_off (void)
{
   GLOBDATA preempt = 1;
}


/*
   preempt_on
      Resets permanent and temporary task preemption flag. If 
      preemption is pending, the scheduler is called.
*/

void Globalfunc preempt_on (void)
{
   GLOBDATA preempt = 0;
   tsk_cli ();
   if (GLOBDATA pretick)
      schedule ();
   tsk_sti ();
}


/*
   tsk_ena_preempt
      Resets temporary task preemption flag. If preemption is pending,
      the scheduler is called.
*/

void Globalfunc tsk_ena_preempt (void)
{
   tsk_cli ();
   if (!(GLOBDATA preempt &= ~2))
      if (GLOBDATA pretick)
         schedule ();
   tsk_sti ();
}


/*
   tsk_dis_preempt
      Sets temporary task preemption flag.
*/

void Globalfunc tsk_dis_preempt (void)
{
   GLOBDATA preempt |= 2;
}


/*
   t_delay
      delay the current task by "ticks" clock ticks.
      If ticks is zero, the task is stopped.
*/

int Globalfunc t_delay (dword ticks)
{
   tcbptr task = GLOBDATA current_task;

   tsk_cli ();
   tsk_dequeue (&task->cqueue);
   tsk_deqtimer (&task->timerq.link);
   task->qhead = LNULL;
   if (ticks)
      {
      task->state = ST_DELAYED;
      task->timerq.flags = 0;
      task->timerq.link.kind = TYP_TIMER;
      tsk_enqtimer (&task->timerq.link, ticks);
      }
   else
      task->state = ST_STOPPED;

   schedule ();
   return (int)((dword)task->retptr);
}


/*
   chain_removefunc
      Enters the passed function into the remove function list,
      returns the chain pointer, or LNULL on error.
*/

callchainptr Globalfunc chain_removefunc (funcptr_ccp func, callchainptr chain, farptr user_ptr)
{
#if (TSK_DYNAMIC)
   if (chain == LNULL)
      {
      if ((chain = tsk_palloc (sizeof (callchain))) == LNULL)
         return LNULL;
      chain->flags = F_TEMP;
      }
   else
      chain->flags = 0;
#endif
   chain->func = func;
   chain->user_ptr = user_ptr;
#if (GROUPS)
   chain->next = GLOBDATA current_task->group->remove;
   GLOBDATA current_task->group->remove = chain;
#else
   chain->next = GLOBDATA remove;
   GLOBDATA remove = chain;
#endif
   return chain;
}

/*
   unchain_removefunc
      Removes the passed chain pointer from the remove list.
*/

void Globalfunc unchain_removefunc (callchainptr chain)
{
   callchainptr last, curr;

   if (chain->func == LNULL)
      return;

#if (GROUPS)
   last = (callchainptr)&GLOBDATA current_task->group->remove;
#else
   last = (callchainptr)&GLOBDATA remove;
#endif
   do
      {
      curr = last->next;
      }
   while (curr != LNULL && curr != chain);
   if (curr == LNULL)
      return;
   last->next = curr->next;
   curr->func = LNULL;

#if (TSK_DYNAMIC)
   if (curr->flags & F_TEMP)
      tsk_pfree (curr);
#endif
}


word Globalfunc ticks_per_sec (void)
{
   return GLOBDATA ticks_per_sec;
}

