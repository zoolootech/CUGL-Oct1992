/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKGRP.C - CTask - Create/Remove groups.

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
         tskmain.c - Install tasker and groups
         tskinst.c - Install/Remove main kernel
         tskgrp.c  - Create/remove groups
         tsktask.c - Task creation and deletion
         tsktutl.c - Task utilities (get/set priority etc.)
         tskutil.c - General utilities (preemption, t_delay)

   Note: This file will be skipped completely if groups are disabled.
*/

#include "tsk.h"
#include "tsklocal.h"

#if(GROUPS)

/* --------------------------------------------------------------------- */

typedef struct {
               word  int20;
               word  mem_end;
               byte  reserved1;
               byte  syscall [5];
               dword exit_addr;        /* Where DOS jumps to on EXIT */
               dword ctl_c_addr;
               dword crit_err_addr;
               word  parent_psp;
               byte  open_files [20];
               word  env_segment;
               dword caller_stack;     /* SS:SP of caller for SPAWN */
               word  filetab_len;      /* Number of entries in file table */
               dword filetab_ptr;      /* file table pointer */
               dword nested_psp;       /* whatever that's supposed to mean */
               } psp;

typedef psp far *pspptr;

/* --------------------------------------------------------------------- */


void Localfunc tsk_create_group (gcbptr group, byteptr name)
{
   pspptr pspp;
   word _psp;
   CRITICAL;

#if (DOS)
   _psp = tsk_getpsp ();
   pspp = TMK_FP(_psp, 0);
#endif

#if (TSK_DYNAMIC)
#if (TSK_DYNLOAD)
   group->palloc = tsk_alloc;
   group->pfree = tsk_free;
#else
   group->palloc = LNULL;
   group->pfree = LNULL;
#endif
#endif

   tsk_init_qhead (&group->namelist.list, TYP_GROUP);
   tsk_init_qhead (&group->telem_list, TYP_GROUP);
   tsk_init_qhead (&group->ticker_list, TYP_GROUP);
   tsk_copy_name (&group->namelist, name);
   group->creator = GLOBDATA current_task;
   group->branch = LNULL;
   group->home = GLOBDATA current_task->group;
   group->remove = LNULL;

   C_ENTER;
#if (DOS)
   group->exit_addr = pspp->exit_addr;
   group->create_psp = _psp;
   if (!(tsk_instflags & IFL_NOEXITCHECK))
      pspp->exit_addr = (dword)GLOBDATA emergency_exit;
#endif
   group->level = group->home->branch;
   group->home->branch = group;
   GLOBDATA current_task->group = group;
   C_LEAVE;
}


/* --------------------------------------------------------------------- */

/*
   tsk_kill_group
      Kills all tasks in the current group and unlinks all structures
      from the name list.
*/


void Localfunc tsk_kill_group (gcbptr group)
{
   nameptr curr;
   tcbptr tsk;
#if (GROUPS)
   queptr qptr;
#endif
#if (DOS)
   pspptr pspp;
#endif

   tsk_remove_chain (group->remove);
   tsk_dis_preempt ();

#if (DOS)
   pspp = TMK_FP (group->create_psp, 0);
   pspp->exit_addr = group->exit_addr;
   if (group->save_psp)
      {
      group->creator->base_psp = group->save_psp;
      group->creator->psp_sssp = group->save_sssp;
      pspp = TMK_FP (group->save_psp, 0);
      pspp->caller_stack = group->save_sssp;
      }

#endif

   curr = (nameptr)group->namelist.list.first;

   while (!(curr->list.kind & Q_HEAD))
      {
      tsk_del_name (curr);
      if (curr->list.kind == TYP_TCB)
         {
         tsk = (tcbptr)curr->strucp;
         if (tsk != GLOBDATA current_task && !(tsk->flags & F_PERM))
            kill_task (tsk);
         }

      curr = (nameptr)group->namelist.list.first;
      }

   qptr = group->telem_list.first;
   while (!(qptr->kind & Q_HEAD))
      {
      tsk_dequeue (qptr);
      tsk_deqtimer ((queptr)TSK_STRUCTOP(tlink,qptr,chain));
      qptr = group->telem_list.first;
      }

   qptr = group->ticker_list.first;
   while (!(qptr->kind & Q_HEAD))
      {
      delete_ticker (TSK_STRUCTOP(ticker,qptr,chain));
      qptr = group->ticker_list.first;
      }

   group->creator->group = group->home;

   tsk_ena_preempt ();
}


/*
   tsk_remove_group
      The current group is unlinked from the group chain, then
      all tasks in the current group, and in all subgroups,
      are killed via "tsk_kill_group".

      Returns  0 on normal completion,
               1 when last group must be removed (including the tasker),
              -1 when the group structure was messed up.

      CAUTION: This routine is for use by the TSKDOS and remove_tasker
               modules ONLY! Do not call it directly.
*/

int Localfunc tsk_remove_group (gcbptr group, int freemem)
{
   CRITICAL;
   gcbptr curr, last;

   C_ENTER;
   while (group->branch != LNULL)
      tsk_remove_group (group->branch, 1);

   if (group->home == LNULL)
      return 1;

   last = LNULL;
   curr = group->home->branch;

   while (curr != LNULL && curr != group)
      {
      last = curr;
      curr = curr->level;
      }
   if (curr == LNULL)
      return -1;

   if (last == LNULL)
      group->home->branch = group->level;
   else
      last->level = group->level;

   GLOBDATA current_task->group = group->home;

   tsk_kill_group (group);
#if (DOS)
   if (freemem)
      tsk_free_mem (group->create_psp);
#endif

   C_LEAVE;
   if (!freemem)
      preempt_on ();
   return 0;
}

#endif /* groups */

