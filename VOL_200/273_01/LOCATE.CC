#include <dos.h>
locate(int row, int col)
/* Position the cursor at row,col */
{
		union REGS inregs;
        inregs.h.ah=2; inregs.h.dh=row; inregs.h.dl=col;
        inregs.h.bh=0;
        int86(0x10,&inregs,&inregs);
}
