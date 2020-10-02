#include <dos.h>
get_ca(char *ch, char *scan) 
/* Read a keystroke.  ch=character code or zero if extended code
					scan=extended code or scan code
*/
{
		union REGS inregs;
        inregs.h.ah=0;
        int86(0x16,&inregs,&inregs);
        *ch=inregs.h.al;
        *scan=inregs.h.ah;
}
