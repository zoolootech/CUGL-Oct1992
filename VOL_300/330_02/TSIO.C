
/*
   Test program for checking the CTask serial I/O interface.
*/

#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <process.h>

#include "tsk.h"
#include "sio.h"
#include "tsksup.h"

#define PORT   0x80     /* COM1, relative */
#define BAUD   9600L    /* Baudrate */


#define STACKSIZE 2048

unsigned int _stklen = 4 * STACKSIZE;  /* For Turbo C: Two tasks + main Task Stack */

tcb tcb1, tcb2, tcb3;
flag halt;
sioptr siop;

word rcvbuf [10000];
byte xmtbuf [100];

int endrun, err;

/*
   Task 1 reads characters from the serial line and displays them on
   the screen. While the halt flag is set, characters are not read,
   so the XON/XOFF and RTS/CTS protocol can be tested for the receiving 
   side.
*/

void far task1 (void)
{
   word ch;

   printf ("Task 1 started\n");
   while (!endrun)
      {
      wait_flag_clear (&halt, 0L);
      if (endrun)
         return;
      ch = v24_receive (siop, 0L);
      putch (ch);
      if (ch & 0xff00)
         {
         err = 1;
         printf ("\n%c*%02x*", ch, ch >> 8);
         }
      }
}


/*
   Task 2 reads characters from the keyboard and sends them to the
   serial port. If 'h' is entered, the halt flag is set, so task1
   stops reading. If 'c' is entered, the halt flag is cleared.
   Entering 'e' stops the program.
   'd' outputs snapshot dump.
*/

void far task2 (void)
{
   int ch;

   printf ("Task 2 started\n");
   while (!endrun)
      {
      ch = t_read_key () & 0xff;
      switch (tolower (ch))
         {
         case 'h':   set_flag (&halt);
                     puts ("-halt-");
                     break;

         case 'c':   clear_flag (&halt);
                     err = 0;
                     puts ("-continue-");
                     break;

         case 'e':   puts ("-end-");
                     endrun = 1;
                     clear_flag (&halt);
                     wake_task (NULL);
                     break;

#if (TSK_NAMED)
         case 'd':   snapshot (stdout);
                     break;
#endif

         default:    /* putch (ch); */
                     v24_send (siop, (byte)ch, 0L);
                     break;
         }
      }
}


int main (void)
{
   char stack1 [STACKSIZE];
   char stack2 [STACKSIZE];

   endrun = 0;

   install_tasker (0, 0, IFL_STD, "TSIO");
   siop = v24_install (PORT, 1, rcvbuf, sizeof (rcvbuf), xmtbuf, sizeof (xmtbuf));

   if (siop == NULL)
      {
      remove_tasker ();
      printf ("Couldn't install COM-Port\n");
      exit (1);
      }
   v24_change_baud (siop, BAUD);
   v24_protocol (siop, XONXOFF | RTSCTS, 40, 60);

   create_task (&tcb1, task1, stack1, STACKSIZE, PRI_STD, NULL TN("TASK1"));
   create_task (&tcb2, task2, stack2, STACKSIZE, PRI_STD, NULL TN("TASK2"));

   create_flag (&halt TN("Halt"));

   start_task (&tcb1);
   start_task (&tcb2);

   preempt_on ();
   t_delay (0L);

   endrun = 1;
   puts ("******** Main Task *********");

   set_priority (NULL, 10);
   schedule ();
   delete_flag (&halt);

   preempt_off ();
   v24_remove (siop, 1);
   remove_tasker ();

   puts ("******** End Run *********");
   return 0;
}

