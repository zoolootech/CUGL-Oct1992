/*
   --- Version 2.0 90-10-12 10:34 ---

   TSKSUP.H - CTask optional support routine prototypes.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany
*/

/* Module tsksnap.c */

extern void Globalfunc screensnap (int rows);
extern void Globalfunc csnapshot (void);
extern void Globalfunc snapshot (FILE far *f);

/* Module conout.c */

extern void init_conout (void);
extern void end_conout (void);
extern int tprintf (char *format, ...);
extern int tputs (char *buf);
extern int tputch (int ch);


