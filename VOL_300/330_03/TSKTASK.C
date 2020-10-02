/*
   --- Version 2.2 90-10-12 15:47 ---

   TSKTASK.C - CTask - Task creation and deletion.

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
   tsk_use_ndp is initialized by install_tasker, and may be changed
   to 0 by the user to enable or disable ndp handling for tasks
   being created.

   Note that setting tsk_use_ndp to 1 will have no effect if 
   install_tasker has determined that there is no NDP present, or
   if the NDP configuration flag is FALSE.
*/

int tsk_use_ndp = 0;


#pragma check_stack(off)

/*
   tsk_kill
      mark task as killed.

      CAUTION: Critical section assumed entered.
*/

local void Staticfunc tsk_kill (tcbptr task, int currtask)
{
   /*
      Note: The following test shouldn't be necessary. However,
      if ever an interrupt handler should cause the current task to
      be killed while the scheduler is active, this would cause
      catastrophic effects (also see the notes in tskasm.asm).
      Well, no interrupt handler should ever do such a nasty thing, but...
      So if the in_sched flag is set, and the task is the current
      task, we have to stop everything.
      The test is ommitted when we're not running under DOS.
   */

#if (DOS)
   if (currtask && GLOBDATA in_sched)
      tsk_fatal ("Kill while in Scheduler");
#endif

   task->state = ST_KILLED;
   task->qhead = LNULL;
   tsk_deqtimer (&task->timerq.link);

#if (TSK_NAMED)
   tsk_dequeue ((queptr)&task->name.list);
#endif

#if (TSK_DYNAMIC)
   if (!currtask)
      {
      if (task->flags & F_STTEMP)
         tsk_pfree (task->stkbot);   /* Bug fixed in 2.1, was 'task->stack' */
      if (task->flags & F_TEMP)
         tsk_pfree (task);
      }
   else if (task->flags & (F_STTEMP | F_TEMP))
      {
      task->qhead = &GLOBDATA kill_queue;
      if (GLOBDATA kill_task->state == ST_STOPPED)
         tsk_runable (GLOBDATA kill_task);
      }
#endif

   if (currtask)
      schedule ();
}

/*
   Killretn kills the current active task. It is used internally, but
   can also be called from outside. (However, *never* call it from an 
   interrupt handler!)
*/

void Taskfunc killretn (void)
{
   tsk_cli ();
   tsk_kill (GLOBDATA current_task, 1);
}


/*
   tsk_kill_queue
      Removes all tasks from a queue, and marks queue invalid. 
      For internal use only.

      CAUTION: Critical section assumed entered.
*/

void Localfunc tsk_kill_queue (queheadptr que)
{
   queptr curr, next;

   CHECK_QHEAD (que, "Task Kill Queue");

   for (curr = que->first; !(curr->kind & Q_HEAD); )
      {
      next = curr->next;
      tsk_kill ((tcbptr)curr, 0);
      curr = next;
      }
   tsk_init_qhead (que, 0xff);
}


/* ---------------------------------------------------------------------- */

/*
   create_task
      Initialises a tcb. The task is in stopped state initially.
*/

tcbptr Globalfunc create_task (tcbptr task,
                               funcptr func,
                               byteptr stack,
                               word stksz,
                               word prior,
                               farptr arg
                               TN(byteptr name)
                               )

{
   struct task_stack far *stk;
#if (CHECKING)
   word i;
#endif

#if (TSK_DYNAMIC)
   if (task == LNULL)
      {
      if ((task = (tcbptr) tsk_palloc (sizeof(tcb))) == LNULL)
         return LNULL;
      task->flags = F_TEMP;
      }
   else
      task->flags = 0;

   if (stack == LNULL)
      {
      if ((stack = (byteptr) tsk_palloc (stksz)) == LNULL)
         {
         if (task->flags & F_TEMP)
            tsk_pfree (task);
         return LNULL;
         }
      task->flags |= F_STTEMP;
      }
#else
   task->flags = 0;
#endif

   stk = (struct task_stack far *)(stack + stksz - sizeof (struct task_stack));
   stk->r_ds = task->t_es = tsk_dseg ();
   stk->r_flags = tsk_flags ();
   stk->retn = func;
   stk->dummyret = killretn;
   stk->arg = arg;
   task->t_bp = 0;

   task->stkbot = stack;
#if (CHECKING)
   for (i = 0; i < stksz - sizeof (struct task_stack); i++)
      *stack++ = (byte)i;
#endif
   task->stack = (byteptr) stk;
   task->cqueue.kind = TYP_TCB;
   task->cqueue.next = LNULL;
   task->qhead = LNULL;
   task->state = ST_STOPPED;
   task->cqueue.el.pri.prior = task->cqueue.el.pri.ini_prior = prior;
   task->timerq.link.el.ticks = task->timerq.elem.time.reload = 0L;
   task->timerq.link.kind = TYP_TIMER;
   task->timerq.link.next = LNULL;
   task->timerq.strucp = (farptr) task;
   task->timerq.struckind = TKIND_WAKE;
   task->timerq.flags = 0;
   task->save_func = task->rest_func = LNULL;
#if (DOS)
   task->sched_ent_func = LNULL;
#endif
   task->user_ptr = LNULL;

#if (GROUPS)
   task->group = task->homegroup = GLOBDATA current_task->group;
#endif
#if (DOS)
   task->indos = 0;
   task->t_new = 1;
   task->base_psp = task->group->create_psp;
#endif

#if (TSK_NAMED)
   tsk_add_name (&task->name, name, TYP_TCB, task);
#endif

#if (EMS)
   if (GLOBDATA ems_savetsk != LNULL)
      GLOBDATA ems_savetsk (task);
#endif

#if (NDP)
   if (tsk_use_ndp && GLOBDATA ndp_present)
      task->flags |= F_USES_NDP;
#endif

   return task;
}


/*
   kill_task
      Removes a task from the system.
*/

void Globalfunc kill_task (tcbptr task)
{
   CRITICAL;

   CHECK_TCBPTR (task, "Kill Task");

   C_ENTER;
   if (task != GLOBDATA current_task)
      {
      if (task->state == ST_KILLED)
         return;
      else
         tsk_dequeue (&task->cqueue);

      tsk_kill (task, 0);
      }
   else
      tsk_kill (task, 1);

   C_LEAVE;
}


/*
   start_task
      Starts a stopped task. Returns -1 if the task was not stopped.
*/

int Globalfunc start_task (tcbptr task)
{
   CRITICAL;

   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Start Task");

   if (task->state == ST_STOPPED)
      {
      task->state = ST_ELIGIBLE;
      C_ENTER;
      tsk_runable (task);
      C_LEAVE;
      return 0;
      }
   return -1;
}


