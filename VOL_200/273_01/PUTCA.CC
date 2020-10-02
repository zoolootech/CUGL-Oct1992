#include <dos.h>
put_ca(char ch, int attr, int count)
/* This will put the specified charater and attribute on the screen at
   the current cursor location.
*/
{
		union REGS inregs;
        inregs.h.bh=0;
        inregs.h.bl=attr;
        inregs.h.al=ch;
        inregs.x.cx=count;
        inregs.h.ah=9;
        int86(0x10,&inregs,&inregs);
}
