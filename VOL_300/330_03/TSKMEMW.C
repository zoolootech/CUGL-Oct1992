/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKMEMW.C - CTask - Memory watch operations.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   This file is new with 2.1. The memory watch related functions were moved
   from tsktimer to this module.

   Version 2.2 adds a create routine that does not enqueue the created
   element. Separate enable and disable routines have been added in
   tsktsub.c.
*/

#include "tsk.h"
#include "tsklocal.h"

#include <stdarg.h>

/*
   create_memory_watch_elem
      Creates a memory watch element. 
*/

tlinkptr CGlobalfunc create_memory_watch_elem (tlinkptr elem, farptr address, 
                                               word mask, word compare, 
                                               byte cmpkind, farptr strucp, 
                                               byte kind, int rept, ...)
{
   va_list val;

   if (cmpkind < TCMP_EQ || cmpkind > TCMP_CHG)
      return LNULL;

   va_start (val, rept);
   elem = tsk_setup_telem (elem, TYP_WATCH, strucp, kind, va_arg (val, dword));
   va_end (val);
   if (elem == LNULL)
      return LNULL;

   if (rept)
      elem->flags |= TFLAG_REPEAT;
   elem->elkind = (byte)(TELEM_MEM | cmpkind);
   elem->elem.mem.address = address;
   elem->elem.mem.mask = mask;
   elem->elem.mem.compare = compare;

   return elem;
}


/*
   create_memory_watch
      Creates a memory watch element and activates it.
      Fro compatibility to previous versions only.
*/

tlinkptr CGlobalfunc create_memory_watch (tlinkptr elem, farptr address, 
                                          word mask, word compare, byte cmpkind,
                                          farptr strucp, byte kind, int rept, ...)
{
   va_list val;

   va_start (val, rept);
   elem = create_memory_watch_elem (elem, address, mask, compare, cmpkind, 
                                    strucp, kind, rept, va_arg (val, dword));
   va_end (val);
   if (elem != LNULL)
      enable_watch (elem);

   return elem;
}


/*
   wait_memory
      Delay current task until specified memory watch condition is met.
*/

int Globalfunc wait_memory (farptr address, 
                            word mask, word compare, byte cmpkind)
{
   tlinkptr elem;
   tcbptr task = GLOBDATA current_task;

   if (cmpkind < TCMP_EQ || cmpkind > TCMP_CHG)
      return 1;

   elem = &task->timerq;
   elem->link.kind = TYP_WATCH;
   elem->struckind = TKIND_WAKE;
   elem->elkind = (byte)(TELEM_MEM | cmpkind);
   elem->flags = 0;
   elem->elem.mem.address = address;
   elem->elem.mem.mask = mask;
   elem->elem.mem.compare = compare;

   tsk_cli ();
   task->state = ST_DELAYED;
   task->qhead = LNULL;

   tsk_putqueue (&GLOBDATA watch_queue, &elem->link);

   schedule ();
   return (int)((dword)task->retptr);
}



