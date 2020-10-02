/*
   --- Version 2.2 90-10-12 10:34 ---

   TSKTSUB.C - CTask - Timer/watch helper routines.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   This file is new with 2.1. The timer related helper functions were moved
   from tsktimer to this module.
*/

#include "tsk.h"
#include "tsklocal.h"

#include <stdarg.h>

/*
   tsk_setup_telem
      Initializes a timeout/watch element.
*/

tlinkptr Localfunc tsk_setup_telem (tlinkptr elem, byte kind, 
                                    farptr strucp, byte struckind, 
                                    dword upar)
{
#if (GROUPS)
   CRITICAL;
#endif

   if (struckind <= TKIND_TASK || struckind > TKIND_COUNTER)
      return LNULL;

#if (TSK_DYNAMIC)
   if (elem == LNULL)
      {
      if ((elem = tsk_palloc (sizeof (tlink))) == LNULL)
         return LNULL;
      elem->flags = TFLAG_TEMP;
      }
   else
      elem->flags = 0;
#else
   elem->flags = 0;
#endif

   elem->link.next = LNULL;
   elem->link.kind = kind;
   elem->struckind = struckind;
   elem->strucp = strucp;
   elem->user_parm = upar;
#if (GROUPS)
   C_ENTER;
   tsk_putqueue (&GLOBDATA current_task->group->telem_list, (queptr)&elem->chain);
   C_LEAVE;
#endif
   return elem;
}


/*
   enable_watch
      Enables a watch entry by enqueueing it into the watch queue.
*/

void Globalfunc enable_watch (tlinkptr elem)
{
   CRITICAL;

   CHECK_TELPTR (elem, TYP_WATCH, "Enable Watch");

   if (elem->link.next)
      return;
   C_ENTER;
   tsk_putqueue (&GLOBDATA watch_queue, &elem->link);
   C_LEAVE;
}


/*
   disable_watch
      Disables a watch entry by removing it from the watch queue.
*/

#if (CHECKING)
void Globalfunc disable_hwatch (tlinkptr elem, byte typ)
#else
void Globalfunc disable_watch (tlinkptr elem)
#endif
{
   CRITICAL;

   CHECK_TELPTR (elem, typ, "Disable Watch/Hotkey");

   C_ENTER;
   if (elem->flags & TFLAG_BUSY)
      elem->flags |= TFLAG_UNQUEUE;
   else
      tsk_dequeue (&elem->link);
   C_LEAVE;
}


/*
   delete_watch
      Deletes a watch element.
*/

#if (CHECKING)
void Globalfunc delete_hwatch (tlinkptr elem, byte typ)
#else
void Globalfunc delete_watch (tlinkptr elem)
#endif
{
   CRITICAL;

   CHECK_TELPTR (elem, typ, "Delete Watch/Hotkey");

   C_ENTER;
   if (elem->flags & TFLAG_BUSY)
      {
      elem->flags |= TFLAG_REMOVE;
      C_LEAVE;
      return;
      }

   tsk_dequeue (&elem->link);
#if (GROUPS)
   tsk_dequeue ((queptr)&elem->chain);
#endif
   C_LEAVE;

#if (TSK_DYNAMIC)
   if (elem->flags & TFLAG_TEMP)
      tsk_pfree (elem);
#endif
}

