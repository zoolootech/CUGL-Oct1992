/*
   --- Version 2.2 90-10-12 11:08 ---

   TSKWPIP.C - CTask - Word Pipe handling routines.

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
   create_wpipe - initialises wpipe.
*/

wpipeptr Globalfunc create_wpipe (wpipeptr pip, farptr buf, word bufsize
                                  TN(byteptr name))
{
#if (TSK_DYNAMIC)
   if (pip == LNULL)
      {
      if ((pip = tsk_palloc (sizeof (wpipe))) == LNULL)
         return LNULL;
      pip->flags = F_TEMP;
      }
   else
      pip->flags = 0;
   if (buf == LNULL)
      {
      if ((buf = tsk_palloc (bufsize)) == LNULL)
         {
         if (pip->flags & F_TEMP)
            tsk_pfree (pip);
         return LNULL;
         }
      pip->flags |= F_STTEMP;
      }
#endif

   tsk_init_qhead (&pip->wait_read, TYP_WPIPE);
   tsk_init_qhead (&pip->wait_write, TYP_WPIPE);
   tsk_init_qhead (&pip->wait_clear, TYP_WPIPE);
   pip->outptr = pip->inptr = pip->filled = 0;
   pip->bufsize = bufsize >> 1;
   pip->wcontents = (wordptr)buf;

#if (TSK_NAMED)
   tsk_add_name (&pip->name, name, TYP_WPIPE, pip);
#endif

   return pip;
}


/*
   delete_wpipe - kills all processes waiting for reading from or writing
                  to the wpipe.
*/

void Globalfunc delete_wpipe (wpipeptr pip)
{
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_WPIPE, "Delete Wpipe");

   C_ENTER;
   tsk_kill_queue (&(pip->wait_read));
   tsk_kill_queue (&(pip->wait_write));
   tsk_kill_queue (&(pip->wait_clear));
   pip->outptr = pip->inptr = pip->filled = 0;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&pip->name);
#endif

#if (TSK_DYNAMIC)
   if (pip->flags & F_STTEMP)
      tsk_pfree (pip->wcontents);
   if (pip->flags & F_TEMP)
      tsk_pfree (pip);
#endif
}


/*
   read_wpipe - Wait until a word is written to the wpipe. If there 
                is a word in the wpipe on entry, it is assigned to 
                the caller, and the task continues to run. If there are
                tasks waiting to write, the first task is made eligible,
                and the word is inserted into the wpipe.
*/

word Globalfunc read_wpipe (wpipeptr pip, dword timeout)
{
   tcbptr curr;
   word res;
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_WPIPE, "Read Wpipe");

   C_ENTER;

   if (pip->filled)
      {
      res = pip->wcontents [pip->outptr++];
      if (pip->outptr >= pip->bufsize)
         pip->outptr = 0;
      pip->filled--;

      if (!((curr = (tcbptr)pip->wait_write.first)->cqueue.kind & Q_HEAD))
         {
         pip->wcontents [pip->inptr++] = curr->retsize;
         if (pip->inptr >= pip->bufsize)
            pip->inptr = 0;
         pip->filled++;
         tsk_runable (curr);
         curr->retptr = LNULL;
         }
      else if (!pip->filled)
         tsk_runable_all (&pip->wait_clear);

      C_LEAVE;
      return res;
      }

   tsk_wait (&pip->wait_read, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}


/*
   c_read_wpipe - If there is a word in the wpipe on entry,
                  read_wpipe is called, otherwise an error status is returned.
*/

word Globalfunc c_read_wpipe (wpipeptr pip)
{
   CRITICAL, res;

   CHECK_EVTPTR (pip, TYP_WPIPE, "Cond Read Wpipe");

   C_ENTER;
   res = (pip->filled) ? read_wpipe (pip, 0L) : (word)-1;
   C_LEAVE;
   return res;
}



/*
   write_wpipe - Wait until space for the word to be written to the 
                 wpipe is available. If there is enough space in the wpipe 
                 on entry, the word is inserted into the wpipe, and
                 the task continues to run. If there are tasks waiting 
                 to read, the first task is made eligible, and the word
                 is passed to the waiting task.
*/

int Globalfunc write_wpipe (wpipeptr pip, word ch, dword timeout)
{
   tcbptr curr;
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_WPIPE, "Write Wpipe");

   C_ENTER;

   if (pip->filled < pip->bufsize)
      {
      if (!((curr = (tcbptr)pip->wait_read.first)->cqueue.kind & Q_HEAD))
         {
         tsk_runable (curr);
         curr->retptr = (farptr)ch;
         C_LEAVE;
         return 0;
         }

      pip->wcontents [pip->inptr++] = ch;
      if (pip->inptr >= pip->bufsize)
         pip->inptr = 0;
      pip->filled++;
      C_LEAVE;
      return 0;
      }

   GLOBDATA current_task->retsize = ch;
   tsk_wait (&pip->wait_write, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}


/*
   c_write_wpipe - If there is space for the word in the wpipe on entry,
                   write_wpipe is called, otherwise an error status is returned.
*/

int Globalfunc c_write_wpipe (wpipeptr pip, word ch)
{
   int res;
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_WPIPE, "Cond Write Wpipe");

   C_ENTER;
   res = (pip->filled < pip->bufsize) ? write_wpipe (pip, ch, 0L) : -1;
   C_LEAVE;
   return res;
}


/*
   wait_wpipe_empty - Wait until the pipe is empty. If the pipe is
                      empty on entry, the task continues to run.
*/

int Globalfunc wait_wpipe_empty (wpipeptr pip, dword timeout)
{
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_WPIPE, "Wait Wpipe Empty");

   C_ENTER;
   if (!pip->filled)
      {
      C_LEAVE;
      return 0;
      }

   GLOBDATA current_task->retptr = LNULL;
   tsk_wait (&pip->wait_clear, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}


/*
   check_wpipe - returns -1 if there are no words in the wpipe, else
                 the first available word.
*/

word Globalfunc check_wpipe (wpipeptr pip)
{
   CHECK_EVTPTR (pip, TYP_WPIPE, "Check Wpipe");

   return (pip->filled) ? pip->wcontents [pip->outptr] : (word)-1;
}


/*
   wpipe_free - returns the number of free words in the pipe.
*/

word Globalfunc wpipe_free (wpipeptr pip)
{
   CHECK_EVTPTR (pip, TYP_WPIPE, "Wpipe Free");
   return pip->bufsize - pip->filled;
}

/*
   flush_wpipe - Empty the pipe buffer, activate tasks waiting for 
                 pipe clear.
*/

void Globalfunc flush_wpipe (wpipeptr pip)
{
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_WPIPE, "Flush Wpipe");
   C_ENTER;
   pip->inptr = pip->outptr = pip->filled = 0;

   tsk_runable_all (&pip->wait_clear);
   C_LEAVE;
}


