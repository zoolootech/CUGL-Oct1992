/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKPORW.C - CTask - Port watch operations.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   This file is new with 2.1. The port watch related functions were moved
   from tsktimer to this module.

   Version 2.2 adds a create routine that does not enqueue the created
   element. Separate enable and disable routines have been added in
   tsktsub.c.
*/

#include "tsk.h"
#include "tsklocal.h"

#include <stdarg.h>

/*
   create_port_watch_elem
      Creates a port watch element.
*/

tlinkptr CGlobalfunc create_port_watch_elem (tlinkptr elem, word port, 
                                             byte in_word, word mask, 
                                             word compare, byte cmpkind,
                                             farptr strucp, byte kind, 
                                             int rept, ...)
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
   elem->elkind = (byte)(TELEM_PORT | cmpkind);
   elem->elem.port.port = port;
   elem->elem.port.in_word = in_word;
   elem->elem.port.mask = mask;
   elem->elem.port.compare = compare;

   return elem;
}

/*
   create_port_watch
      Creates a port watch element and activates it.
      For compatibility to previous versions only.
*/

tlinkptr CGlobalfunc create_port_watch (tlinkptr elem, word port, byte in_word,
                                        word mask, word compare, byte cmpkind,
                                        farptr strucp, byte kind, int rept, ...)
{
   va_list val;

   va_start (val, rept);
   elem = create_port_watch_elem (elem, port, in_word, mask, compare, cmpkind,
                                  strucp, kind, rept, va_arg (val, dword));
   va_end (val);
   if (elem != LNULL)
      enable_watch (elem);

   return elem;
}


/*
   wait_port
      Delay current task until specified port watch condition is met.
*/

int Globalfunc wait_port (word port, byte in_word,
                          word mask, word compare, byte cmpkind)
{
   tlinkptr elem;
   tcbptr task = GLOBDATA current_task;

   if (cmpkind < TCMP_EQ || cmpkind > TCMP_CHG)
      return 1;

   elem = &task->timerq;
   elem->link.kind = TYP_WATCH;
   elem->struckind = TKIND_WAKE;
   elem->elkind = (byte)(TELEM_PORT | cmpkind);
   elem->flags = 0;
   elem->elem.port.port = port;
   elem->elem.port.in_word = in_word;
   elem->elem.port.mask = mask;
   elem->elem.port.compare = compare;

   tsk_cli ();
   task->state = ST_DELAYED;
   task->qhead = LNULL;

   tsk_putqueue (&GLOBDATA watch_queue, &elem->link);

   schedule ();
   return (int)((dword)task->retptr);
}

