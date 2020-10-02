
/*
   Test program for checking basic CTask functions.

   Keyboard input:

      e  Terminates program
      d  Snapshot dump to STDOUT
      h  Stop screen output
      c  Resume screen output
      b  Beep (Turbo C only)
      m  Snapshot dump to mono screen
      s  Snapshot dump to colour screen

*/

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "tsk.h"
#include "tsksup.h"
#include "tskprf.h"

#define STACKSIZE 2048

unsigned int _stklen = 6 * STACKSIZE;  /* For Turbo C: Five tasks + main Task Stack */

typedef struct {
               farptr xx;
               char str [20];
               } message;

tcb tcb1, tcb2, tcb3, tcb4, tcb5;
mailbox box;
message msg;
flag halt;
pipe pip;
buffer buf;

byte pipbuf [10];
word bufbuf [40];

int endrun;


/*
   Task1 sends a mail to Task3, and waits for a response in the buffer.
   The response is then displayed.
   Task1 will stop while the halt flag is set.
*/

void far task1 (void)
{
   char str [20];

   tprintf ("Task 1 started\n");
   while (!endrun)
      {
      wait_flag_clear (&halt, 0L);

      t_delay (5L);
      tputch ('1');
      strcpy (msg.str, "From T1");
      send_mail (&box, &msg);

      read_buffer (&buf, str, 20, 0L);
      tprintf ("Task 1 read buf: <%s>\n", str);
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
      if (ch < 0)
         tputch (0x04);
      else
         {
         switch (tolower (ch & 0xff))
            {
            case 'h':   set_flag (&halt);
                        break;
            case 'c':   clear_flag (&halt);
                        break;
            case 'e':   wake_task (NULL);
                        break;
   #if (TSK_NAMED)
            case 'd':   csnapshot ();
                        break;
            case 'm':   tsk_set_mono (25, 80);
                        screensnap (25);
                        break;
            case 's':   tsk_set_colour (25, 80);
                        screensnap (25);
                        break;
   #endif
   #if (TSK_TURBO)
            case 'b':   sound (2000);
                        t_delay (2);
                        nosound ();
                        break;
   #endif
            }

         if (!endrun)
            write_pipe (&pip, (char)ch, 0L);
         tputch ('2');
         }
      }
}


/*
   Task3 waits for mail, then sends it back through a buffer.
*/

void far task3 (void)
{
   message far *m;

   tprintf ("Task 3 started\n");
   while (!endrun)
      {
      m = wait_mail (&box, 0L);
      tprintf ("Task 3 received <%Fs>\n", m->str);

      m->str [6] = '3';
      write_buffer (&buf, m->str, 7, 0L);
      }
}


/*
   Task4 waits for a character in the pipe and displays it. To make
   things livelier, it uses a timeout while waiting, and will display
   faces when the timeout occurred before the character.
*/

void far task4 (void)
{
   int ch;

   tprintf ("Task 4 started\n");
   while (!endrun)
      {
      ch = read_pipe (&pip, 10L);
      if (ch < 0)
         tputch (0x02);
      else
         tprintf ("Task 4 got <%c>\n", ch);
      }
}

/*
   Task 5 checks the memory watch capability by waiting for the
   BIOS keyboard status byte at 40:17 to contain 1 in the lower two
   bits, signalling that both the left and right shift keys are pressed.
*/

void far task5 (void)
{
   tprintf ("Task 5 started\n");
   while (!endrun)
      {
         wait_memory ((farptr)0x417L, 0x03, 0x03, TCMP_EQ);
         tprintf ("Task 5: Both Shift keys pressed\n");
         wait_memory ((farptr)0x417L, 0x03, 0x03, TCMP_NE);
         tprintf ("Task 5: Shift keys released\n");
      }
}


int main (void)
{
   char stack1 [STACKSIZE];
   char stack2 [STACKSIZE];
   char stack3 [STACKSIZE];
   char stack4 [STACKSIZE];
   char stack5 [STACKSIZE];
   int i;

#if(TSK_TURBO)
   directvideo = 0;
#endif
   endrun = 0;
   if ((i = install_tasker (0, 0, IFL_DISK | IFL_INT8_DIR | IFL_PRINTER, "Test")) < 0)
      {
      printf ("Can't install, returncode = %d\n", i);
      return 1;
      }

   init_conout ();

   create_mailbox (&box TN("Mailbox"));
   create_flag (&halt TN("Halt"));
   create_pipe (&pip, pipbuf, sizeof (pipbuf) TN("Pipe"));
   create_buffer (&buf, bufbuf, sizeof (bufbuf) TN("Buffer"));

   create_task (&tcb1, task1, stack1, STACKSIZE, PRI_STD, NULL TN("TASK1"));
   create_task (&tcb2, task2, stack2, STACKSIZE, PRI_STD, NULL TN("TASK2"));
   create_task (&tcb3, task3, stack3, STACKSIZE, PRI_STD, NULL TN("TASK3"));
   create_task (&tcb4, task4, stack4, STACKSIZE, PRI_STD, NULL TN("TASK4"));
   create_task (&tcb5, task5, stack5, STACKSIZE, PRI_STD, NULL TN("TASK5"));
   start_task (&tcb1);
   start_task (&tcb2);
   start_task (&tcb3);
   start_task (&tcb4);
   start_task (&tcb5);
   preempt_on ();

   t_delay (0L);

   endrun = 1;
   tputs ("******** Main Task *********");

   kill_task (&tcb1);
   kill_task (&tcb2);
   kill_task (&tcb3);
   kill_task (&tcb4);
   kill_task (&tcb5);

   delete_mailbox (&box);
   delete_pipe (&pip);
   delete_buffer (&buf);
   delete_flag (&halt);

   schedule ();

   end_conout ();

   remove_tasker ();
   puts ("******** End Run *********");
   return 0;
}

