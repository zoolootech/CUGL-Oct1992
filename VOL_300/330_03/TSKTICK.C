/*
   --- Version 2.2 90-10-12 10:34 ---

   TSKTICK.C - CTask - Ticker routines.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   This file is new with 2.1. The ticker related functions were moved
   from tsktimer to this module.

   Timer logic has been significantly changed in version 2.0.
*/

#include "tsk.h"
#include "tsklocal.h"

/*
   create_ticker
      link element into ticker chain.
*/

tick_ptr Globalfunc create_ticker (tick_ptr elem, dword val)
{
   CRITICAL;

#if (TSK_DYNAMIC)
   if (elem == LNULL)
      {
      if ((elem = tsk_palloc (sizeof (ticker))) == LNULL)
         return LNULL;
      elem->flags = F_TEMP;
      }
   else
      elem->flags = 0;
#else
   if (elem == LNULL)
      return LNULL;
#endif

   elem->ticks = val;
   C_ENTER;
   elem->next = GLOBDATA ticker_chain;
   GLOBDATA ticker_chain = elem;
   tsk_putqueue (&GLOBDATA current_task->group->ticker_list, (queptr)&elem->chain);
   C_LEAVE;

   return elem;
}


/*
   delete_ticker
      unlink element from ticker chain.
*/

void Globalfunc delete_ticker (tick_ptr elem)
{
   tick_ptr curr;
   CRITICAL;

   curr = (tick_ptr)&GLOBDATA ticker_chain;

   C_ENTER;
   while (curr->next != LNULL && curr->next != elem)
      curr = curr->next;

   curr->next = elem->next;
#if (GROUPS)
   tsk_dequeue ((queptr)&elem->chain);
#endif
   C_LEAVE;
#if (TSK_DYNAMIC)
   if (elem->flags & F_TEMP)
      tsk_pfree (elem);
#endif
}

/*
   set_ticker
      set new ticker value.
*/

void Globalfunc set_ticker (tick_ptr elem, dword val)
{
   CRITICAL;

   C_ENTER;
   elem->ticks = val;
   C_LEAVE;
}


/*
   get_ticker
      get current ticker value.
*/

dword Globalfunc get_ticker (tick_ptr elem)
{
   dword tck;
   CRITICAL;

   C_ENTER;
   tck = elem->ticks;
   C_LEAVE;
   return tck;
}





