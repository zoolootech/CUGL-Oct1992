/*
   --- Version 2.2 90-10-12 10:34 ---

   TSKTTSK.C - CTask - Timer task.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   No rights reserved.

   This file is new with 2.1. The timer task functions were moved
   from tsktimer to this module.

   Timer logic has been significantly changed in version 2.0.
*/

#include "tsk.h"
#include "tsklocal.h"

/*
   tsk_timer_action 
      performs the necessary action when a timeout occurred.
      Starting with version 2.0, this part is not critical,
      and may be preempted.

      Version 2.1 makes this routine global for use by the keyboard
      hotkey handler.
*/

void Localfunc tsk_timer_action (tlinkptr elem)
{
   tcbptr task;
   byte st;
   CRITICAL;

   switch (elem->struckind)
      {
      case TKIND_WAKE:
      case TKIND_TASK:  task = (tcbptr) elem->strucp;
                        C_ENTER;
                        st = task->state;

                        if (st == ST_WAITING || st == ST_DELAYED)
                           {
                           task->retptr = 
                              ((elem->elkind & 0xf0) == TELEM_TIMER)
                                 ? TTIMEOUT
                                 : TWATCH;
                           tsk_runable (task);
                           }
                        C_LEAVE;
                        break;

      case TKIND_FLAG:  set_flag ((flagptr) elem->strucp);
                        break;

      case TKIND_COUNTER: inc_counter ((counterptr) elem->strucp);
                        break;

      case TKIND_COUNTDEC: dec_counter ((counterptr) elem->strucp);
                        break;

      case TKIND_PROC:  tsk_callfunc (elem->strucp, elem);
                        break;

      default:          break;
      }
}


/*
   tsk_exec_watch 
      checks the watch condition, and returns 1 if the 
      condition is met.
*/

local int Staticfunc tsk_exec_watch (tlinkptr curr)
{
   word val, cmp;
   int elcmp;

   elcmp = curr->elkind & 0x0f;

   switch (curr->elkind & 0xf0)
      {
      case TELEM_MEM:   val = *(curr->elem.mem.address) & curr->elem.mem.mask;
                        cmp = curr->elem.mem.compare;
                        if (elcmp == TCMP_CHG)
                           curr->elem.mem.compare = val;
                        break;

      /* Microsoft C generates "Internal Compiler Error" on compiling
         the following statement */
#if (TSK_TURBO)
      case TELEM_PORT:  val = (curr->elem.port.in_word)
                              ? tsk_inpw (curr->elem.port.port)
                              : (word)tsk_inp (curr->elem.port.port);
#else
      case TELEM_PORT:  if (curr->elem.port.in_word)
                           val = tsk_inpw (curr->elem.port.port);
                        else
                           val = (word)tsk_inp (curr->elem.port.port);
#endif
                        val &= curr->elem.port.mask;
                        cmp = curr->elem.port.compare;
                        if (elcmp == TCMP_CHG)
                           curr->elem.port.compare = val;
                        break;

      default:          return 0;
      }

   switch (elcmp)
      {
      case TCMP_EQ:  return val == cmp;
      case TCMP_CHG:
      case TCMP_NE:  return val != cmp;
      case TCMP_LE:  return val <= cmp;
      case TCMP_GE:  return val >= cmp;
      case TCMP_LES: return (int)val <= (int)cmp;
      case TCMP_GES: return (int)val >= (int)cmp;
      default:       return 0;
      }
}


/*
   The timer task handles all timeouts.

   Starting with version 2.0, two queues are maintained, one for
   the timeouts, and one for watch elements.

   The timeout queue now is ordered, with the tick count in the queue
   element head holding the difference in ticks to the previous element.
   Thus, only the first element of the timeout queue has to be counted
   down, which will considerably speed up processing here if there are
   multiple elements in the queue.

   The watch queue is unordered.

   The loop to check the queue elements is fully protected.
   This allows other tasks access to the timer queue. The
   concept used in pre-2.0 versions was pretty complicated to
   handle, and still had to run with interupts disabled for
   most parts.

   The new logic unchains expired timeout elements immediately, within
   the protected loop, but delays processing them until after the loop 
   is finished. Modification of those elements should not normally occur,
   and will be rejected. The processing of the timeout/watch action can 
   thus be handled with interrupts enabled.

   Version 2.1 corrects a bug that prevented wakeup elements from being
   correctly processed if they had the repeat attribute. It introduces
   a single link pointer in the timer control block for chaining the
   elements to be acted upon, which is used exclusively by the timer
   task. The 2.0 algorithm used the normal timer queue for chaining,
   which convoluted the code, and, in the end, led to the mentioned bug.

   Version 2.2 cleans up processing by eliminating the "state" field,
   all necessary info is now contained in the flags. This better supports
   enabling and disabling elements while the timer task is processing them.
*/

void Taskfunc tsk_timer (void)
{
   queptr curr;
   queptr help;
   tlinkptr tlast;
   tlinkptr tcurr;
   byte flg;
   CRITICAL;

   while (1)
      {
      wait_counter_set (&tsk_timer_counter, 0L);

      tlast = LNULL;

      C_ENTER;

      /* First, check the timeout queue. Since version 2.0, only
         the first element has to be counted down.
      */

      if (!((curr = GLOBDATA timer_queue.first)->kind & Q_HEAD))
         if (!--curr->el.ticks)
            {
            /* Remove all counted-down elements from the timer queue,
               marking them as not in queue and busy, and chaining them
               through the special "next" pointer.
               We can then process the elements outside the critical 
               section.
            */
            do
               {
               help = curr;
               curr = curr->next;
               tsk_dequeue (help);
               ((tlinkptr)help)->flags |= TFLAG_BUSY;
               ((tlinkptr)help)->next = tlast;
               tlast = (tlinkptr)help;
               }
            while (!(curr->kind & Q_HEAD) && !curr->el.ticks);
            }

      C_LEAVE;
      /* shortly allow interrupts */
      C_ENTER;

      /* Now, check the watch queue. */

      for (curr = GLOBDATA watch_queue.first; !(curr->kind & Q_HEAD); )
         {
         help = curr;
         curr = curr->next;

         if (tsk_exec_watch ((tlinkptr)help))
            {
            tsk_dequeue (help);
            ((tlinkptr)help)->flags |= TFLAG_BUSY;
            ((tlinkptr)help)->next = tlast;
            tlast = (tlinkptr)help;
            }
         }

      /* Ready checking the queues, we can now re-enable interrupts
         for execution of the timeout/watch action. Interrupts are
         disabled only for a short period to check the state and
         re-enqueue repeat elements.  */

      C_LEAVE;

      while (tlast != LNULL)
         {
         tcurr = tlast;
         tlast = tlast->next;
         tsk_timer_action (tcurr);

         C_ENTER;
         flg = tcurr->flags;
         if (flg & TFLAG_REMOVE)
            {
            if (!(flg & TFLAG_TEMP))
               flg &= ~TFLAG_REMOVE;
            }
         else if (!(flg & TFLAG_UNQUEUE))
            {
            if (flg & (TFLAG_REPEAT | TFLAG_ENQUEUE))
               {
               if (tcurr->link.kind == TYP_TIMER)
                  tsk_enqtimer ((queptr)tcurr, tcurr->elem.time.reload);
               else
                  tsk_putqueue (&GLOBDATA watch_queue, (queptr)tcurr);
               }
            }
         tcurr->flags &= ~(TFLAG_BUSY | TFLAG_UNQUEUE | TFLAG_ENQUEUE | TFLAG_REMOVE);
         C_LEAVE;

#if (TSK_DYNAMIC)
         if (flg & TFLAG_REMOVE)
            {
#if (GROUPS)
            tsk_dequeue ((queptr)&tcurr->chain);
#endif
            tsk_pfree (tcurr);
            }
#endif
         }
      }
}


/*
   int8 is the timer interrupt chain task.
*/

#if (IBM && !INT8_EARLY)

void Taskfunc tsk_int8 (void)
{
   while (1)
      {
      wait_counter_set (&tsk_int8_counter, 0L);
      tsk_chain_timer ();
      }
}

#endif

