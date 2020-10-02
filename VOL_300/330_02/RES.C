
/*
   Test program for checking CTask DOS TSR compatibility.

   This program goes TSR, and then repeatedly reads a file, and
   outputs status messages to the serial port.

   The program is called with res [filename] [port]
   where [port] is  "-"    output to COM1 (relative)
                    "-1"   output to COM1 (relative)
                    "-2"   output to COM2 (relative)
                    "1"    output to COM1 (absolute)
   If no filename is given, nothing is done in the background.
   If no port is given, COM1 (absolute) is used.
*/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <io.h>
#include <errno.h>

#include "tsk.h"
#include "sio.h"

#define PORT   0x00     /* COM1, absolute */
#define BAUD   9600L    /* Baudrate */

#define STACKSIZE 2048

unsigned int _stklen = 2 * STACKSIZE;  /* For Turbo C: One task + main task Stack */

tcb tcb1;
sioptr siop;

byte fbuf[512];
char fname[128];
word rcvbuf [128];
byte xmtbuf [128];

int ofile;

#if (TSK_TURBO)
#define stayres(len)    keep (0, (len))
#else
#define stayres(len)    _dos_keep (0, (len))
#define sound(x)
#define nosound()
#endif

struct mem_control {
                   byte id;
                   word owner;
                   word paragraphs;
                   };

void exit_resident (void)
{
   struct mem_control far *mem;

   mem = MK_FP (_psp - 1, 0);
   stayres (mem->paragraphs);
}


void comprintf (char *fmt, ...)
{
   va_list argptr;
   char buf [256];
   int res, i;

   va_start (argptr, fmt);
   res = vsprintf (buf, fmt, argptr);
   for (i = 0; i < res; i++)
      v24_send (siop, buf [i], 0L);
   va_end (argptr);
}


/*
   Task1 does something in the backgound.
*/

void far task1 (void)
{
  int retval;

   while (1) 
      {
      t_delay (36L);  /* 2 seconds = 36 */
      if (fname [0]) 
         {
         if ((ofile = open (fname, O_RDONLY)) == -1) 
            {
            comprintf ("*** Open file ERROR %d ***\n\n", errno);
            sound (800);
            t_delay (2L);
            nosound ();
            }
         else 
            {
            comprintf ("Open file OK\n");
            while ((retval = read (ofile, fbuf, 512)) > 0) 
               {
	            t_delay (2L);
               comprintf ("Read block OK\n");
               }
            if (retval == -1)
               {
               comprintf ("*** Read block ERROR %d ***\n", errno);
	            sound (200);
               t_delay (8L);
               nosound ();
               }
            else 
               {
               comprintf ("Read File Complete\n\n");
               sound (4000);
               t_delay (1L);
               nosound ();
               }
            close (ofile);
            }
         }
      else 
         {
         sound (200);
         t_delay (1L);
         nosound ();
         }
      }
}


int main (int argc, char *argv [])
{
   char stack1 [STACKSIZE];
   int port, i;
   
   if (argc >= 2) 
      strcpy (fname, argv [1]);
   else 
      fname[0] = '\0';

   port = PORT;
   if (argc > 2)
      {
      port = 0;
      i = 0;
      if (argv [2][0] == '-')
         {
         port = 0x80;
         i = 1;
         }
      if (argv [2][i] >= '1' && argv [2][i] <= '3')
         port |= argv [2][i] - '1';
      }

   install_tasker (0, 0, IFL_STD, "Tsio");
   siop = v24_install (port, 1, rcvbuf, sizeof (rcvbuf), 
                       xmtbuf, sizeof (xmtbuf));

   if (siop == NULL)
      {
      remove_tasker ();
      return 1;
      }

   v24_change_baud (siop, BAUD);
   v24_protocol (siop, XONXOFF, 40, 60);

   create_task (&tcb1, task1, stack1, STACKSIZE, PRI_STD, NULL TN("SIO-TASK"));

   start_task (&tcb1);
   set_priority (NULL, PRI_STD);
   preempt_on ();
   schedule ();
   exit_resident ();
   return 0; /* dummy */
}


