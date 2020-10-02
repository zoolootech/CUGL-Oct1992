#include <dos.h>
scroll_up(int trow, int lcol,int brow, int rcol,int attr, int lines)
/* This will scroll the specified window up the specified number of lines.
   SEE SCROLL_DN FOR DESCP. IF ARGS PASSED
*/
{
   union REGS inregs;

   inregs.h.bh = attr;
   inregs.h.cl = lcol;
   inregs.h.ch = trow;
   inregs.h.dl = rcol;
   inregs.h.dh = brow;
   inregs.h.al = lines;
   inregs.h.ah = 6;                     /* do the backscroll */
   int86(0x10,&inregs,&inregs);
}
