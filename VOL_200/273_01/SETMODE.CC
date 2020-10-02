#include <dos.h>
set_mode(int mode)
/* This will set the video mode to the mode passed */
{
		union REGS inregs;
        inregs.h.ah=0x00;
        inregs.h.al=mode;
        int86(0x10,&inregs,&inregs);
}
