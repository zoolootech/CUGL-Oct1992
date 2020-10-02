#include <dos.h>
void clrbox(int trow,int tcol,int lrow,int lcol,int wattr)
/* Clear the area inside the box built by the BOX function to the
   specified attribute
*/
{
   union REGS inregs;

   inregs.h.bh = wattr;
   inregs.h.cl = tcol+1;
   inregs.h.ch = trow+1;
   inregs.h.dl = lcol-1;
   inregs.h.dh = lrow-1;
   inregs.h.al = 0;
   inregs.h.ah = 6;			/* do the scroll */
   int86(0x10,&inregs,&inregs);
}
