/*
   --- Version 2.1 90-10-12 10:34 ---

   TSKLOCAL.H - CTask - Internal definitions and prototypes.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   CAUTION: Routines defined here may *not* be called from outside the
            CTask kernel.
*/

/* Miscellaneous internal definitions */

typedef void (cdecl far *funcptr_void_fp)(farptr);

#define TERR_WAIT_SCHED 1


/*
   struct task_stack describes the contents of a tasks stack after creation.
   The first 2 words are the registers to be restored by the scheduler.
   Only the segment register is significant initially.
   The next three words contain the function address plus the CPU flags
   setup as if an interrupt had occurred at the function's entry address.

   This setup is the same as the stack of an interrupted task after
   scheduling.

   The following two words contain a dummy return address, which points
   to the routine "killretn". Thus, if the task main function should ever
   return, the task is automatically killed. The last doubleword is
   used for the optional argument to the task.
*/

struct task_stack {
                  word     r_bx;
                  word     r_ds;
                  funcptr  retn;
                  word     r_flags;
                  funcptr  dummyret;
                  farptr   arg;
                  };

/* --------------------------------------------------------------------- */

/*
                     The global variable block

   The task queues:

      timer_queue       All tasks using a timeout, either through "t_delay" 
                        or an event wait, are enqueued into "timer_queue",
                        using the "timerq" link.

      eligible_queue    All tasks eligible for running are enqueued 
                        in "eligible_queue".

      current_task      Points to the current running task's tcb.

   System flags:

      preempt           is zero if preemption is allowed.
                        Bit 0 is set if preemption has been disabled globally.
                        Bit 1 is set for temporary disabling preemption.
                        Temporary preemption is automatically removed by the
                        scheduler.

      pretick           is nonzero if a schedule request from an 
                        interrupt handler was rejected due to 
                        tsk_preempt nonzero. This allows an immediate 
                        scheduling whenever tsk_preempt is set to 0.

      var_prior         Can be set nonzero to enable variable priority.
                        Variable priority will increase the priority of
                        eligible tasks on each scheduler call while they
                        are waiting to be executed, so that low priority 
                        tasks will slowly get to the head of the eligible
                        queue, getting a chance to be run. With variable
                        priority off, lower priority tasks will never be
                        executed while higher priority tasks are eligible.
                    
   System variables:

      in_sched          is used in the scheduler only. It is nonzero while
                        the scheduler is active.

      tick_factor       is undefined if CLOCK_MSEC is 0, otherwise it 
                        contains the clock tick factor used by tsk_timeout.

      ticks_per_sec     is the approximate number of ticks per second
                        expressed as an unsigned integer.

      stub_table        is a pointer to the entry stub table if
                        code sharing is enabled, else it is NULL.

      l_swap            is set by tskdos and used by the scheduler.
                        It contains the length of the DOS swap area.
                        (DOS only)

      dos_vars          contains the address of the DOS swap area.
                        (DOS only)

      dos_in_use        contains the address of the DOS in-use flag.
                        (DOS only)

      groups            is the group list head. It points to the most 
                        recently created group.

      name_list         is only present if TSK_NAMED is nonzero, and
                        if there are no groups.
                        It is the head of the name queue.

      kill_queue        is a queue of TCBs waiting to be released
                        after being killed.

      kill_task         is a pointer to the TCB of the special killer
                        task.

      hotkey_scan       is the list of hotkey entries with non-zero
                        scan-code field.

      hotkey_noscan     is the list of hotkey entries with zero
                        scan-code field.

      ems_save          is a pointer to the EMS context save routine.
      ems_rest          is a pointer to the EMS context restore routine.
      ems_savetsk       is a pointer to the store EMS context in TCB routine.

      emergency_exit    points to the emergency exit routine.

      main_ptr          points to the TCB of the main task (non-group only).

      remove            is a pointer to the remove chain (non-group only).

      ndp_present       is non-zero if a numeric coprocessor is installed.
*/

typedef struct {
               char        id [8];     /* contains version string */

               tcbptr      current_task;
               queue_head  eligible_queue;

               queue_head  timer_queue;
               queue_head  watch_queue;

               byte        preempt;
               byte        pretick;
               byte        var_prior;

               byte        in_sched;

               word        tick_factor;
               word        ticks_per_sec;
               tick_ptr    ticker_chain;

               farptr      stub_table;

#if (TSK_DYNAMIC)
               queue_head  kill_queue;
               tcbptr      kill_task;
#endif

#if (HOTKEYS)
               queue_head  hotkey_scan;
               queue_head  hotkey_noscan;
#endif

#if (EMS)
               farptr      ems_save;
               farptr      ems_rest;
               funcptr_void_fp ems_savetsk;
#endif
#if (DOS)
               funcptr     emergency_exit;
               word        l_swap;
               dword       dos_vars;
               dword       dos_in_use;
#endif
#if (GROUPS)
               gcb         group;
#else
               tcbptr      main_ptr;
               callchainptr remove;
#if (TSK_NAMED)
               namerec     name_list;
#endif
#endif
#if (NDP)
               byte        ndp_present;
#endif
               } ctask_globvars;

typedef ctask_globvars far *globvarptr;

/* ------------------ CTask-internal variables ------------------------- */

extern counter Neardata tsk_timer_counter;
extern word Neardata tsk_instflags;
extern char Neardata tsk_version [];

#if (DOS)
extern counter Neardata tsk_int8_counter;
#endif

extern ctask_globvars Neardata tsk_glob_rec;
#if (!SINGLE_DATA)
extern globvarptr Neardata tsk_global;
#endif

#if (SINGLE_DATA)
#define  GLOBDATA        tsk_glob_rec.
#else
#define  GLOBDATA        tsk_global->
#endif

/* -------- CTask-internal functions (don't use in applications) ------- */

/*
   Allocation routines are local to a group. If groups are defined,
   alloc/free must be called indirectly through the pointer in the
   GCB for code sharing to access the correct routines.
*/

#if (TSK_DYNAMIC && GROUPS)
#define tsk_palloc(x)   (tsk_global->current_task->group->palloc (x))
#define tsk_pfree(x)    (tsk_global->current_task->group->pfree (x))
#elif (TSK_DYNAMIC)
#define tsk_palloc(x)   tsk_alloc(x)
#define tsk_pfree(x)    tsk_free(x)
#endif


/* module tskasm */

#if (TSK_TURBO)
#define tsk_dseg()   _DS
#else
extern word Globalfunc tsk_dseg (void);
#endif
extern word Globalfunc tsk_flags (void);
extern void Localfunc tsk_callfunc (farptr funcad, farptr param);
extern void Globalfunc tsk_memcpy (farptr dest, farptr src, word len);

#if (CLOCK_MSEC)
extern dword Localfunc tsk_timeout (dword tout);
#else
#define tsk_timeout(tout)  tout
#endif


/* module tsktask */

extern void Localfunc tsk_kill_queue (queheadptr que);

/* module tsksub */

extern void Localfunc tsk_runable (tcbptr task);
extern void Localfunc tsk_run_pending (tcbptr task);
extern void Localfunc tsk_runable_all (queheadptr que);
extern void Localfunc tsk_wait (queheadptr que, dword timeout);
extern void Localfunc tsk_init_qhead (queheadptr head, byte kind);
#define tsk_eligible(task) tsk_enqueue(task->qhead = &GLOBDATA eligible_queue, &task->cqueue)

#if (TSK_NAMED)
extern void Localfunc tsk_copy_name (nameptr elem, byteptr name);
extern void Localfunc tsk_add_name (nameptr elem, byteptr name, byte kind,
                                    farptr strucp);
extern void Localfunc tsk_del_name (nameptr elem);
#endif


/* module tskque */

extern void Localfunc tsk_enqueue (queheadptr head, queptr elem);
extern void Localfunc tsk_dequeue (queptr elem);
extern void Localfunc tsk_enqtimer (queptr elem, dword tout);
extern void Localfunc tsk_deqtimer (queptr elem);
extern void Localfunc tsk_putqueue (queheadptr q, queptr elem);


/* module tsktim */

extern void Localfunc tsk_install_timer (word divisor, word sys_ticks);
extern void Localfunc tsk_remove_timer (void);
extern void Localfunc tsk_chain_timer (void);


/* module tskttsk */

#if (IBM && !INT8_EARLY)
extern void Taskfunc tsk_int8 (void);
#endif
extern void Taskfunc tsk_timer (void);


/* module tsktsub */

extern tlinkptr Localfunc tsk_setup_telem (tlinkptr elem, byte kind, 
                                           farptr strucp, byte struckind, 
                                           dword upar);

/* module tskdos */

#if (DOS)
extern void Localfunc tsk_install_dos (void);
extern void Localfunc tsk_remove_dos (void);
extern void far cdecl tsk_emergency_exit (void);
extern void Localfunc tsk_fatal (byteptr errmsg);
#endif


/* module tskkbd */

#if (IBM)
extern void Localfunc tsk_install_kbd (void);
extern void Localfunc tsk_remove_kbd (void);
#endif


/* module tskbios */

#if (AT_BIOS)
extern void Localfunc tsk_install_bios (void);
extern void Localfunc tsk_remove_bios (void);
#endif


/* module tskint17 */

#if (IBM)
extern void Localfunc tsk_install_int17 (void);
extern void Localfunc tsk_remove_int17 (void);
#endif


/* module tskems */

#if (EMS)
extern int Localfunc tsk_install_ems (void);
#endif


/* module tsksec */

#if (DOS)
extern void Localfunc tsk_free_mem (word psp);
extern word Localfunc tsk_getpsp (void);
#endif


/* module tskgrp */

#if (GROUPS)
extern void Localfunc tsk_create_group (gcbptr group, byteptr name);
extern void Localfunc tsk_kill_group (gcbptr group);
extern int Localfunc tsk_remove_group (gcbptr group, int freemem);
#endif


/* module tskinst */

extern int Localfunc tsk_install_main (byte varpri, int speedup, 
                                        word flags TN(byteptr name));
extern void Localfunc tsk_remove_chain (callchainptr chain);
extern void Localfunc tsk_remove_tasker (void);


/* module tskndp */

extern int Localfunc tsk_check_ndp (void);


#if (CHECKING)

extern void CGlobalfunc tsk_fatal_pmd (byteptr txt, ...);
#define TFPMD(txt,ptr)  tsk_fatal_pmd ("%s - Invalid pointer %FP",txt,ptr)

#if (TSK_NAMED)
#define CHECK_TCBPTR(ptr,txt) {if(ptr == LNULL || ptr->cqueue.kind != TYP_TCB || ptr->name.list.kind != TYP_TCB) TFPMD(txt,ptr);}
#else
#define CHECK_TCBPTR(ptr,txt) {if(ptr == LNULL || ptr->cqueue.kind != TYP_TCB) TFPMD(txt,ptr);}
#endif
#define CHECK_QHEAD(ptr,txt) {if(ptr == LNULL || !(((queptr)ptr)->kind & Q_HEAD)) TFPMD(txt,ptr);}
#define CHECK_EVTPTR(ptr,knd,txt) {if(ptr == LNULL || ((queptr)ptr)->kind != (knd | Q_HEAD)) TFPMD(txt,ptr);}
#define CHECK_TELPTR(ptr,knd,txt) {if(ptr == LNULL || ptr->link.kind != knd) TFPMD(txt,ptr);}

#else

#define CHECK_TCBPTR(ptr,txt)
#define CHECK_QHEAD(ptr,txt)
#define CHECK_EVTPTR(ptr,knd,txt)
#define CHECK_TELPTR(ptr,knd,txt)

#endif

