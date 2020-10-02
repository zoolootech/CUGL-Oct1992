/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKMSG.C - CTask - Message handling routines.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany
*/

#include "tsk.h"
#include "tsklocal.h"


/*
   create_mailbox - initialises mailbox.
*/

mailboxptr Globalfunc create_mailbox (mailboxptr box TN(byteptr name))
{
#if (TSK_DYNAMIC)
   if (box == LNULL)
      {
      if ((box = tsk_palloc (sizeof (mailbox))) == LNULL)
         return LNULL;
      box->flags = F_TEMP;
      }
   else
      box->flags = 0;
#endif

   tsk_init_qhead (&box->waiting, TYP_MAILBOX);
   box->mail_first = box->mail_last = LNULL;

#if (TSK_NAMED)
   tsk_add_name (&box->name, name, TYP_MAILBOX, box);
#endif

   return box;
}


/*
   delete_mailbox - kills all processes waiting for mail
*/

void Globalfunc delete_mailbox (mailboxptr box)
{
   CRITICAL;

   CHECK_EVTPTR (box, TYP_MAILBOX, "Delete Mailbox");

   C_ENTER;
   tsk_kill_queue (&(box->waiting));
   box->mail_first = box->mail_last = LNULL;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&box->name);
#endif

#if (TSK_DYNAMIC)
   if (box->flags & F_TEMP)
      tsk_pfree (box);
#endif
}


/*
   wait_mail - Wait until mail arrives. If there is mail in the box on
               entry, the first mail block is assigned to the caller,
               and the task continues to run.
*/

farptr Globalfunc wait_mail (mailboxptr box, dword timeout)
{
   msgptr msg;
   CRITICAL;

   CHECK_EVTPTR (box, TYP_MAILBOX, "Wait Mail");

   C_ENTER;
   if ((msg = box->mail_first) != LNULL)
      {
      if ((box->mail_first = msg->next) == LNULL)
         box->mail_last = LNULL;
      C_LEAVE;
      return msg;
      }

   tsk_wait (&box->waiting, timeout);
   return GLOBDATA current_task->retptr;
}

/*
   c_wait_mail - If there is mail in the box on entry, the first mail 
                 block is assigned to the caller, else an error is returned.
*/

farptr Globalfunc c_wait_mail (mailboxptr box)
{
   msgptr msg;
   CRITICAL;

   CHECK_EVTPTR (box, TYP_MAILBOX, "Cond Wait Mail");

   C_ENTER;
   if ((msg = box->mail_first) != LNULL)
      if ((box->mail_first = msg->next) == LNULL)
         box->mail_last = LNULL;
   C_LEAVE;
   return msg;
}


/*
   send_mail - Send a mail block to a mailbox. If there are tasks waiting
               for mail, the first waiting task is assigned the block and
               is made eligible.
*/

void Globalfunc send_mail (mailboxptr box, farptr msg)
{
   tcbptr curr;
   CRITICAL;

   CHECK_EVTPTR (box, TYP_MAILBOX, "Send Mail");

   C_ENTER;
   if ((curr = (tcbptr)box->waiting.first)->cqueue.kind & Q_HEAD)
      {
      if (box->mail_first == LNULL)
         box->mail_first = (msgptr)msg;
      else
         box->mail_last->next = (msgptr)msg;
      ((msgptr)msg)->next = LNULL;
      box->mail_last = (msgptr)msg;
      C_LEAVE;
      return;
      }
   tsk_runable (curr);
   curr->retptr = msg;
   C_LEAVE;
}


/*
   check_mailbox - returns TRUE if there is mail in the box.
*/

int Globalfunc check_mailbox (mailboxptr box)
{
   CHECK_EVTPTR (box, TYP_MAILBOX, "Check Mailbox");
   return box->mail_first != LNULL;
}


