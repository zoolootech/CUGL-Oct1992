#include <dos.h>
scroll_dn(int trow, int lcol,int brow, int rcol,int attr, int lines)
/* This will scroll the defined window down x number of lines:
   trow=upper left row
   lcol=upper left column
   brow=lower right row
   rcol=lower right col
   lines=number of lines to scroll
*/
{
   union REGS inregs;

   inregs.h.bh = attr;
   inregs.h.cl = lcol;
   inregs.h.ch = trow;
   inregs.h.dl = rcol;
   inregs.h.dh = brow;
   inregs.h.al = lines;
   inregs.h.ah = 7;                     /* do the backscroll */
   int86(0x10,&inregs,&inregs);
}
