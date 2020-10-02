/*
   --- Version 2.0 90-10-09 22:02 ---

   Sample module for channeling console output through
   a single task to avoid problems with non-reentrant output
   routines.
*/

#include "tsk.h"
#include "tsksup.h"

#include <string.h>
#include <stdarg.h>

#define BUFSIZE   1024     /* Size of the console out buffer */
#define STRLEN    256      /* Max. length of single output string */
#define STACKSIZE 2048     /* Size of stack for output task */

local buffer conout_buf;
local char conout_stack [STACKSIZE];
local char conout_bufbuf [BUFSIZE];
local char conout_str [STRLEN+1];
local tcb conout_task;

resource printf_resource;

/* -------------------------------------------------------------- */

/*
   conout: The console output task. Reads strings from the buffer
           and displays them on the console.
*/

local void Taskfunc conout (void)
{
   int siz, i;
   register int ch;

   while (1)
      {
      siz = read_buffer (&conout_buf, conout_str, STRLEN, 0L);
      for (i = 0; i < siz; i++)
         {
         switch (ch = conout_str [i])
            {
            case '\n':  putch ('\r');
                        putch ('\n');
                        break;

#if (TSK_TURBO)
            case 0x07:  sound (2000);
#if (CLOCK_MSEC)
                        t_delay (167L);
#else
                        t_delay (3L);
#endif
                        nosound ();
                        break;
#endif

            default:    putch (ch);
            }
         }
      }
}


/*
   init_conout:   Creates buffer and task. Must be called
                  before using any other routine from this module.
*/

void init_conout (void)
{
   create_buffer (&conout_buf, conout_bufbuf, BUFSIZE TN("CONOUTBF"));
   create_task (&conout_task, conout, conout_stack, STACKSIZE, PRI_STD + 100, 
                LNULL TN("CONOUT"));
   start_task (&conout_task);
   create_resource (&printf_resource TN("PRINTF"));
}


/*
   end_conout: Deletes task and buffer. Should be called before
               terminating CTask.
*/

void end_conout (void)
{
   kill_task (&conout_task);
   delete_buffer (&conout_buf);
   delete_resource (&printf_resource);
}

/* -------------------------------------------------------------- */

/*
   tprintf:    Buffered replacement for printf/cprintf.
*/

int tprintf (char *format, ...)
{
   va_list argptr;
   char buf [256];
   int res;

   va_start (argptr, format);
   request_resource (&printf_resource, 0L);
   if ((res = vsprintf (buf, format, argptr)) > 0)
      if (write_buffer (&conout_buf, buf, res, 0L) < 0)
         res = 0;
   release_resource (&printf_resource);
   va_end (argptr);
   return res;
}


/*
   tputs:      Buffered replacement for puts.
*/

int tputs (char *buf)
{
   return (write_buffer (&conout_buf, buf, strlen (buf), 0L) < 0) ? -1 : 0;
}


/*
   tputch:     Buffered replacement for putch.
*/

int tputch (int ch)
{
   return (write_buffer (&conout_buf, &ch, 1, 0L) < 0) ? EOF : ch;
}

