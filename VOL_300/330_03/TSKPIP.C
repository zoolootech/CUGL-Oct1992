/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKPIP.C - CTask - Pipe handling routines.

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
   create_pipe - initialises pipe.
*/

pipeptr Globalfunc create_pipe (pipeptr pip, farptr buf, word bufsize
                                TN(byteptr name))
{
#if (TSK_DYNAMIC)
   if (pip == LNULL)
      {
      if ((pip = tsk_palloc (sizeof (pipe))) == LNULL)
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

   tsk_init_qhead (&pip->wait_read, TYP_PIPE);
   tsk_init_qhead (&pip->wait_write, TYP_PIPE);
   tsk_init_qhead (&pip->wait_clear, TYP_PIPE);
   pip->outptr = pip->inptr = pip->filled = 0;
   pip->bufsize = bufsize;
   pip->contents = (byteptr)buf;

#if (TSK_NAMED)
   tsk_add_name (&pip->name, name, TYP_PIPE, pip);
#endif

   return pip;
}


/*
   delete_pipe - kills all processes waiting for reading from or writing
                 to the pipe.
*/

void Globalfunc delete_pipe (pipeptr pip)
{
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_PIPE, "Delete Pipe");

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
      tsk_pfree (pip->contents);
   if (pip->flags & F_TEMP)
      tsk_pfree (pip);
#endif
}


/*
   read_pipe - Wait until a character is written to the pipe. If there 
               is a character in the pipe on entry, it is assigned to 
               the caller, and the task continues to run. If there are
               tasks waiting to write, the first task is made eligible,
               and the character is inserted into the pipe.
*/

int Globalfunc read_pipe (pipeptr pip, dword timeout)
{
   tcbptr curr;
   int res;
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_PIPE, "Read Pipe");

   C_ENTER;

   if (pip->filled)
      {
      res = pip->contents [pip->outptr++];
      if (pip->outptr >= pip->bufsize)
         pip->outptr = 0;
      pip->filled--;

      if (!((curr = (tcbptr)pip->wait_write.first)->cqueue.kind & Q_HEAD))
         {
         pip->contents [pip->inptr++] = (byte)curr->retsize;
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
   c_read_pipe - If there is a character in the pipe on entry,
                 read_pipe is called, otherwise en error status is returned.
*/

int Globalfunc c_read_pipe (pipeptr pip)
{
   CRITICAL, res;

   CHECK_EVTPTR (pip, TYP_PIPE, "Cond Read Pipe");

   C_ENTER;
   res = (pip->filled) ? read_pipe (pip, 0L) : -1;
   C_LEAVE;
   return res;
}



/*
   write_pipe - Wait until space for the character to be written to the 
                pipe is available. If there is enough space in the pipe 
                on entry, the character is inserted into the pipe, and
                the task continues to run. If there are tasks waiting 
                to read, the first task is made eligible, and the character
                is passed to the waiting task.
*/

int Globalfunc write_pipe (pipeptr pip, byte ch, dword timeout)
{
   tcbptr curr;
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_PIPE, "Write Pipe");

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

      pip->contents [pip->inptr++] = ch;
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
   c_write_pipe - If there is space for the character in the pipe on entry,
                  write_pipe is called, otherwise en error status is returned.
*/

int Globalfunc c_write_pipe (pipeptr pip, byte ch)
{
   int res;
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_PIPE, "Cond Write Pipe");

   C_ENTER;
   res = (pip->filled < pip->bufsize) ? write_pipe (pip, ch, 0L) : -1;
   C_LEAVE;
   return res;
}


/*
   wait_pipe_empty - Wait until the pipe is empty. If the pipe is
                     empty on entry, the task continues to run.
*/

int Globalfunc wait_pipe_empty (pipeptr pip, dword timeout)
{
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_PIPE, "Wait Pipe Empty");

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
   check_pipe - returns -1 if there are no characters in the pipe, else
                the first available character.
*/

int Globalfunc check_pipe (pipeptr pip)
{
   CHECK_EVTPTR (pip, TYP_PIPE, "Check Pipe");

   return (pip->filled) ? (int)pip->contents [pip->outptr] : -1;
}


/*
   pipe_free - returns the number of free characters in the pipe.
*/

word Globalfunc pipe_free (pipeptr pip)
{
   CHECK_EVTPTR (pip, TYP_PIPE, "Pipe Free");
   return pip->bufsize - pip->filled;
}


/*
   flush_pipe - Empty the pipe buffer, activate tasks waiting for 
                pipe clear.
*/

void Globalfunc flush_pipe (pipeptr pip)
{
   CRITICAL;

   CHECK_EVTPTR (pip, TYP_PIPE, "Flush Pipe");
   C_ENTER;
   pip->inptr = pip->outptr = pip->filled = 0;

   tsk_runable_all (&pip->wait_clear);
   C_LEAVE;
}

