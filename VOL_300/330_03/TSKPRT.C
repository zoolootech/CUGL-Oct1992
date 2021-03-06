/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKPRT.C - CTask - Printer interface routines.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   The algorithm for waiting when the printer is busy has been changed
   in version 1.2. In the previous versions, a timed delay was used after
   a certain number of times through the loop. Depending on the kind of
   printer, and the system speed, this could lead to extremely slow
   output. Now a busy waiting loop with scheduling is employed. This
   change will not likely hamper system performance significantly, but
   can speed up printer output processing.

   Also new in version 1.2 is the stack switch module, tskprti.asm. The
   interrupt routine is now called through an "envelope" that first
   switches to a local stack.
*/


#include "tsk.h"
#include "tsklocal.h"
#include "prt.h"

#if (DOS)

#define STKSZ     256   /* Printer Task stack size */

/*
   The following values may need some tuning for optimum performance.
*/
/* 
   MAX_WAIT_xx: Maximum number of times through the loop when waiting
                for the printer to get ready.
*/

#define MAX_WAIT_INT    30 /* For interrupt output */
#define MAX_WAIT_POLL   30 /* For polling output */

#if (CLOCK_MSEC)
#define INT_TIMEOUT  100L  /* Timeout for ACK interrupt wait */
#else
#define INT_TIMEOUT  2L    /* Timeout for ACK interrupt wait */
#endif



#define inta00    0x20   /* 8259 interrupt controller control-port */
#define inta01    0x21   /* 8259 interrupt controller mask-port */

#define rdserv    0x0b   /* read service register control value */
#define eoi       0x20   /* end of interrupt signal for 8259 */

#define data      0x00
#define status    0x01
#define control   0x02

#define STROBE    0x01
#define IRQENA    0x10

#define dflt_control    (INIT | SELECT)


/*
   To add support for other LPT-Ports, define
      - Port base
      - IRQ-Bit (optional)
      - Interrupt vector (optional)
   here, add the necessary data to the pr_port_descr array, and, if irq
   was specified, define the corresponding interrupt function by 
   duplicating prtint0, replacing the index into the prt_data array.
*/

#define PORTS  3              /* Number of defined ports */

#define lpt1_base    0x3bc    /* LPT1 port base (mono card) */
#define lpt2_base    0x378    /* LPT2 port base (IBM printer adapter) */
#define lpt3_base    0x278    /* LPT3 port base (secondary parallel) */

#define lpt1_irq     0x80     /* IRQ-Bit for LPT1 */
#define lpt2_irq     0x80     /* IRQ-Bit for LPT2 (same as LPT1) */
#define lpt3_irq     0x20     /* IRQ-Bit for LPT3 */

#define lpt1_vect    0x0f     /* Interrupt vector for LPT1 */
#define lpt2_vect    0x0f     /* Interrupt vector for LPT2 */
#define lpt3_vect    0x0d     /* Interrupt vector for LPT3 */

typedef struct {
               intprocptr  proc;
               int         base;
               byte        irq;
               byte        vector;
               } port_data;

extern void interrupt far tskprt_int0 (void);
extern void interrupt far tskprt_int1 (void);
extern void interrupt far tskprt_int2 (void);

port_data pr_port_descr [PORTS] = {
                             { tskprt_int0, lpt1_base, lpt1_irq, lpt1_vect },
                             { tskprt_int1, lpt2_base, lpt2_irq, lpt2_vect },
                             { tskprt_int2, lpt3_base, lpt3_irq, lpt3_vect }
                                  };

/*-------------------------------------------------------------------------*/

typedef struct {
               intprocptr savvect;     /* Interrupt vector save location */
               int      port_base;     /* Port base I/O address */
               byte     irqbit;        /* IRQ-Bit for this port */
               byte     civect;        /* Interrupt Vector for this port */
               byte     wait_xmit;     /* Transmit delayed */
               byte     xmit_pending;  /* Transmit in progress */
               byte     polling;       /* Use polling if on */
               byte     ccontrol;      /* Current control reg */
               pipe     xmit_pipe;     /* Transmit pipe */
               flag     pready;        /* Printer ready flag */
               tcb      pr_task;       /* Printer task */
               byte     pr_stack [STKSZ]; /* Printer task stack */
               } prt_datarec;

typedef prt_datarec *prtptr;

local prt_datarec prt_data [PORTS];
local int prt_installed [PORTS] = { 0 };
local callchain remove_chain = { LNULL, LNULL };

/*-------------------------------------------------------------------------*/


void Localfunc tsk_prt_int (int port)
{
   prtptr prt = &prt_data [port];

   /* Turn off int */
   prt->ccontrol &= ~IRQENA;
   tsk_outp (prt->port_base + control, prt->ccontrol);

   /* Signal print char complete */
   set_flag (&prt->pready);
}


/*-------------------------------------------------------------------------*/


local void Staticfunc pr_toggle_strobe (int port_base, byte ctl)
{
   tsk_outp (port_base + control, ctl | STROBE);
   tsk_nop ();
   tsk_outp (port_base + control, ctl);
}

local int Staticfunc pr_ready (int port_base)
{
   return (tsk_inp (port_base + status) & (BUSY | ACK | PEND | SELIN | ERROR))
          == (BUSY | ACK | SELIN | ERROR);
}


local void Staticfunc pr_output_poll (prtptr prt, byte ch)
{
   int wait_count;
   int port_base;

   port_base = prt->port_base;
   while (!pr_ready (port_base))
      {
      for (wait_count = 0; wait_count < MAX_WAIT_POLL; wait_count++)
         if (pr_ready (port_base))
            break;
      if (!pr_ready (port_base))
         yield ();
      }
   tsk_outp (port_base + data, ch);
   pr_toggle_strobe (port_base, prt->ccontrol);
}


local void Staticfunc pr_output_int (prtptr prt, byte ch)
{
   int wait_count;
   int port_base;

   port_base = prt->port_base;
   while (!pr_ready (port_base))
      {
      for (wait_count = 0; wait_count < MAX_WAIT_INT; wait_count++)
         if (pr_ready (port_base))
            break;
      if (!pr_ready (port_base))
         yield ();
      }

   clear_flag (&prt->pready);

   tsk_outp (port_base + data, ch);
   pr_toggle_strobe (port_base, (byte)(prt->ccontrol | IRQENA));

   for (wait_count = 0; wait_count < MAX_WAIT_INT; wait_count++)
      if (check_flag (&prt->pready))
         return;

   if (wait_flag_set (&prt->pready, INT_TIMEOUT) < 0)
      tsk_outp (prt->port_base + control, prt->ccontrol &= ~IRQENA);
}


local void Taskfunc pr_task (prtptr prt)
{
   int ch;

   while (1)
      {
      ch = read_pipe (&prt->xmit_pipe, 0L);
      if (prt->polling)
         pr_output_poll (prt, (byte)ch);
      else
         pr_output_int (prt, (byte)ch);
      }
}

/*-------------------------------------------------------------------------*/

local void Taskfunc prt_chain_remove (callchainptr chain)
{
   /* 
      We just assume here that all local data is in the same segment.
      This should be true for all compilers and memory models
      unless you define an extremely low data threshold.
   */
   prt_remove_all ();
}


int Globalfunc prt_install (int port, byte polling, word prior,
                              farptr xmitbuf, word xmitsize)
{
   prtptr prt;
   int pbase;
   intprocptr far *intptr;
#if (TSK_NAMEPAR)
   static char name [] = "PRTn";

   name [3] = (char)((port & 0x7f) + '0');
#endif

   if (port & 0x80)
      {
      port &= 0x7f;
      if (port > 4)
         return -1; 
      pbase = *((wordptr)(TMK_FP (0x40, port * 2 + 8)));
      if (!pbase)
         return -1;

      for (port = 0; port < PORTS; port++)
         if (pr_port_descr [port].base == pbase)
            break;
      }

   if (port < 0 || port >= PORTS || !xmitsize || prt_installed [port])
      return -1;

   prt = &prt_data [port];

   if (create_pipe (&prt->xmit_pipe, xmitbuf, xmitsize TN(name)) == LNULL)
      return -1;

   create_flag (&prt->pready TN(name));

   create_task (&prt->pr_task, pr_task, prt->pr_stack, STKSZ, prior, 
                prt TN(name));

   pbase = prt->port_base = pr_port_descr [port].base;
   prt->civect = pr_port_descr [port].vector;
   prt->irqbit = pr_port_descr [port].irq;
   prt->ccontrol = dflt_control;
   prt->wait_xmit = prt->xmit_pending = 0;

   if (!prt->irqbit)
      polling = 1;

   prt->polling = polling;

   tsk_outp (pbase + control, dflt_control);
   if (!polling)
      {
      intptr = (intprocptr far *)TMK_FP (0, prt->civect * 4);
      tsk_cli ();
      prt->savvect = *intptr;
      *intptr = pr_port_descr [port].proc;
      tsk_sti ();
      tsk_outp (inta01, tsk_inp (inta01) & ~prt->irqbit);
      }

   prt_installed [port] = 1;
   if (remove_chain.func == LNULL)
      chain_removefunc (prt_chain_remove, &remove_chain, NULL);

   start_task (&prt->pr_task);

   return port;
   }


void Globalfunc prt_remove (int port)
   {
   prtptr prt;
   intprocptr far *intptr;

   if ((port & 0xfff0) || !prt_installed [port])
      return;
   prt = &prt_data [port];

   tsk_outp (prt->port_base + control, prt->ccontrol & ~IRQENA);

   if (!prt->polling)
      {
      tsk_outp (inta01, tsk_inp (inta01) | prt->irqbit);
      intptr = (intprocptr far *)TMK_FP (0, prt->civect * 4);
      tsk_cli ();
      *intptr = prt->savvect;
      tsk_sti ();
      }
   kill_task (&prt->pr_task);
   delete_pipe (&prt->xmit_pipe);
   delete_flag (&prt->pready);

   prt_installed [port] = 0;
   }


void Taskfunc prt_remove_all (void)
{
   int i;

   unchain_removefunc (&remove_chain);

   for (i = 0; i < PORTS; i++)
      if (prt_installed [i])
         prt_remove (i);
}


/*-------------------------------------------------------------------------*/


void Globalfunc prt_change_control (int port, byte ctl)
{
   prtptr prt;
   CRITICAL;

   if ((port & 0xfff0) || !prt_installed [port])
      return;
   prt = &prt_data [port];
   C_ENTER;
   prt->ccontrol = (byte)((prt->ccontrol & ~(AUTOFEED | INIT | SELECT))
                          | (ctl & (AUTOFEED | INIT | SELECT)));
   C_LEAVE;
   tsk_outp (prt->port_base + control, prt->ccontrol);
}


int Globalfunc prt_write (int port, byte ch, dword timeout)
{
   if ((port & 0xfff0) || !prt_installed [port])
      return -1;
   return write_pipe (&prt_data [port].xmit_pipe, ch, timeout);
}


int Globalfunc prt_status (int port)
{
   if ((port & 0xfff0) || !prt_installed [port])
      return -1;
   return tsk_inp (prt_data [port].port_base + status);
}


int Globalfunc prt_complete (int port)
{
   if ((port & 0xfff0) || !prt_installed [port])
      return -1;
   return (check_pipe (&prt_data [port].xmit_pipe) == -1);
}


int Globalfunc prt_wait_complete (int port, dword timeout)
{
   if ((port & 0xfff0) || !prt_installed [port])
      return -1;
   return wait_pipe_empty (&prt_data [port].xmit_pipe, timeout);
}

void Globalfunc prt_flush (int port)
{
   if ((port & 0xfff0) || !prt_installed [port])
      return;
   flush_pipe (&prt_data [port].xmit_pipe);
}

#endif

