/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKMAIN.C - CTask - Install/remove tasker front-end functions.

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

   The split was made to allow better tuning of code space, and to
   better support code sharing and secondary invocations.

   For code share linking without group creation, none of the routines
   are needed, all necessary local code is in 'tskres.asm'.

   For secondary invocations with code sharing and group creation, only 
   the tskmain and tskgrp routines are needed in the secondary (plus
   the linkage routines in 'tskres'. All other routines can be shared.

   For secondary invocations without code sharing, some of the 
   initialization code is not needed (installing the primary kernel
   tasks and the interrupt handlers). This unneeded code is in 'tskinst',
   which will not be linked if the 'SECONDARY' define is specified on
   compilation of the tskmain module.
*/

/* --------------------------------------------------------------------- */

#include "tsk.h"
#include "tsklocal.h"
#include "tskdeb.h"

#if (DEBUG)
#include "tskprf.h"
#include "kbd.h"
#endif

/* 
   Installation flags. Local to the group.
*/

word Neardata tsk_instflags;

/* 
   Global variable block. For secondary invocations, only the 'group'
   gcb within this block is used. The real global variable block is
   always indirectly accessed through the tsk_global pointer, and is the
   same through all invocations.
   Interrupt handlers and other system-related routines that
   are only present in the primary invocation use this block directly.
*/

ctask_globvars Neardata tsk_glob_rec;

/*
   Ctask active flag. Is 1 when CTask was installed, 0 otherwise.
   Added in version 2.1 to support replacement alloc/free functions
   as suggested by Joe Urso.
*/

int ctask_active = 0;

/* --------------------------------------------------------------------- */

/*
   Debug support added in version 2.1 (moved here in version 2.2):
      The DEB_TSKDIS define enables scheduler task display,
      the DEB_FLASHERS define enables interrupt counting on the
      display. Each entry has an associated hotkey entry, and
      a common hotkey function, to toggle the display enable variable.
*/

#if (DEBUG & DEB_TSKDIS)
local tlink tdebhotkey;
int Neardata tsk_debtask = 1;
#endif
#if (DEBUG & DEB_FLASHERS)
local tlink fdebhotkey;
int Neardata tsk_debflash = 1;
#endif
#if (DEBUG)
dword Neardata tsk_dtposn;
#endif

#if ((DEBUG & (DEB_TSKDIS | DEB_FLASHERS)) && HOTKEYS)
void Taskfunc debhotkey_proc (tlinkptr elem)
{
   *((wordptr)elem->user_parm) = !*((wordptr)elem->user_parm);
}
#endif

/* --------------------------------------------------------------------- */

/*
   install_tasker
*/

int Globalfunc install_tasker (byte varpri, int speedup, word flags
                               TN(byteptr name))
{
#if (GROUPS)
   tcbptr ct;
#endif
   int rc;

   tsk_instflags = flags;

#if (DEBUG)
  #if (DEBUG & DEB_SECOND)
   tsk_set_dualdis ();
  #else
   tsk_set_currdis ();
  #endif
   if (tsk_regen_s)
      {
      tsk_setpos (0, 0);
      tsk_rputc ('\f');
      tsk_setpos (1, 0);
      tsk_dtposn = tsk_regen;
      }
#endif
#if (DEBUG & DEB_TSKDIS)
   if (tsk_regen_s)
      {
      tsk_setpos (0, 0);
      tsk_rprintf (DEBLINTD);
      }
#endif
#if (DEBUG & DEB_FLASHERS)
   if (tsk_regen_s)
      {
      tsk_setpos (0, DEBP_CNTSCHED / 2);
      tsk_rprintf (DEBLINFL);
      tsk_setpos (1, DEBP_CNTSCHED / 2);
      tsk_rprintf (DEBINIFL);
      }
#endif

#if (GROUPS)

   tsk_memcpy (tsk_glob_rec.id, tsk_version, 8);

   if (ctask_resident ())
      {
      tsk_glob_rec.group.main_ptr = ct = GLOBDATA current_task;
      tsk_create_group (&tsk_glob_rec.group, name);

#if (DOS)
      tsk_glob_rec.group.save_psp = ct->base_psp;
      tsk_glob_rec.group.save_sssp = ct->psp_sssp;
      ct->base_psp = tsk_getpsp ();
#endif

#if (TSK_DYNAMIC && TSK_DYNLOAD)
      create_resource (&alloc_resource TN("ALLOCRSC"));
#endif

      rc = 1;
      }
   else
      {
#if defined(SECONDARY)
      return -1;
#else
      tsk_install_main (varpri, speedup, flags TN(name));
      rc = 0;
#endif
      }
#else
   tsk_install_main (varpri, speedup, flags TN(name));
   rc = 0;
#endif /* groups */

   ctask_active = 1;

#if ((DEBUG & DEB_TSKDIS) && HOTKEYS)
   create_hotkey_entry (&tdebhotkey, SCAN_T, 
                        KF1_ALT | KF1_CONTROL | KF1_LEFTSHIFT, KF1_CONTROL | KF1_LEFTSHIFT,
                        0, 0,
                        0, 0,
                        (farptr)debhotkey_proc, TKIND_PROC, 1, (farptr)&tsk_debtask);
#endif
#if ((DEBUG & DEB_FLASHERS) && HOTKEYS)
   create_hotkey_entry (&fdebhotkey, SCAN_F, 
                        KF1_ALT | KF1_CONTROL | KF1_LEFTSHIFT, KF1_CONTROL | KF1_LEFTSHIFT,
                        0, 0,
                        0, 0,
                        (farptr)debhotkey_proc, TKIND_PROC, 1, (farptr)&tsk_debflash);
#endif

   return rc;
}


/*
   remove_tasker
      Front-end for remove_group/tsk_remove_tasker.
      Kills the current task group, and/or un-installs CTask.
*/

void Globalfunc remove_tasker (void)
{

#if (GROUPS)
   if (tsk_remove_group (GLOBDATA current_task->group, 0) == 1)
      {
#if defined(SECONDARY)
      return;
#else
      tsk_kill_group (GLOBDATA current_task->group);
      tsk_remove_tasker ();
#endif
      }
#else
   tsk_remove_tasker ();
#endif

   ctask_active = 0;
}

