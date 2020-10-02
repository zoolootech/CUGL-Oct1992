/*
   --- Version 2.2 90-10-12 10:34 ---

   TSKTOPS.C - CTask - Timer operations.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   This file is new with 2.1. The timer related functions were moved
   from tsktimer to this module.

   Timer logic has been significantly changed in version 2.0.
*/

#include "tsk.h"
#include "tsklocal.h"

#include <stdarg.h>

/*
   create_timer_elem
      Creates a timer queue timeout element.
*/

tlinkptr CGlobalfunc create_timer_elem (tlinkptr elem, dword tout, 
                                        farptr strucp, byte kind, 
                                        int rept, ...)
{
   va_list val;

   va_start (val, rept);
   elem = tsk_setup_telem (elem, TYP_TIMER, strucp, kind, va_arg (val, dword));
   va_end (val);
   if (elem == LNULL)
      return LNULL;

   if (rept)
      elem->flags |= TFLAG_REPEAT;
   elem->elkind = TELEM_TIMER;
   elem->elem.time.reload = tsk_timeout(tout);

   return elem;
}


/*
   enable_timer
      The timer element is inserted into the timeout queue with the
      timer reload value.
*/

void Globalfunc enable_timer (tlinkptr elem)
{
   dword tout;
   CRITICAL;

   CHECK_TELPTR (elem, TYP_TIMER, "Enable Timer");

   if (elem->link.next)
      return;

   tout = elem->elem.time.reload;
   if (tout)
      {
      C_ENTER;
      if (elem->flags & TFLAG_BUSY)
         elem->flags |= TFLAG_ENQUEUE;
      else
         tsk_enqtimer (&elem->link, tout);
      C_LEAVE;
      }
}


/*
   disable_timer
      The timer element is removed from the timeout queue.
*/

void Globalfunc disable_timer (tlinkptr elem)
{
   CRITICAL;

   CHECK_TELPTR (elem, TYP_TIMER, "Disable Timer");

   C_ENTER;
   if (elem->flags & TFLAG_BUSY)
      elem->flags |= TFLAG_UNQUEUE;
   else
      tsk_deqtimer (&elem->link);
   C_LEAVE;
}


/*
   create_timer
      Creates a timer queue timeout element. The element is inserted into
      the timeout queue.
*/

tlinkptr CGlobalfunc create_timer (tlinkptr elem, dword tout, farptr strucp,
                                   byte kind, int rept, ...)
{
   va_list val;

   va_start (val, rept);
   elem = create_timer_elem (elem, tout, strucp, kind, rept, va_arg (val, dword));
   va_end (val);
   if (elem != LNULL)
      enable_timer (elem);

   return elem;
}


/*
   delete_timer
      Deletes a timeout element.
*/

void Globalfunc delete_timer (tlinkptr elem)
{
   CRITICAL;

   CHECK_TELPTR (elem, TYP_TIMER, "Delete Timer");

   C_ENTER;
   if (elem->flags & TFLAG_BUSY)
      {
      elem->flags |= TFLAG_REMOVE;
      C_LEAVE;
      return;
      }

   tsk_deqtimer (&elem->link);
#if (GROUPS)
   tsk_dequeue ((queptr)&elem->chain);
#endif
   C_LEAVE;

#if (TSK_DYNAMIC)
   if (elem->flags & TFLAG_TEMP)
      tsk_pfree (elem);
#endif
}


/*
   change_timer
      Changes the timeout and/or repeat-flag in a timer element.
      If the timer was idle, it is inserted into the timeout queue.

      If 0 is passed as timeout, the element is removed from the
      timeout queue (same as disable_timer).
*/

void CGlobalfunc change_timer (tlinkptr elem, dword tout, int rept, ...)
{
   va_list val;
   CRITICAL;

   CHECK_TELPTR (elem, TYP_TIMER, "Change Timer");

   if (!tout)
      {
      disable_timer (elem);
      return;
      }

   va_start (val, rept);
   C_ENTER;
   elem->user_parm = va_arg (val, dword);
   va_end (val);
   tout = elem->elem.time.reload = tsk_timeout(tout);
   if (rept)
      elem->flags |= TFLAG_REPEAT;
   else
      elem->flags &= ~TFLAG_REPEAT;

   if (elem->flags & TFLAG_BUSY)
      elem->flags |= TFLAG_ENQUEUE;
   else
      {
      tsk_deqtimer (&elem->link);
      if (tout)
         tsk_enqtimer (&elem->link, tout);
      }
   C_LEAVE;
}

