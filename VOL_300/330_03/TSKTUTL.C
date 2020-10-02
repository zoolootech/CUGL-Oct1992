/*
   --- Version 2.2 90-10-12 10:34 ---

   TSKTUTL.C - CTask - Task utilitiy routines.

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
   wake_task
      Restarts a task waiting for an event or timeout. 
      Returns -1 if the task was not waiting or stopped.
*/

int Globalfunc wake_task (tcbptr task)
{
   CRITICAL;

   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Wake Task");

   C_ENTER;
   if (task->state >= ST_ELIGIBLE)
      {
      C_LEAVE;
      return -1;
      }

   task->retptr = TWAKE;
   tsk_runable (task);
   C_LEAVE;
   return 0;
}


/*
   stop_task
      Sets a task to the stopped state.
*/

int Globalfunc stop_task (tcbptr task)
{
   CRITICAL;

   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Stop Task");

   if (task->state < ST_STOPPED)
      return -1;

   C_ENTER;
   task->state = ST_STOPPED;
   tsk_dequeue (&task->cqueue);
   task->qhead = LNULL;
   tsk_deqtimer (&task->timerq.link);
   C_LEAVE;

   return 0;
}


/*
   get_priority
      Returns the priority of a task.
*/

word Globalfunc get_priority (tcbptr task)
{
   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Get Priority");

   return task->cqueue.el.pri.ini_prior;
}


/*
   set_priority
      Changes the priority of a task. If the task is enqueued in a
      queue, its position in the queue is affected.
*/

void Globalfunc set_priority (tcbptr task, word prior)
{
   queheadptr que;
   CRITICAL;

   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Set Priority");

   C_ENTER;
   task->cqueue.el.pri.prior = task->cqueue.el.pri.ini_prior = prior;

   if ((task->state != ST_RUNNING) && ((que = task->qhead) != LNULL))
      {
      tsk_dequeue (&task->cqueue);
      tsk_enqueue (que, &task->cqueue);
      }
   C_LEAVE;
}


/*
   set_task_flags
      Changes the user modifiable flags of the task.
*/

void Globalfunc set_task_flags (tcbptr task, byte flags)
{
   CRITICAL;

   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Set Task Flags");
   C_ENTER;
   task->flags = (byte)((task->flags & FL_SYSM) | (flags & FL_USRM));
   C_LEAVE;
}


/*
   set_funcs
      Set the save/restore functions in the TCB.
*/

void Globalfunc set_funcs (tcbptr task, funcptr save, funcptr rest)
{
   CRITICAL;

   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Set Task Funcs");
   C_ENTER;
   task->save_func = save;
   task->rest_func = rest;
   C_LEAVE;
}


/*
   set_user_ptr
      Set the user pointer in the TCB.
*/

farptr Globalfunc set_user_ptr (tcbptr task, farptr uptr)
{
   farptr old;
   CRITICAL;

   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Set Task User Pointer");
   C_ENTER;
   old = task->user_ptr;
   task->user_ptr = uptr;
   C_LEAVE;

   return old;
}


/*
   get_user_ptr
      Returns the user pointer from the TCB.
*/

farptr Globalfunc get_user_ptr (tcbptr task)
{
   if (task == LNULL)
#if (GROUPS)
      task = GLOBDATA current_task->group->main_ptr;
#else
      task = GLOBDATA main_ptr;
#endif

   CHECK_TCBPTR (task, "Get Task User Pointer");
   return task->user_ptr;
}


/*
   curr_task
      Returns TCB of current running task.
*/

tcbptr Globalfunc curr_task (void)
{
   return GLOBDATA current_task;
}


