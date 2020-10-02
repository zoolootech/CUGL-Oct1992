/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKHOT.C - CTask - Keyboard Hotkey operations.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   This file is new with 2.1, which added keyboard hotkey support.
   This module reduces to nothing if the HOTKEYS flag is clear.

   Version 2.2 adds a create function that does not enqueue the created
   element. Separate enable and disable routines have been added.
*/

#include "tsk.h"
#include "tsklocal.h"

#if (HOTKEYS)

#include <stdarg.h>

/*
   create_hotkey_elem
      Creates a hotkey element, but does not activate it.
*/

tlinkptr CGlobalfunc create_hotkey_elem (tlinkptr elem, byte scancode,
                                         byte mask1, byte flag1,
                                         byte mask2, byte flag2,
                                         byte mask3, byte flag3,
                                         farptr strucp, byte kind, 
                                         int rept, ...)
{
   va_list val;

   va_start (val, rept);
   elem = tsk_setup_telem (elem, TYP_HOTKEY, strucp, kind, va_arg (val, dword));
   va_end (val);
   if (elem == LNULL)
      return LNULL;

   if (rept)
      elem->flags |= TFLAG_REPEAT;
   elem->elkind = (byte)TELEM_HOTKEY;
   elem->elem.key.scancode = scancode;
   elem->elem.key.flag1.mask = mask1;
   elem->elem.key.flag1.compare = flag1;
   elem->elem.key.flag2.mask = mask2;
   elem->elem.key.flag2.compare = flag2;
   elem->elem.key.flag3.mask = mask3;
   elem->elem.key.flag3.compare = flag3;

   return elem;
}


/*
   create_hotkey_entry
      Creates a hotkey element and activates it.
      For compatibility with previous versions.
*/

tlinkptr CGlobalfunc create_hotkey_entry (tlinkptr elem, byte scancode,
                                          byte mask1, byte flag1,
                                          byte mask2, byte flag2,
                                          byte mask3, byte flag3,
                                          farptr strucp, byte kind, 
                                          int rept, ...)
{
   va_list val;

   va_start (val, rept);
   elem = create_hotkey_elem (elem, scancode, mask1, flag1, mask2, flag2, 
                              mask3, flag3, strucp, kind, rept, 
                              va_arg (val, dword));
   va_end (val);
   if (elem != LNULL)
      enable_hotkey (elem);

   return elem;
}


/*
   wait_hotkey
      Delay current task until hotkey is hit.
*/

int Globalfunc wait_hotkey (byte scancode,
                            byte mask1, byte flag1,
                            byte mask2, byte flag2,
                            byte mask3, byte flag3)
{
   tlinkptr elem;
   tcbptr task = GLOBDATA current_task;

   elem = &task->timerq;
   elem->link.kind = TYP_HOTKEY;
   elem->struckind = TKIND_WAKE;
   elem->elkind = (byte)TELEM_HOTKEY;
   elem->flags = 0;
   elem->elem.key.scancode = scancode;
   elem->elem.key.flag1.mask = mask1;
   elem->elem.key.flag1.compare = flag1;
   elem->elem.key.flag2.mask = mask2;
   elem->elem.key.flag2.compare = flag2;
   elem->elem.key.flag3.mask = mask3;
   elem->elem.key.flag3.compare = flag3;

   tsk_cli ();
   task->state = ST_DELAYED;
   task->qhead = LNULL;

   if (scancode)
      tsk_putqueue (&GLOBDATA hotkey_scan, &elem->link);
   else
      tsk_putqueue (&GLOBDATA hotkey_noscan, &elem->link);

   schedule ();
   return (int)((dword)task->retptr);
}


/*
   enable_hotkey
      Enables a hotkey entry by enqueueing into the appropriate hotkey queue.
*/

void Globalfunc enable_hotkey (tlinkptr elem)
{
   CRITICAL;

   CHECK_TELPTR (elem, TYP_HOTKEY, "Enable Hotkey");

   if (elem->link.next)
      return;
   C_ENTER;
   if (elem->elem.key.scancode)
      tsk_putqueue (&GLOBDATA hotkey_scan, &elem->link);
   else
      tsk_putqueue (&GLOBDATA hotkey_noscan, &elem->link);
   C_LEAVE;
}


#endif

