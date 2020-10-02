/*
   --- Version 2.1 90-10-12 10:34 ---

   TSKPRF.H - CTask - Formatted output routines

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany
*/

extern int CGlobalfunc tsk_sprintf (farptr str, farptr fmt, ...);
extern int Globalfunc tsk_vsprintf (farptr str, farptr fmt, farptr arg);
extern void CGlobalfunc tsk_rprintf (farptr fmt, ...);
extern void Globalfunc tsk_vrprintf (farptr fmt, farptr arg);
extern void CGlobalfunc tsk_printf (farptr fmt, ...);
extern void Globalfunc tsk_vprintf (farptr fmt, farptr arg);
extern void CGlobalfunc tsk_fprintf (int handle, farptr fmt, ...);
extern void Globalfunc tsk_vfprintf (int handle, farptr fmt, farptr arg);

extern void Globalfunc tsk_putc (char c);
extern void Globalfunc tsk_puts (farptr str);
extern void Globalfunc tsk_rputc (char c);
extern void Globalfunc tsk_rputs (farptr str);

extern word Globalfunc tsk_set_dualdis (void);
extern word Globalfunc tsk_set_currdis (void);
extern void Globalfunc tsk_set_regen (int regenseg, int port, int rows, int cols);
extern void Globalfunc tsk_set_colour (int rows, int cols);
extern void Globalfunc tsk_set_mono (int rows, int cols);
extern void Globalfunc tsk_setpos (int row, int col);
extern void Globalfunc tsk_set_attr (int attr);
extern void Globalfunc tsk_set_clreol (int clr);

extern dword tsk_regen;
extern word tsk_regen_o;
extern word tsk_regen_s;
extern word tsk_disport;

