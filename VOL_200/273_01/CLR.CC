#include <dos.h>
void clr(wattr)
/* CLear the screen using the attribute passed */
{
   union REGS inregs;

   inregs.h.bh = wattr;
   inregs.h.cl = 0;
   inregs.h.ch = 0;
   inregs.h.dl = 79;
   inregs.h.dh = 24;
   inregs.h.al = 0;
   inregs.h.ah = 6;			/* do the scroll */
   int86(0x10,&inregs,&inregs);
   locate(0,0);
}
