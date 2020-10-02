/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKSIO.C - CTask - Serial I/O interface routines.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   With version 1.1, support for shared IRQ lines and dynamically
   defined ports was added. The tables used are

      port_list   The pointer to the list of defined ports. Newly
                  defined ports are added to the end of the list.
                  Ports can never be deleted. Each port descriptor
                  contains the hardware info for the port, plus a
                  pointer to the associated sio-control-block if the
                  port was initialized.
                  
      port_last   The pointer to the last element in port_list.

      irq_array   Is an array of pointers to sio-control-blocks. For
                  each possible IRQ-line the entry in this table points
                  to the first block active for this line. If the IRQ
                  is shared, sio-blocks are chained into this list via 
                  their "next" pointer.

      irq_procs   Contains the pointer to the interrupt handler function
                  for the corresponding IRQ-line.

      sio_data    Contains the statically defined sio control blocks.
      port_descr  Contains the statically defined port descriptor blocks.

   NOTE:    You can not dynamically define ports for IRQ-lines that
            have no interrupt handler function defined. To be completely
            flexible, you would have to define sioint-handlers for all
            possible IRQs and enter the addresses into the irq_procs array.

   CAUTION: Please restrict the installation and removal of v24-
            ports to *one* task. The manipulation of the lists is
            not protected, so simultaneous install/remove calls
            may cause trouble. Since ports are normally installed and
            removed in the main task, the protection of critical regions
            seemed unnecessary.

   CAUTION: Shared interrupt logic and IRQ-lines above 4 were not
            tested due to the lack of suitable hardware. Changes may
            be necessary in the handling of the modem control register
            OUT2 line that normally controls interrupt enable, depending
            on the multiport-hardware installed in the target system.

   With Version 1.2, special interrupt handler functions have been added
   in a separate assembler file, TSKSIOI.ASM. These functions switch to
   a local stack, and then call the sioint-functions defined here, which
   no longer have the interrupt attribute.

*/

#include "tsk.h"
#include "tsklocal.h"
#include "sio.h"

#if (DOS)

#define MAX_IRQ   16    /* Maximum number of interrupt lines, 
                           16 for AT, 8 for XT. Can be left at 16 */

#define CHAIN_IRQBIT    0x04  /* Chained int controller IRQ bit */

#define RESTORE_DEFAULT 1     /* Restore parameter for remove_all */

#define RTS       0x02
#define DTR       0x01
#define OUT2      0x08

#define ERR_MASK  0x1e

#define inta00    0x20   /* 8259 interrupt controller control-port */
#define inta01    0x21   /* 8259 interrupt controller mask-port */

#define inta10    0xa0   /* secondary 8259 control-port for IRQ 8-15 */
#define inta11    0xa1   /* secondary 8259 mask-port for IRQ 8-15 */

#define rdserv    0x0b   /* read service register control value */
#define eoi       0x20   /* end of interrupt signal for 8259 */

#define intdata   0x0b   /* Enable Interrupts except Line status */

#define rxreadybit 0x01
#define txemptybit 0x40
#define txreadybit 0x20
#define framingbit 0x08
#define breakbit   0x10

/* Note: In version 1.1, port offsets start at 0, not 8 */

#define linecontrol  0x03
#define linestatus   0x05
#define intid        0x02
#define intenable    0x01
#define modemcontrol 0x04
#define modemstatus  0x06
#define receivedata  0x00
#define transmitdata 0x00

#define baudreg_dll  0x00     /* baud rate least significant byte */
#define baudreg_dlm  0x01     /* baud rate most significant byte */

/*
   Default values for initialising the ports.
   Change to your liking (but remember that OUT2 must be set in the
   modem control register to allow interrupts to come through with 
   normal controllers).
*/

#define dflt_modcon 0x0b   /* Modem Control: Activate DTR, RTS, OUT2 */
#define dflt_baud   96     /* Baud Rate Divisor: 1200 Baud */
#define dflt_lcon   0x03   /* Line Control: No Parity, 1 Stop, 8 Data */

/*
   Defined baud rates. You may expand this table with non-standard
   rates if desired.
*/

local long baud_table [] = {
                               50L, 2304L,
                               75L, 1536L,
                              110L, 1047L,
                              134L,  857L,
                              150L,  768L,
                              300L,  384L,
                              600L,  192L,
                             1200L,   96L,
                             1800L,   64L,
                             2000L,   58L,
                             2400L,   48L,
                             3600L,   32L,
                             4800L,   24L,
                             7200L,   16L,
                             9600L,   12L,
                            19200L,    6L,
                            38400L,    3L,
                                0L,    0L };

local byte force_transmit_ready;       /* flag to indicate
                                          transmitter needs service */

/*-------------------------------------------------------------------------*/

/*
   To add static support for other COM-Ports, define
      - Port base
      - IRQ-Line
      - Interrupt vector
   here, and add the necessary data to the port_descr array.
   If the port does *not* share an IRQ with the predefined ports,
   define the corresponding interrupt function by duplicating both siointx
   here and _tsksio_intx in the TSKSIOI.ASM file, and place the
   entry for the _tsksio_intx function into the irq_procs array.

   Note that ports may also be defined on-line if TSK_DYNAMIC is enabled.
*/

#define STATIC_PORTS  2       /* Number of statically defined ports */

/* Note: In version 1.1, port offsets start at 0, not 8 */

#define com1_base    0x3f8    /* COM1 port base */
#define com2_base    0x2f8    /* COM2 port base */

#define com1_irq     4        /* IRQ-Line for COM1 */
#define com2_irq     3        /* IRQ-Line for COM2 */

#define com1_vect    0x0c     /* Interrupt vector for COM1 */
#define com2_vect    0x0b     /* Interrupt vector for COM2 */

/*-------------------------------------------------------------------------*/


extern void interrupt far tsksio_int2 (void);
extern void interrupt far tsksio_int3 (void);
extern void interrupt far tsksio_int4 (void);
extern void interrupt far tsksio_int5 (void);
extern void interrupt far tsksio_int7 (void);
extern void interrupt far tsksio_int10 (void);
extern void interrupt far tsksio_int11 (void);
extern void interrupt far tsksio_int12 (void);
extern void interrupt far tsksio_int15 (void);

/* 
   Table of Interrupt handler functions for each IRQ line.
*/

local intprocptr irq_procs [MAX_IRQ] = {  LNULL,         /* IRQ 0 */
                                          LNULL,         /* IRQ 1 */
                                          tsksio_int2,   /* IRQ 2 */
                                          tsksio_int3,   /* IRQ 3 */
                                          tsksio_int4,   /* IRQ 4 */
                                          tsksio_int5,   /* IRQ 5 */
                                          LNULL,         /* IRQ 6 */
                                          tsksio_int7,   /* IRQ 7 */
                                          LNULL,         /* IRQ 8 */
                                          LNULL,         /* IRQ 9 */
                                          tsksio_int10,  /* IRQ 10 */
                                          tsksio_int11,  /* IRQ 11 */
                                          tsksio_int12,  /* IRQ 12 */
                                          LNULL,         /* IRQ 13 */
                                          LNULL,         /* IRQ 14 */
                                          tsksio_int15   /* IRQ 15 */
                                          };


local sio_datarec sio_data [STATIC_PORTS];

/* When adding entries to port_descr, be sure to chain the 
   elements in ascending order via the first field, and to
   increase the internal port number in the second field. */

local port_data port_descr [STATIC_PORTS] = {
     { &port_descr[1], 0, LNULL, com1_base, com1_irq, com1_vect },
     { LNULL,          1, LNULL, com2_base, com2_irq, com2_vect }
                                            };

local sioptr  irq_array [MAX_IRQ] = { LNULL };

local portptr port_list = &port_descr [0];
local portptr port_last = &port_descr [1];

local int ports = STATIC_PORTS;
local callchain remove_chain = { LNULL, LNULL };

/*-------------------------------------------------------------------------*/


local void Staticfunc change_rts (sioptr data, int on)
{
   data->rtsoff = (byte)(!on);
   data->cmodcontrol = (byte)((data->cmodcontrol & ~RTS) | ((on) ? RTS : 0));
   tsk_outp (data->port_base + modemcontrol, data->cmodcontrol);
}


local void Staticfunc transmit_ready (sioptr data)
{
   int i;
#if (TSK_MSC)
   int temp;
#endif

   force_transmit_ready = 0;

   if ((i = data->r_xoff) < 0)
      {
#if (TSK_MSC)
      temp = (i == -1) ? XOFF : XON;
      tsk_outp (data->port_base + transmitdata, temp);
#else
      /* NOTE: Microsoft C 5.0 generates an "Internal Compiler Error"
               when compiling the following statement.
      */
      tsk_outp (data->port_base + transmitdata, (i == -1) ? XOFF : XON);
#endif
      data->r_xoff = (i == -1) ? 1 : 0;
      data->xmit_pending = 1;
      return;
      }

   data->xmit_pending = 0;

   if ((data->wait_xmit = (byte)(check_pipe (&data->xmit_pipe) != -1)) == 0)
      return;

   if ((data->modem_flags & data->modstat) ^ data->modem_flags)
      return;

   if (data->flags & XONXOFF && data->t_xoff)
      return;

   data->wait_xmit = 0;

   if ((i = c_read_pipe (&data->xmit_pipe)) < 0)
      return;

   tsk_outp (data->port_base + transmitdata, (byte)i);
   data->xmit_pending = 1;
}


local void Staticfunc modem_status_int (sioptr data)
{
   data->modstat = tsk_inp (data->port_base + modemstatus);

   if (data->wait_xmit)
      transmit_ready (data);
}


local void Staticfunc receive_ready (sioptr data)
{
   word status;
   word ch;

   while ((status = tsk_inp (data->port_base + linestatus)) & rxreadybit)
      {

      /* Correct for possible loss of transmit interrupt from IIR register */   
      if (status & txreadybit)
          force_transmit_ready = 1;

      tsk_nop ();
      ch = tsk_inp (data->port_base + receivedata);

      if (data->flags & XONXOFF)
         {
         if (ch == XON)
            {
            data->t_xoff = 0;
            if (data->wait_xmit)
               transmit_ready (data);
            continue;
            }
         else if (ch == XOFF)
            {
            data->t_xoff = 1;
            continue;
            }
         if (!data->r_xoff && 
             wpipe_free (&data->rcv_pipe) < data->xoff_threshold)
            {
            data->r_xoff = -1;
            if (!data->xmit_pending)
               transmit_ready (data);
            }
         }

      if (data->flags & RTSCTS && !data->rtsoff)
         if (wpipe_free (&data->rcv_pipe) < data->xoff_threshold)
            change_rts (data, 0);

      status = (status & ERR_MASK) << 8;
      if (c_write_wpipe (&data->rcv_pipe, ch | status) < 0)
         data->overrun = 1;
      }
}


/*-------------------------------------------------------------------------*/


local void Staticfunc sioint (sioptr data)
{
   int id;

   force_transmit_ready = 0;

   while (!((id = tsk_inp (data->port_base + intid)) & 1))
      switch (id & 0x07)
         {
         case 0x00:  modem_status_int (data);
                     break;

         case 0x02:  transmit_ready (data);
                     break;

         case 0x04:  receive_ready (data);
                     break;

/*       case 0x06:  line_status_int (data); (currently not used)
                     break;
*/
         }
      if (force_transmit_ready)
         transmit_ready (data);
}


void Localfunc tsk_sio_int (int irq)
{
   sioptr curr;

   for (curr = irq_array [irq]; curr != LNULL; curr = curr->next)
      sioint (curr);
}


/*-------------------------------------------------------------------------*/

local void Taskfunc v24_chain_remove (callchainptr chain)
{
   /* 
      We just assume here that all local data is in the same segment.
      This should be true for all compilers and memory models
      unless you define an extremely low data threshold.
   */
   v24_remove_all ();
}


int Globalfunc v24_define_port (int base, byte irq, byte vector)
{
#if (TSK_DYNAMIC)
   portptr portp;

   if (irq >= MAX_IRQ)
      return -1; 
   if (irq_procs [irq] == LNULL)
      return -1; 

   if ((portp = tsk_palloc (sizeof (port_data))) == LNULL)
      return -1;
   portp->pnum = ports;
   portp->base = base;
   portp->irq = irq;
   portp->vector = vector;
   portp->next = LNULL;
   portp->sio = LNULL;

   if (port_list == LNULL)
      port_list = portp;
   else
      port_last->next = portp;

   port_last = portp;
   ports++;

   return portp->pnum;

#else
   return -1;
#endif
}


local sioptr Staticfunc ret_error (sioptr sio)
{
   sio->port->sio = LNULL;
#if (TSK_DYNAMIC)
   if (sio->port->pnum >= STATIC_PORTS)
      tsk_pfree (sio);
#endif
   return LNULL; 
}


sioptr Globalfunc v24_install (int port, int init,
                        farptr rcvbuf, word rcvsize,
                        farptr xmitbuf, word xmitsize)
{
   sioptr sio;
   portptr portp;
   int pbase;
   intprocptr far *intptr;
   int i, inta;

#if (TSK_NAMEPAR)
   static char xname [] = "SIOnXMIT", rname [] = "SIOnRCV";

   xname [3] = rname [3] = (char)((port & 0x7f) + '0');
#endif

   if (port < 0 || !rcvsize || !xmitsize)
      return LNULL;

   portp = port_list;

   if (port & 0x80)     /* Relative port number */
      {
      port &= 0x7f;
      if (port > 4)
         return LNULL; 
      pbase = *((wordptr)(TMK_FP (0x40, port * 2)));
      if (!pbase)
         return LNULL;

      for (port = 0; port < ports; port++, portp = portp->next)
         if (portp->base == pbase)
            break;

      if (port >= ports)
         return LNULL;
      }
   else 
      {
      if (port > ports)
         return LNULL;
      for (i = 0; i < port; i++)
         portp = portp->next;
      }

   if (portp->sio != LNULL) /* Port already in use ? */
      return LNULL;

   if (port < STATIC_PORTS)
      portp->sio = sio = &sio_data [port];
   else
#if (TSK_DYNAMIC)
      if ((portp->sio = sio = tsk_palloc (sizeof (sio_datarec))) == LNULL)
         return LNULL;
#else
      return LNULL;
#endif

   pbase = sio->port_base = portp->base;
   sio->port = portp;

   /* Check if port accessible by modifying the modem control register */

   i = sio->cmodcontrol = sio->save_mcon = tsk_inp (pbase + modemcontrol);
   if (i & 0xe0)
      return ret_error (sio);
   tsk_nop ();
   tsk_outp (pbase + modemcontrol, 0xe0 | i);
   tsk_nop ();
   if (tsk_inp (pbase + modemcontrol) != (byte) i)
      return ret_error (sio);

   /* Port seems OK */

   if (create_pipe (&sio->xmit_pipe, xmitbuf, xmitsize TN(xname)) == LNULL)
      return ret_error (sio);

   if (create_wpipe (&sio->rcv_pipe, rcvbuf, rcvsize TN(rname)) == LNULL)
      {
      delete_pipe (&sio->xmit_pipe);
      return ret_error (sio);
      }

   sio->civect = portp->vector;
   sio->irqbit = (byte)(1 << (portp->irq & 0x07));

   sio->wait_xmit = sio->xmit_pending = 0;
   sio->overrun = 0;
   sio->flags = 0;
   sio->modem_flags = 0;
   sio->r_xoff = sio->t_xoff = 0;
   sio->rtsoff = 0;

   sio->clcontrol = sio->save_lcon = tsk_inp (pbase + linecontrol);
   tsk_nop ();
   sio->save_inten = tsk_inp (pbase + intenable);
   tsk_nop ();

   if (init)
      {
      sio->clcontrol = dflt_lcon;
      sio->cmodcontrol = dflt_modcon;
      }

   tsk_outp (pbase + linecontrol, sio->clcontrol | 0x80);
   tsk_nop ();
   sio->save_bd1 = tsk_inp (pbase + baudreg_dll);
   tsk_nop ();
   sio->save_bd2 = tsk_inp (pbase + baudreg_dlm);
   tsk_nop ();
   tsk_outp (pbase + linecontrol, sio->clcontrol);
   tsk_nop ();

   tsk_outp (pbase + intenable, 0);

   if (irq_array [portp->irq] == LNULL)
      {
      intptr = (intprocptr far *)TMK_FP (0, sio->civect * 4);
      tsk_cli ();
      sio->savvect = *intptr;
      *intptr = irq_procs [portp->irq];
      tsk_sti ();
      }

   if (init)
      {
      tsk_outp (pbase + linecontrol, dflt_lcon | 0x80);
      tsk_nop ();
      tsk_outp (pbase + baudreg_dll, dflt_baud);
      tsk_nop ();
      tsk_outp (pbase + baudreg_dlm, dflt_baud >> 8);
      tsk_nop ();
      tsk_outp (pbase + linecontrol, dflt_lcon);
      tsk_nop ();
      tsk_outp (pbase + modemcontrol, dflt_modcon);
      tsk_nop ();
      }
   else
      {
      i = tsk_inp (pbase + modemcontrol) | OUT2;
      tsk_nop ();
      tsk_outp (pbase + modemcontrol, i);
      tsk_nop ();
      }

   while (tsk_inp (pbase + linestatus) & rxreadybit)
      {
      tsk_nop ();
      tsk_inp (pbase + receivedata);
      tsk_nop ();
      }
   tsk_nop ();

   tsk_inp (pbase + linestatus);
   tsk_nop ();
   sio->modstat = tsk_inp (pbase + modemstatus);
   tsk_nop ();
   tsk_inp (pbase + intid);
   tsk_nop ();

   inta = (portp->irq > 7) ? inta11 : inta01;

   if (irq_array [portp->irq] == LNULL)
      {
      if (portp->irq > 7)
         {
         i = tsk_inp (inta01) & ~CHAIN_IRQBIT;
         tsk_nop ();
         tsk_outp (inta01, i);
         }

      sio->save_irq = (byte)((i = tsk_inp (inta)) & sio->irqbit);
      tsk_nop ();
      tsk_outp (inta, i & ~sio->irqbit);
      }
   else
      sio->save_irq = (irq_array [portp->irq])->save_irq;

   tsk_cli ();
   sio->next = irq_array [portp->irq];
   irq_array [portp->irq] = sio;
   tsk_sti ();

   if (remove_chain.func == LNULL)
      chain_removefunc (v24_chain_remove, &remove_chain, NULL);

   /* Enable interrupts with correction for possible loss of the 
      first tranmit interrupt on INS8250 and INS8250-B chips */
   for (;;)
       {
       if (tsk_inp (pbase + linestatus) & txreadybit)
           {
           break;
           }
       tsk_nop ();
       }
   tsk_nop ();
   tsk_cli ();
   tsk_outp (pbase + intenable, intdata);
   tsk_nop ();
   tsk_outp (pbase + intenable, intdata);
   tsk_sti ();

   return sio;
   }


void Globalfunc v24_remove (sioptr sio, int restore)
   {
   intprocptr far *intptr;
   int pbase, i, inta;
   portptr portp;
   sioptr curr, last;

   pbase = sio->port_base;
   portp = sio->port;

   last = LNULL;
   curr = irq_array [portp->irq];
   while (curr != sio && curr != LNULL)
      {
      last = curr;
      curr = curr->next;
      }
   if (curr == LNULL)
      return;

   tsk_outp (pbase + intenable, 0);
   tsk_cli ();
   if (last == LNULL)
      irq_array [portp->irq] = sio->next;
   else
      last->next = sio->next;
   tsk_sti ();

   inta = (portp->irq > 7) ? inta11 : inta01;

   if (restore)
      {
      tsk_outp (pbase + modemcontrol, sio->save_mcon);
      tsk_nop ();
      tsk_outp (pbase + linecontrol, sio->save_lcon | 0x80);
      tsk_nop ();
      tsk_outp (pbase + baudreg_dll, sio->save_bd1);
      tsk_nop ();
      tsk_outp (pbase + baudreg_dlm, sio->save_bd2);
      tsk_nop ();
      tsk_outp (pbase + linecontrol, sio->save_lcon);
      tsk_nop ();
      if (irq_array [portp->irq] == LNULL)
         {
         tsk_cli ();
         tsk_outp (pbase + intenable, sio->save_inten);
         i = tsk_inp (inta) & ~sio->irqbit;
         tsk_nop ();
         tsk_outp (inta, i | sio->save_irq);
         }
      }
   else if (irq_array [portp->irq] == LNULL)
      {
      tsk_cli ();
      i = tsk_inp (inta) | sio->irqbit;
      tsk_nop ();
      tsk_outp (inta, i);
      }

   if (irq_array [portp->irq] == LNULL)
      {
      tsk_cli ();
      intptr = (intprocptr far *)TMK_FP (0, sio->civect * 4);
      *intptr = sio->savvect;
      }
   tsk_sti ();

   portp->sio = LNULL;
   delete_pipe (&sio->xmit_pipe);
   delete_wpipe (&sio->rcv_pipe);

#if (TSK_DYNAMIC)
   if (portp->pnum >= STATIC_PORTS)
      tsk_pfree (sio);
#endif
   }


void Taskfunc v24_remove_all (void)
{
   int i;
   sioptr sio;

   unchain_removefunc (&remove_chain);

   for (i = 0; i < MAX_IRQ; i++)
      {
      while ((sio = irq_array [i]) != LNULL)
         v24_remove (sio, RESTORE_DEFAULT);
      }
}


/*-------------------------------------------------------------------------*/

/*
   void v24_change_rts (sioptr sio, int on)
*/

void Globalfunc v24_change_rts (sioptr sio, int on)
{
   sio->cmodcontrol = (byte)((sio->cmodcontrol & ~RTS) | ((on) ? RTS : 0));
   tsk_outp (sio->port_base + modemcontrol, sio->cmodcontrol);
}

/*
   void v24_change_dtr (sioptr sio, int on)
*/

void Globalfunc v24_change_dtr (sioptr sio, int on)
{
   sio->cmodcontrol = (byte)((sio->cmodcontrol & ~DTR) | ((on) ? DTR : 0));
   tsk_outp (sio->port_base + modemcontrol, sio->cmodcontrol);
}


/*
   void Globalfunc v24_change_baud (sioptr sio, int rate)
*/

void Globalfunc v24_change_baud (sioptr sio, long rate)
{
   int i;

   for (i = 0; baud_table [i]; i += 2)
      if (baud_table [i] == rate)
         break;
   if ((i = (int)baud_table [i + 1]) == 0)
      return;

   tsk_outp (sio->port_base + linecontrol, sio->clcontrol | (byte)0x80);
   tsk_nop ();
   tsk_outp (sio->port_base + baudreg_dll, (byte)i);
   tsk_nop ();
   tsk_outp (sio->port_base + baudreg_dlm, (byte)(i >> 8));
   tsk_nop ();
   tsk_outp (sio->port_base + linecontrol, sio->clcontrol);
}


void Globalfunc v24_change_parity (sioptr sio, int par)
{
   sio->clcontrol = (byte)((sio->clcontrol & 0xc7) | par);
   tsk_outp (sio->port_base + linecontrol, sio->clcontrol);
}


void Globalfunc v24_change_wordlength (sioptr sio, int len)
{
   int i;

   switch (len)
      {
      case 5:  i = 0x00; break;
      case 6:  i = 0x01; break;
      case 7:  i = 0x02; break;
      case 8:  i = 0x03; break;
      default: return;
      }
   sio->clcontrol = (byte)((sio->clcontrol & 0xfc) | i);
   tsk_outp (sio->port_base + linecontrol, sio->clcontrol);
}


void Globalfunc v24_change_stopbits (sioptr sio, int n)
{
   int i;

   switch (n)
      {
      case 1:  i = 0x00; break;
      case 2:  i = 0x04; break;
      default: return;
      }
   sio->clcontrol = (byte)((sio->clcontrol & 0xfb) | i);
   tsk_outp (sio->port_base + linecontrol, sio->clcontrol);
}


void Globalfunc v24_watch_modem (sioptr sio, byte flags)
{
   sio->modem_flags = (byte)(flags & (CTS | DSR | RI | CD));
}


void Globalfunc v24_protocol (sioptr sio, int prot, word offthresh, word onthresh)
{
   byte old;
   
   old = sio->flags;
   sio->flags = (byte)prot;
   if (prot)
      {
      if (!offthresh)
         offthresh = 10;
      sio->xoff_threshold = offthresh;
      if (onthresh <= offthresh)
         onthresh = offthresh + 10;
      sio->xon_threshold = onthresh;
      }

   if ((old & RTSCTS) != ((byte)prot & RTSCTS))
      {
      change_rts (sio, 1);
      sio->modem_flags = (byte)((sio->modem_flags & ~CTS) |
                         ((prot & RTSCTS) ? CTS : 0));
      }

   if (!(prot & XONXOFF))
      {
      if (sio->r_xoff)
         sio->r_xoff = -2;
      sio->t_xoff = 0;
      }

   if (!sio->xmit_pending)
      transmit_ready (sio);
}


/*-------------------------------------------------------------------------*/


int Globalfunc v24_send (sioptr sio, byte ch, dword timeout)
{
   int res;

   if ((res = write_pipe (&sio->xmit_pipe, ch, timeout)) < 0)
      return res;
   tsk_cli ();
   if (!sio->xmit_pending)
      transmit_ready (sio);
   tsk_sti ();
   return 0;
}


int Globalfunc v24_receive (sioptr sio, dword timeout)
{
   int res;
   
   if ((res = (int)read_wpipe (&sio->rcv_pipe, timeout)) < 0)
      return res;

   if (!sio->flags)
      return res;

   if (wpipe_free (&sio->rcv_pipe) > sio->xon_threshold)
      {
      tsk_cli ();
      if (sio->r_xoff)
         {
         sio->r_xoff = -2;
         if (!sio->xmit_pending)
            transmit_ready (sio);
         }
      tsk_sti ();

      if (sio->rtsoff)
         change_rts (sio, 1);
      }
   return res;
}


int Globalfunc v24_overrun (sioptr sio)
{
   int res;

   res = sio->overrun;
   sio->overrun = 0;
   return res;
}


int Globalfunc v24_check (sioptr sio)
{
   return check_wpipe (&sio->rcv_pipe);
}


int Globalfunc v24_modem_status (sioptr sio)
{
   return sio->modstat;
}


int Globalfunc v24_complete (sioptr sio)
{
   return (check_pipe (&sio->xmit_pipe) == -1);
}


int Globalfunc v24_wait_complete (sioptr sio, dword timeout)
{
   return wait_pipe_empty (&sio->xmit_pipe, timeout);
}


void Globalfunc v24_flush_receive (sioptr sio)
{
    flush_wpipe (&sio->rcv_pipe);
}


void Globalfunc v24_flush_transmit (sioptr sio)
{
    flush_pipe (&sio->xmit_pipe);
}

#endif

