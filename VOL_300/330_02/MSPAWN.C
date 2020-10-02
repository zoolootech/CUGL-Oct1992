
/*
   Minimal test program for checking CTask DOS spawn compatibility.

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
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <dos.h>

#include "tsk.h"
#include "tsksup.h"
#include "tskprf.h"
#include "kbd.h"

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

#define STACKSIZE 1024

unsigned int _stklen = 3 * STACKSIZE;  /* For Turbo C: Two tasks + main Task Stack */

tcb tcb1, tcb3;
int scrnl, cscreen;
int endrun, xendrun;
char cmdname [64];
char *cmd;
struct {
         word environ;
         farptr cmdtail;
         farptr fcb1;
         farptr fcb2;
         } exepar;

byte cmdtail [2] = { 0, 0x0d };
byte dumfcb [] = { "           " };

tlink hotkey_mono;
tlink hotkey_colour;
tlink hotkey_none;
int hot_screen = 0;

void Taskfunc hotproc_mono (tlinkptr elem)
{
   hot_screen = 'm';
   wake_task (&tcb1);
}

void Taskfunc hotproc_colour (tlinkptr elem)
{
   hot_screen = 'c';
   wake_task (&tcb1);
}

void Taskfunc hotproc_none (tlinkptr elem)
{
   hot_screen = -1;
   wake_task (&tcb1);
}


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
   while (!endrun)
      {
		t_delay (18L);
      if (hot_screen > 0 && hot_screen != cscreen)
         switch (cscreen = hot_screen)
            {
            case 'c':   tsk_set_colour (25, 80);
                        break;
            case 'm':   tsk_set_mono (25, 80);
                        break;
            default:    hot_screen = 0;
                        break;
            }

      if (hot_screen >= 0 && (scrnl || hot_screen))
         {
         hot_screen = 0;
         screensnap (25);
         }
      }
}


/*
   Task3 does something in the backgound.
*/

void far task3 (void)
{
   static char far *fname = "c:/state.rst";
   union REGS regs;
   struct SREGS sregs;

   while (!endrun)
      {
      t_delay (18L);
      regs.x.ax = 0x3d00;
      regs.x.dx = TFP_OFF(fname);
      sregs.ds = TFP_SEG(fname);
      intdosx (&regs, &regs, &sregs);

      if (regs.x.cflag != 0) 
         {
         outser (0x3f8, '!');
#if (!TSK_MSC)
         sound (200);
         t_delay (2L);
         nosound ();
#endif
         }
      else
			{
         outser (0x3f8, '+');
         regs.x.bx = regs.x.ax;
         regs.h.ah = 0x3e;
         intdos (&regs, &regs);
         }
      }
}


int main (int argc, char *argv [])
{
   char stack1 [STACKSIZE];
   char stack3 [STACKSIZE];
   struct SREGS sregs;
   union REGS regs;
   int c;

#if(TSK_TURBO)
   harderr (handler);
   ctrlbrk (cbreak);
#endif

   c = (argc < 2) ? 'x' : tolower (argv [1][0]);
   cmd = getenv ("COMSPEC");
   strcpy (cmdname, (cmd == NULL) ? "command.com" : cmd);

   exepar.cmdtail = cmdtail;
   exepar.environ = 0;
   exepar.fcb1 = dumfcb;
   exepar.fcb2 = dumfcb;
   regs.x.ax = 0x4b00;
   regs.x.dx = TFP_OFF(cmdname);
   sregs.ds = TFP_SEG(cmdname);
   regs.x.bx = TFP_OFF(&exepar);
   sregs.es = TFP_SEG(&exepar);

   endrun = xendrun = 0;
   install_tasker (0, 0, IFL_INT8_DIR, "Spawn");
   create_task (&tcb1, task1, stack1, STACKSIZE, PRI_STD, NULL TN("SnapTask"));
   create_task (&tcb3, task3, stack3, STACKSIZE, PRI_STD, NULL TN("DosCTask"));

   switch (c)
      {
      case 'c':   tsk_set_colour (25, 80);
                  scrnl = cscreen = 'c';
                  break;
      case 'm':   tsk_set_mono (25, 80);
                  scrnl = cscreen = 'm';
                  break;
      default:    scrnl = cscreen = 0;
                  break;
      }

   start_task (&tcb1);
   start_task (&tcb3);
   preempt_on ();

   create_hotkey_entry (&hotkey_mono, SCAN_M, 
                        KF1_ALT | KF1_CONTROL | KF1_LEFTSHIFT, KF1_CONTROL | KF1_LEFTSHIFT,
                        0, 0,
                        0, 0,
                        (farptr)hotproc_mono, TKIND_PROC, 1, LNULL);
   create_hotkey_entry (&hotkey_colour, SCAN_C, 
                        KF1_ALT | KF1_CONTROL | KF1_LEFTSHIFT, KF1_CONTROL | KF1_LEFTSHIFT,
                        0, 0,
                        0, 0,
                        (farptr)hotproc_colour, TKIND_PROC, 1, LNULL);
   create_hotkey_entry (&hotkey_none, SCAN_N, 
                        KF1_ALT | KF1_CONTROL | KF1_LEFTSHIFT, KF1_CONTROL | KF1_LEFTSHIFT,
                        0, 0,
                        0, 0,
                        (farptr)hotproc_none, TKIND_PROC, 1, LNULL);

   set_priority (NULL, PRI_STD);
   tsk_printf ("Spawning...\n");
   intdosx (&regs, &regs, &sregs);
   tsk_printf ("Returned from spawn, Carry = %d, AX = %04X\n", 
               regs.x.cflag, regs.x.ax);

   endrun = 1;
   tsk_puts ("******** Main Task *********");

   schedule ();

   xendrun = 1;
   delete_hotkey (&hotkey_mono);
   delete_hotkey (&hotkey_colour);
   delete_hotkey (&hotkey_none);

   preempt_off ();
   remove_tasker ();
   tsk_puts ("******** End Run *********");
   return 0;
}

