/*
   --- Version 2.2 90-10-12 15:49 ---

   TSKINST.C - CTask - Install/Remove main tasker.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   Version 2.1 separates the functions previously collected in tskmain.c into
         tskmain.c - Front ends for installation/removal
         tskinst.c - Install/Remove main kernel
         tskgrp.c  - Create/remove groups
         tsktask.c - Task creation and deletion
         tsktutl.c - Task utilities (get/set priority etc.)
         tskutil.c - General utilities (preemption, t_delay)

   Added in version 2.1: Debug support and NDP checking.
*/

#include "tsk.h"
#include "tsklocal.h"

#define STACKSIZE 512

/*
   The tcb's of the standard tasks.

      timer_tcb   is the tcb for the timer task.
                  This task waits for the tsk_timer_counter, which is
                  increased on every timer tick. It processes the entries
                  in the timeout queue.

      int8_tcb    is the tcb for the int8 chain task.
                  This task waits for the tsk_int8_counter, which is
                  increased on every system timer tick. It then chains to
                  the previous timer interrupt entry. If the IFL_INT8_EARLY
                  installation flag is set, this task will only be awakened
                  if the timer interrupt occurs during scheduler execution.
                  If the INT8_EARLY configuration flag is TRUE, this tcb
                  is not created.

      main_tcb    is the "main" task which called "install_tasker". This
                  task has no separate stack, rather the stack on entry
                  to the scheduler is used. Since main_tcb is not used
                  if this is a secondary invocation of CTask, "main_ptr"
                  points to the main_tcb for a primary invocation, and
                  to the underlying task for a secondary invocation.

      kill_tcb    is the tcb for the "kill" task. This task is only
                  used when dynamic allocation is enabled. Its only
                  purpose is to dispose of a dynamically allocated TCB,
                  and/or a dynamically allocated task stack. This task
                  was added in version 2.1 to fix a problem with killing
                  the current task when there were dynamically allocated
                  elements in it.
*/

/* 
   The following variables are never directly accessed from shared code,
   so they can be defined locally.
*/

local tcb timer_tcb;
local tcb main_tcb;

#if (TSK_DYNAMIC)
local tcb kill_tcb;
local char kill_stack [STACKSIZE];
#endif

local char timer_stack [STACKSIZE];

#if (IBM && !INT8_EARLY)
local tcb int8_tcb;
local char int8_stack [STACKSIZE];
#endif

#if (CODE_SHARING)
extern byte tsk_jmptab [];
#endif

/* 
   The following variables are local to the primary invocation.
   They are referenced from the timer task.
*/

counter Neardata tsk_timer_counter;

#if (IBM && !INT8_EARLY)
counter Neardata tsk_int8_counter;
#endif

/* --------------------------------------------------------------------- */

#if (TSK_DYNAMIC)

/*
   tsk_kill is awakened directly by the internal task kill routines
   if the current task is to be killed, and it has dynamically 
   allocated elements in it. Since the freeing of the dynamic elements
   may cause the task to be made waiting, this can't be done immediately 
   in certain situations. So the task is only marked as killed, and its
   queue head pointer is set to the kill_queue entry in the global variable
   block. The tsk_kill task is then marked as runable. The scheduler will
   enter the TCB into the kill_queue, and eventually activate tsk_kill.
*/

local void Taskfunc tsk_kill (void)
{
   tcbptr task;
   CRITICAL;

   while (1)
      {
      C_ENTER;
      task = (tcbptr)tsk_glob_rec.kill_queue.first;
      while (!(task->cqueue.kind & Q_HEAD))
         {
         tsk_dequeue (&task->cqueue);
         C_LEAVE;
         if (task->flags & F_STTEMP)
            tsk_pfree (task->stkbot);   /* Bug fixed in 2.1, was 'task->stack' */
         if (task->flags & F_TEMP)
            tsk_pfree (task);
         C_ENTER;
         task = (tcbptr)tsk_glob_rec.kill_queue.first;
         }
      kill_tcb.state = ST_STOPPED;
      kill_tcb.qhead = LNULL;
      schedule ();
      C_LEAVE;
      }
}

#endif

/* --------------------------------------------------------------------- */

/*
   tsk_install_main
      Installs the Ctask system. The internal tasks are created,
      the queues are initialised, and the interrupt handler installation
      routines are called. Task preemption is initially off.

      Handling of the speedup parameter is system dependent.
*/

int Localfunc tsk_install_main (byte varpri, int speedup, word flags
                                TN(byteptr name))
{
   word divisor, sys_ticks;


   tsk_instflags = flags;

   tsk_glob_rec.current_task = &main_tcb;
#if (GROUPS)
   tsk_glob_rec.group.main_ptr = &main_tcb;
#else
   tsk_glob_rec.main_ptr = &main_tcb;
#endif

#if (!SINGLE_DATA)
   tsk_global = &tsk_glob_rec;
#endif
#if (GROUPS)
   main_tcb.group = &tsk_glob_rec.group;
#endif

#if (CODE_SHARING)
   tsk_glob_rec.stub_table = tsk_jmptab;
#else
   tsk_glob_rec.stub_table = LNULL;
#endif

   tsk_init_qhead (&tsk_glob_rec.eligible_queue, 0);
   tsk_init_qhead (&tsk_glob_rec.timer_queue, 0);
   tsk_init_qhead (&tsk_glob_rec.watch_queue, 0);

   tsk_glob_rec.preempt = 1;
   tsk_glob_rec.pretick = 0;
   tsk_glob_rec.var_prior = varpri;
   tsk_glob_rec.in_sched = 0;
   tsk_glob_rec.ticker_chain = LNULL;
#if (NDP)
   tsk_glob_rec.ndp_present = 0;  /* Init to 0 for creation of system tasks */
#endif

#if (GROUPS)
   tsk_glob_rec.group.branch = LNULL;
   tsk_create_group (&tsk_glob_rec.group, name);
   tsk_glob_rec.group.branch = LNULL;
   tsk_glob_rec.group.home = LNULL;

#if (DOS)
   tsk_glob_rec.group.save_psp = 0;
   tsk_glob_rec.emergency_exit = tsk_emergency_exit;

   tsk_install_dos ();
#endif
#else
   tsk_glob_rec.remove = LNULL;
#if (TSK_NAMED)
   tsk_init_qhead (&tsk_glob_rec.name_list.list, 0);
   tsk_glob_rec.name_list.name [0] = 0;
#endif
#endif
#if (EMS)
   tsk_install_ems ();
#endif

   /* 
      Call create_task to initialise the main task's TCB.
      Note that the function pointer and stack parameters are
      insignificant, but must be valid pointers (the stack is
      initialised, but not used).
   */

   create_task (&main_tcb, tsk_timer, timer_stack, STACKSIZE, 
                PRI_TIMER - 1, LNULL TN("-MAIN-"));

   main_tcb.qhead = &tsk_glob_rec.eligible_queue;
   main_tcb.state = ST_RUNNING;
   main_tcb.flags |= F_PERM;

#if (TSK_DYNAMIC && TSK_DYNLOAD)
   create_resource (&alloc_resource TN("ALLOCRSC"));
#endif

#if (TSK_DYNAMIC)
   tsk_init_qhead (&tsk_glob_rec.kill_queue, 0);
   create_task (&kill_tcb, tsk_kill, kill_stack, STACKSIZE, 
                PRI_STD, LNULL TN("-KILLER-"));
   kill_tcb.flags |= F_PERM;
   tsk_glob_rec.kill_task = &kill_tcb;
#endif

   create_task (&timer_tcb, tsk_timer, timer_stack, STACKSIZE, 
                PRI_TIMER, LNULL TN("-TIMER-"));
   timer_tcb.flags |= F_PERM;

   create_counter (&tsk_timer_counter TN("TIMCOUNT"));

   start_task (&timer_tcb);

#if (IBM && !INT8_EARLY)

   /* Note: to allow delayed timer ticks even with the INT8_DIR
      installation flag active, we must create the int8 task and
      counter without checking for the flag. Delayed ticks are 
      possible under the new interrupt handler scheme in version 2.1.
   */

   create_task (&int8_tcb, tsk_int8, int8_stack, STACKSIZE, PRI_INT8, LNULL
                  TN("-INT8-"));
   int8_tcb.flags |= F_PERM;

   create_counter (&tsk_int8_counter TN("INT8CNT"));
   start_task (&int8_tcb);

#endif

   /* NOTE: divisor/sys_ticks calculations are machine dependent. */

   if (speedup <= 0 || speedup > 8)
      {
      divisor = 0;
      sys_ticks = 1;
      tsk_glob_rec.ticks_per_sec = 0x0012;
      tsk_glob_rec.tick_factor = 0x36ED;
      }
   else
      {
      divisor = 1 << (16 - speedup);
      sys_ticks = 1 << speedup;
      tsk_glob_rec.ticks_per_sec =
         (0x1235 + (0x0080 >> speedup)) >> (8 - speedup);
      tsk_glob_rec.tick_factor =
         (0x36ED + (0x0080 >> (8 - speedup))) >> speedup;
      }

   tsk_install_timer (divisor, sys_ticks);

#if (HOTKEYS)
   tsk_init_qhead (&tsk_glob_rec.hotkey_scan, 0);
   tsk_init_qhead (&tsk_glob_rec.hotkey_noscan, 0);
#endif

#if (IBM)
   tsk_install_kbd ();

   if (flags & IFL_PRINTER)
      tsk_install_int17 ();
#endif

#if (AT_BIOS)
   if (flags & IFL_INT15)
      tsk_install_bios ();
#endif

#if (NDP)
   tsk_glob_rec.ndp_present = tsk_use_ndp = tsk_check_ndp ();
#endif

   return 0;
}


/* --------------------------------------------------------------------- */

/*
   tsk_remove_chain
      Calls the chained remove routines.
*/

void Localfunc tsk_remove_chain (callchainptr chain)
{
   callchainptr curr;
   funcptr_ccp fn;

   while (chain != LNULL)
      {
      curr = chain;
      chain = chain->next;
      if ((fn = curr->func) != LNULL)
         {
         curr->func = LNULL;
         tsk_callfunc ((farptr)fn, curr);
         }
#if (TSK_DYNAMIC)
      if (curr->flags & F_TEMP)
         tsk_pfree (curr);
#endif
      }
}


/*
   tsk_remove_tasker
      Calls the interrupt handler un-install routines.
*/

void Localfunc tsk_remove_tasker (void)
{
#if (!GROUPS)
   tsk_remove_chain (tsk_glob_rec.remove);
#endif

   tsk_glob_rec.preempt = 0;

#if (AT_BIOS)
   if (tsk_instflags & IFL_INT15)
      tsk_remove_bios ();
#endif

#if (IBM && !INT8_EARLY)

   /* Allow all stored clock ticks to be processed */

   if (!(tsk_instflags & IFL_INT8_DIR))
      {
      set_priority (&int8_tcb, 0xffff);
      while (check_counter (&tsk_int8_counter))
         schedule();
      }
#endif

#if (IBM)
   if (tsk_instflags & IFL_PRINTER)
      tsk_remove_int17 ();

   tsk_remove_kbd ();
#endif

   tsk_remove_timer ();

#if (DOS)
   tsk_remove_dos ();
#endif

   ctask_active = 0;
}

