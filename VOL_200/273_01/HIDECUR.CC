#include <dos.h>
hide_cur()
/* Turn the cursor off */
{
		union REGS inregs;
        inregs.h.ah=1;
        inregs.h.cl=7;
        inregs.h.ch=0x26;
        int86(0x10,&inregs,&inregs);
}
