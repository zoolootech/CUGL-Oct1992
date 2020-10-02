/*
   --- Version 2.2 90-10-12 10:33 ---

   CTask Subroutines

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   Note: The find_xxx_name functions were moved to a separate file,
   'tskname.c', in version 2.1 for potential code savings.
*/

#include "tsk.h"
#include "tsklocal.h"


/*
   tsk_runable
      Make a task eligible for running. The task is removed from the
      timer queue and enqueued in the eligible queue.

      CAUTION: Critical section assumed entered.
*/

void Localfunc tsk_runable (tcbptr task)
{
   CHECK_TCBPTR (task, "Task Runable");

   task->state = ST_ELIGIBLE;
   tsk_deqtimer (&task->timerq.link);
   tsk_dequeue (&task->cqueue);
   tsk_enqueue (task->qhead = &GLOBDATA eligible_queue, &task->cqueue);
}


/*
   tsk_run_pending
      Make a task eligible for running, but don't enqueue it in the
      eligible queue.

      CAUTION: Critical section assumed entered.
*/

void Localfunc tsk_run_pending (tcbptr task)
{
   CHECK_TCBPTR (task, "Task Runable");

   task->state = ST_ELIGIBLE;
   tsk_deqtimer (&task->timerq.link);
   tsk_dequeue (&task->cqueue);
}


/*
   tsk_runable_all
      Make all tasks in a queue eligible for running.

      CAUTION: Critical section assumed entered.
*/

void Localfunc tsk_runable_all (queheadptr que)
{
   queptr curr;
   tcbptr task;

   CHECK_QHEAD (que, "Task Runable All: Queue");

   for (curr = que->first; !(curr->kind & Q_HEAD); )
      {
      task = (tcbptr)curr;
      CHECK_TCBPTR (task, "Task Runable All: TCB");
      curr = curr->next;
      task->state = ST_ELIGIBLE;
      tsk_deqtimer (&task->timerq.link);
      tsk_enqueue (task->qhead = &GLOBDATA eligible_queue, &task->cqueue);
      }
   tsk_init_qhead (que, que->kind);
}


/*
   tsk_wait
      put current running task in wait state.

      With version 2.2, the wait action was changed to immediately
      enqueue the task into the respective queue. In previous versions,
      the task was not enqueued, this was done by the scheduler based 
      on the queue head pointer. Starting with version 2.1, interrupts 
      are enabled early in the scheduler, before the scheduler gets to 
      the point where it enqueues the task. This could lead to race 
      conditions, with tasks being only partially enqueued, or never 
      dequeued, when "tsk_runable" was called by an interrupt handler
      hitting in this interval.

      CAUTION: Critical section assumed entered.
*/

void Localfunc tsk_wait (queheadptr que, dword timeout)
{
   tcbptr curr;

   /*
      Note: The following test shouldn't be necessary. However,
      if ever an interrupt handler should cause the current task to
      be made waiting while the scheduler is active, this would cause
      catastrophic effects (also see the notes in tskasm.asm).
      Well, no interrupt handler should ever do such a nasty thing, but...
      So if the in_sched flag is set, we have to stop everything.
      The test is ommitted when we're not running under DOS.
   */

#if (DOS || DEBUG)
   if (GLOBDATA in_sched)
      tsk_fatal ("Wait while in Scheduler");
#endif

   CHECK_QHEAD (que, "Task Wait: Queue");

   curr = GLOBDATA current_task;
   curr->state = ST_WAITING;
   tsk_enqueue (curr->qhead = que, &curr->cqueue);
   if (timeout)
      {
      tsk_enqtimer (&curr->timerq.link, timeout);
      curr->timerq.flags = 0;
      curr->timerq.link.kind = TYP_TIMER;
      }
   schedule ();
}


#if (TSK_NAMED)

/*
   tsk_copy_name
      A replacement for strcpy, used for copying names. The C-runtime
      strcpy is not used here to keep the kernel model independent.
*/

void Localfunc tsk_copy_name (nameptr elem, byteptr name)
{
   byteptr n;
   int i;

   n = elem->name;
   if (name != LNULL)
      for (i = 0; i < 8; i++)
         if ((*n++ = *name++) == 0)
            break;
   *n = 0;
}


/*
   tsk_add_name
      Initialise name-list element and insert it into the name list.
      NOTE: no check is made for duplicate names; names are not sorted.
      Version 2.1 allows name pointers to be LNULL. Structures with LNULL
      name pointers are not added to the list.
*/

void Localfunc tsk_add_name (nameptr elem, byteptr name, byte kind, farptr strucp)
{
   CRITICAL;

   elem->list.kind = kind;
   elem->strucp = strucp;
   if (name == LNULL)
      {
      elem->list.first = LNULL;
      elem->name [0] = 0;
      return;
      }

   tsk_copy_name (elem, name);

   C_ENTER;
#if (GROUPS)
   tsk_putqueue (&GLOBDATA current_task->group->namelist.list, (queptr)&elem->list);
#else
   tsk_putqueue (&GLOBDATA name_list.list, (queptr)&elem->list);
#endif
   C_LEAVE;
}


/*
   tsk_del_name
      delete name-element from the name-list.
*/

void Localfunc tsk_del_name (nameptr elem)
{
   CRITICAL;

   C_ENTER;
   tsk_dequeue ((queptr)&elem->list);
   C_LEAVE;
}

#endif


/*
   tsk_init_qhead
      Initializes the head of a queue.
*/

void Localfunc tsk_init_qhead (queheadptr head, byte kind)
{
   head->kind = Q_HEAD | kind;
   head->first = head->last = (queptr)head;
}

