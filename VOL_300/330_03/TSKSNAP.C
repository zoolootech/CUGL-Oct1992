/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKSNAP.C - CTask State Snaphot

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   This file is new with Version 1.1
   It has been changed to use the assembler printf routines in 
   version 2.1.
*/

#include "tsk.h"
#include "tsklocal.h"
#include "tsksup.h"
#include "tskprf.h"
#include "tskdeb.h"

#include <stdarg.h>

#define DBPSP  0  /* If defined nonzero, the PSP info is displayed in place
                     of stack-pointer and stack-bottom in dump_tasks. */

#define NOFILEIO 1  /* If defined nonzero, the routines using the standard
                       I/O routines are not included in the compilation. */

#if (DOS)

local char *sn_state [] = { "*???*", "Stop", "Delay", "Wait", "Elig", "Run" };
local char *sn_kind [] = { "*Undef*", "Task", "Flag", "Resource", "Counter",
                           "Mailbox", "Pipe", "Wpipe", "Buffer" };

local int maxrow, row;

#if (CHECKING)
local word Staticfunc stack_free (tcbptr task)
{
   word i;
   byteptr ptr;

   ptr = task->stkbot;
   for (i = 0; *ptr++ == (byte)i; i++)
      ;
   return i;
}
#endif

local int CStaticfunc xprintf (int handle, byteptr format, ...)
{
   va_list argptr;

   if (maxrow > 0 && row >= maxrow)
      return -1;

   va_start (argptr, format);

   if (handle >= 0)
      tsk_vfprintf (handle, format, argptr);
   else if (handle == -1)
      tsk_vprintf (format, argptr);
   else
      tsk_vrprintf (format, argptr);

   va_end (argptr);
   return 0;
}


local int Staticfunc sn_wlist (int f, byteptr str, queheadptr list, int col)
{
   queptr curr;

   curr = list->first;
   if (curr->kind & Q_HEAD)
      return col;
   if (col)
      {
      row++;
      xprintf (f, "\n%31c", ' ');
      }
   col = 52;
   xprintf (f, "%6Fs(%Fp):  ", (farptr)str, list);
   while (!(curr->kind & Q_HEAD))
      {
      if (col >= 60)
         {
         row++;
         xprintf (f, "\n%51c", ' ');
         col = 52;
         }
      xprintf (f, "%-8Fs(%Fp) ", ((tcbptr)curr)->name.name, curr);
      col += 20;
      curr = curr->next;
      }
   return col;
}


local void Staticfunc dgroup (int f, gcbptr group)
{
   xprintf (f, ">> Group %-8Fs (%Fp), C:%-8Fs (%Fp), B:%Fp, L:%Fp\n",
            group->namelist.name, group, 
            group->creator->name.name, group->creator,
            group->branch, group->level);
   row++;
}


local void Staticfunc event_group (int f, gcbptr base)
{
   queptr curr;
   int col;
   flagptr fl;
   counterptr ct;
   resourceptr rs;
   mailboxptr mb;
   pipeptr pp;
   wpipeptr wp;
   bufferptr bp;
   gcbptr group;

   for (group = base; group != LNULL; group = group->level)
      {
      dgroup (f, group);
      for (curr = group->namelist.list.first; !(curr->kind & Q_HEAD); curr = curr->next)
         if (curr->kind != TYP_TCB)
            {
            if (xprintf (f, "%-8Fs   %-8Fs  ", ((nameptr)curr)->name,
                         (farptr)sn_kind [curr->kind]))
               return;
            switch (curr->kind)
               {
               case TYP_FLAG: fl = (flagptr) ((nameptr)curr)->strucp;
                              xprintf (f, "%8Fs  ", (farptr)((fl->state) ? "Set" : "Clear"));
                              col = sn_wlist (f, "Set", &fl->wait_set, 0);
                              sn_wlist (f, "Clear", &fl->wait_clear, col);
                              break;

               case TYP_COUNTER: ct = (counterptr) ((nameptr)curr)->strucp;
                              xprintf (f, "%8ld  ", ct->state);
                              col = sn_wlist (f, "Set", &ct->wait_set, 0);
                              sn_wlist (f, "Clear", &ct->wait_clear, col);
                              break;

               case TYP_RESOURCE: rs = (resourceptr) ((nameptr)curr)->strucp;
                              if (rs->count)
                                 {
                                 xprintf (f, "%8u  ", rs->count);
                                 xprintf (f, "Owner (%Fp):  ", (farptr)&rs->owner);
                                 xprintf (f, "%-8Fs(%Fp) ", rs->owner->name.name, 
                                                         rs->owner);
                                 col = 70;
                                 }
                              else
                                 {
                                 xprintf (f, "    Free  ");
                                 col = 0;
                                 }
                              sn_wlist (f, "Wait", &rs->waiting, col);
                              break;

               case TYP_MAILBOX: mb = (mailboxptr) ((nameptr)curr)->strucp;
                              xprintf (f, "%8Fs  ", (farptr)((mb->mail_first == LNULL)
                                       ? "Empty" : "Mail"));
                              sn_wlist (f, "Mail", &mb->waiting, 0);
                              break;

               case TYP_PIPE: pp = (pipeptr) ((nameptr)curr)->strucp;
                              xprintf (f, "%8u  ", pp->filled);
                              col = sn_wlist (f, "Read", &pp->wait_read, 0);
                              col = sn_wlist (f, "Write", &pp->wait_write, col);
                              sn_wlist (f, "Clear", &pp->wait_clear, col);
                              break;

               case TYP_WPIPE: wp = (wpipeptr) ((nameptr)curr)->strucp;
                              xprintf (f, "%8u  ", wp->filled);
                              col = sn_wlist (f, "Read", &wp->wait_read, 0);
                              col = sn_wlist (f, "Write", &wp->wait_write, col);
                              sn_wlist (f, "Clear", &wp->wait_clear, col);
                              break;

               case TYP_BUFFER: bp = (bufferptr) ((nameptr)curr)->strucp;
                              xprintf (f, "%8u  ", bp->msgcnt);
                              col = sn_wlist (f, "Read", &bp->buf_read.waiting, 0);
                              sn_wlist (f, "Write", &bp->buf_write.waiting, col);
                              break;

               default:       xprintf (f, "*Invalid Type %02x", curr->kind);
                              break;
               }
            xprintf (f, "\n");
            row++;
            }
      if (group->branch != LNULL)
         event_group (f, group->branch);
      }
}


local void Staticfunc dump_events (int f)
{
   if (xprintf (f, "List of Events:\n"))
      return;
   /*           1234567890123456789012345678901234567890123456789012345
                12345678   12345678  12345678  123456(123456789):        */
   row++;
   if (xprintf (f, "Name       Type         State     Waitfor(Queue)   Tasks\n"))
      return;
   row++;

   event_group (f, &GLOBDATA group);
   xprintf (f, "\n");
   row++;
}


local void Staticfunc task_group (int f, gcbptr base, word oldpri)
{
   queptr curr;
   tcbptr task;
   gcbptr group;

   for (group = base; group != LNULL; group = group->level)
      {
      dgroup (f, group);
      for (curr = group->namelist.list.first; !(curr->kind & Q_HEAD);
           curr = curr->next)
         if (curr->kind == TYP_TCB)
            {
            task = (tcbptr) ((nameptr)curr)->strucp;

            if (xprintf (f, "%-8Fs %-5Fs %Fp ",
                         ((nameptr)curr)->name, 
                         (farptr) sn_state [(task->state <= 5) 
                                            ? task->state : 0],
                         task->qhead))
               return;

            if (task->timerq.link.next == LNULL)
               xprintf (f, "%8c", '-');
            else switch (task->timerq.link.kind)
               {
               case TYP_WATCH:         xprintf (f, " Watch ");
                                       if ((task->timerq.elkind & 0xf0)
                                           == TELEM_MEM)
                                          xprintf (f, "M");
                                       else
                                          xprintf (f, "P");
                                       break;

               case TYP_HOTKEY:        xprintf (f, " Hotkey ");
                                       break;

               case TYP_TIMER:         xprintf (f, "%8ld", 
                                             task->timerq.link.el.ticks);
                                       break;

               default:                xprintf (f, "  ?%5d",
                                                task->timerq.link.kind); 
                                       break;
               }

#if (CHECKING)
#if (!DBPSP)
            xprintf (f, " %Fp %Fp   %6d  %Fp",
                  task,
                  task->stack, 
                  stack_free (task),
                  ((struct task_stack far *)task->stack)->retn);
#else
            xprintf (f, " %Fp %04X %04X   %6d  %Fp",
                  task,
                  task->base_psp,
                  *((word far *)(&task->swap_area [0x10])),
                  stack_free (task),
                  ((struct task_stack far *)task->stack)->retn);
#endif
#else
#if (!DBPSP)
            xprintf (f, " %Fp %Fp %Fp %Fp",
                  task,
                  task->stack, 
                  task->stkbot,
                  ((struct task_stack far *)task->stack)->retn);
#else
            xprintf (f, " %Fp %04X %04X %Fp %Fp",
                  task,
                  task->base_psp,
                  *((word far *)(&task->swap_area [0x10])),
                  task->psp_sssp,
                  ((struct task_stack far *)task->stack)->retn);
#endif
#endif
            if (task != GLOBDATA current_task)
               xprintf (f, " %5u\n", task->cqueue.el.pri.prior);
            else
               xprintf (f, "*%5u\n", oldpri);
            row++;
            }

      if (group->branch != LNULL)
         task_group (f, group->branch, oldpri);
      }
}


local void Staticfunc dump_tasks (int f, word oldpri)
{
   static union REGS regs;

   regs.x.ax = 0x1683;
   int86 (0x2f, &regs, &regs);
   row++;
   xprintf (f, "\nTask List (VM=%04X):\n", regs.x.bx);
   row++;
#if (CHECKING)
#if (!DBPSP)
   xprintf (f, "Name     State Queue      Timeout TCB-addr  Stackptr  FreeStack Instrptr  Prior\n");
#else
   xprintf (f, "Name     State Queue      Timeout TCB-addr  BPSP CPSP FreeStack Instrptr  Prior\n");
#endif
#else
#if (!DBPSP)
   xprintf (f, "Name     State Queue      Timeout TCB-addr  Stackptr  Stackbot  Instrptr  Prior\n");
#else
   xprintf (f, "Name     State Queue      Timeout TCB-addr  BPSP CPSP PSP-SSSP  Instrptr  Prior\n");
#endif
#endif
   row++;

   task_group (f, &GLOBDATA group, oldpri);
   xprintf (f, "\n");
   row++;
}


local void Staticfunc ssnap (int f)
{
   word oldpri;

   oldpri = get_priority (GLOBDATA current_task);
   set_priority (GLOBDATA current_task, 0xffff);

   row = 0;
   dump_tasks (f, oldpri);
   dump_events (f);

   set_priority (GLOBDATA current_task, oldpri);
}


#if (!NOSTDIO)

void Globalfunc snapshot (FILE far *f)
{
   fflush (f);
   maxrow = 0;
   ssnap (fileno (f));
}

#endif

void Globalfunc csnapshot (void)
{
   maxrow = 0;
   ssnap (-1);
}


void Globalfunc screensnap (int rows)
{
#if (DEBUG)
   maxrow = rows - 3;   /* Leave 2 lines at top alone */
   tsk_setpos (2, 0);
#else
   maxrow = rows - 1;
   tsk_setpos (0, 0);
#endif
   tsk_set_clreol (1);
   ssnap (-2);
}

#endif

