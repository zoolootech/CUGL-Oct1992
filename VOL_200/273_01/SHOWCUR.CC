#include <dos.h>
show_cur(int size)
/* This will make the cursor visiable.
   size=1 = cursor uses scan lines 6-7.
   size=9 = cursoe uses scan lines 0-7.
*/
{
		union REGS inregs;
        inregs.h.ah=1;
        inregs.h.cl=7;
        if(size==9) inregs.h.ch=0;
        else inregs.h.ch=6;
        int86(0x10,&inregs,&inregs);
}
