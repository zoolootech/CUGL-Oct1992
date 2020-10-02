#include <dos.h>
void clrarea(int trow,int tcol,int lrow,int lcol,int wattr)
/* Clear a portion of the screen using the passed attribute
	trow = upper left row of area
	tcol = upper left col of area
	lrow = lower right row of area
	lcol = lower right col of area
*/
{
   union REGS inregs;

   inregs.h.bh = wattr;
   inregs.h.cl = tcol;
   inregs.h.ch = trow;
   inregs.h.dl = lcol;
   inregs.h.dh = lrow;
   inregs.h.al = 0;
   inregs.h.ah = 6;                     /* do the scroll */
   int86(0x10,&inregs,&inregs);
}
