/*
   --- Version 2.0 90-10-12 10:34 ---

   PRT.H - CTask - Printer driver routine definitions.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany
*/


#define AUTOFEED  0x02
#define INIT      0x04
#define SELECT    0x08

#define BUSY      0x80
#define ACK       0x40
#define PEND      0x20
#define SELIN     0x10
#define ERROR     0x08

extern int Globalfunc prt_install (int port, byte polling, 
                                   word prior, 
                                   farptr xmitbuf, word xmitsize);
extern void Globalfunc prt_remove (int port);
extern void Taskfunc prt_remove_all (void);
extern void Globalfunc prt_change_control (int port, byte control);
extern int Globalfunc prt_write (int port, byte ch, dword timeout);
extern int Globalfunc prt_status (int port);
extern int Globalfunc prt_complete (int port);
extern int Globalfunc prt_wait_complete (int port, dword timeout);
extern void Globalfunc prt_flush (int port);

