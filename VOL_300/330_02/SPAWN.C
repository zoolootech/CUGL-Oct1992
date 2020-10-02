
/*
   Test program for checking CTask DOS spawn compatibility.

   NOTE: Task 1 does a "screensnap" every second. This is only useful
         in a dual-monitor system.
         The monitor to use is determined by the program parameters:
            SPAWNxx   - Don't do screensnap.
            SPAWNxx M - Use mono monitor
            SPAWNxx C - Use colour monitor

   NOTE: Task 3 repeatedly opens and closes the file 'C:\STATE.RST'
         to test background DOS operations. If this file does not
         exist, or can't be opened, the program will beep (Turbo C)
         or output an error message (MS C). You should have a file
         with that name in the C: root directory, or change Task 3.

   Keyboard input while SPAWN is in control:

      e  Terminates program
      d  Snapshot dump to STDOUT
      h  Stop screensnap output
      c  Resume screensnap output
      s  Spawn COMMAND.COM. Enter EXIT to return to SPAWN.
      b  Beep (Turbo C only)
*/

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#include "tsk.h"
#include "tsksup.h"
#include "tskprf.h"

#if (TSK_MSC)
#define sound(x)
#define nosound()
#endif

#define SEROUT    1     /* If non-0, tasks will signal activity on
                           COM1-Port. */

#if (SEROUT)
#define outser(p,x)    tsk_outp(p,x)
#else
#define outser(p,x)
#endif

#define STACKSIZE 2048

unsigned int _stklen = 8 * STACKSIZE;  /* For Turbo C: Four tasks + main Task Stack */

typedef struct {
               farptr xx;
               char str [20];
               } message;

tcb tcb1, tcb2, tcb3, tcb4;
mailbox box;
message msg;
flag halt;
pipe pip;
buffer buf;

byte pipbuf [10];
word bufbuf [40];
int scrnl;
int endrun, xendrun;
char cmdname [64];
char *cmd;

#if (TSK_TURBO)

int handler (int errval, int ax, int bp, int si)
{
   return 0;
}

int cbreak (void)
{
   return 1;
}

#endif

/*
   Task1 will display a screen snap on the selected monitor every second.
*/

void far task1 (void)
{
   tprintf ("Task 1 started\n");
   while (!endrun)
      {
		t_delay (18L);
		wait_flag_clear (&halt, 0L);
      if (scrnl)
         screensnap (25);
      }
}

/*
   Task2 reads the keyboard. If a character has been read, it is passed
   to Task4 via a pipe. Entering 'h' will set the halt flag (stopping Task1),
   entering 'c' will clear the halt flag.
   'e' stops the program.
*/

void far task2 (void)
{
   int ch;

   tprintf ("Task 2 started\n");
   while (!endrun)
      {
      ch = t_wait_key (36L);
      if (ch == -1)
         outser (0x3f8, '2');
      else
         {
         ch &= 0xff;
         switch (tolower (ch))
            {
            case 'h':   set_flag (&halt);
                        break;
            case 'c':   clear_flag (&halt);
                        break;
            case 'e':   wake_task (NULL);
                        break;
   #if (TSK_NAMED)
            case 'd':   snapshot (stdout);
                        break;
   #endif
   #if (TSK_TURBO)
            case 'b':   sound (2000);
                        t_delay (2);
                        nosound ();
                        break;
   #endif
            case 's':   spawnlp (P_WAIT, cmdname, cmdname, NULL);
                        break;
            }

         if (!endrun)
            write_pipe (&pip, (char)ch, 0L);
         outser (0x3f8, 'r');
         }
      }
}


/*
   Task3 does something in the backgound.
*/

void far task3 (void)
{
   int fil;

   tprintf ("Task 3 started\n");
   while (!endrun)
      {
      t_delay (18L);
      if ((fil = open ("c:/state.rst", O_RDONLY)) == -1) 
         {
#if (TSK_MSC)
         tprintf ("\nError opening file\n");
#else
         outser (0x3f8, '!');
         sound (200);
         t_delay (2L);
         nosound ();
#endif
         }
      else
			outser (0x3f8, '+');
      close (fil);
      }
}


/*
   Task4 waits for a character in the pipe and displays it.
*/

void far task4 (void)
{
   int ch;

   tprintf ("Task 4 started\n");
   while (!endrun)
      {
      ch = read_pipe (&pip, 40L);
      if (ch < 0)
         outser (0x3f8, '4');
      else
         tprintf ("Task 4 got <%c>\n", ch);
      }
}


int main (int argc, char *argv [])
{
   char stack1 [STACKSIZE];
   char stack2 [STACKSIZE];
   char stack3 [STACKSIZE];
   char stack4 [STACKSIZE];
   char *m;
   int c;

#if(TSK_TURBO)
   harderr (handler);
   ctrlbrk (cbreak);
#endif

   c = (argc < 2) ? 'x' : tolower (argv [1][0]);
   cmd = getenv ("COMSPEC");
   strcpy (cmdname, (cmd == NULL) ? "command.com" : cmd);

   endrun = xendrun = 0;
   install_tasker (0, 0, IFL_INT8_DIR | IFL_DISK, "Spawn");
   init_conout ();
   create_task (&tcb1, task1, stack1, STACKSIZE, PRI_STD, NULL TN("TASK1"));
   create_task (&tcb2, task2, stack2, STACKSIZE, PRI_STD, NULL TN("TASK2"));
   create_task (&tcb3, task3, stack3, STACKSIZE, PRI_STD, NULL TN("TASK3"));
   create_task (&tcb4, task4, stack4, STACKSIZE, PRI_STD, NULL TN("TASK4"));
   create_mailbox (&box TN("Mailbox"));
   create_flag (&halt TN("Halt"));
   create_pipe (&pip, pipbuf, sizeof (pipbuf) TN("Pipe"));
   create_buffer (&buf, bufbuf, sizeof (bufbuf) TN("Buffer"));

   m = malloc (10000);
   malloc (1);
   free (m);

   switch (c)
      {
      case 'c':   tsk_set_colour (25, 80);
                  scrnl = 1;
                  break;
      case 'm':   tsk_set_mono (25, 80);
                  scrnl = 1;
                  break;
      default:    scrnl = 0;
                  break;
      }

   start_task (&tcb1);
   start_task (&tcb2);
   start_task (&tcb3);
   start_task (&tcb4);
   preempt_on ();

   t_delay (0L);

   endrun = 1;
   tputs ("******** Main Task *********");

   set_priority (NULL, PRI_STD);
   delete_mailbox (&box);
   delete_pipe (&pip);
   delete_buffer (&buf);
   delete_flag (&halt);

   schedule ();

   xendrun = 1;

   preempt_off ();
   end_conout ();
   remove_tasker ();
   puts ("******** End Run *********");
   return 0;
}

