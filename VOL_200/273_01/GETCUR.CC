#include <dos.h>
get_cur(int *row, int *col)
/* return the current cursor location into row,col.
   If the cursor is hidden return value will be 1 else 0 will be returned.	
*/
{
		union REGS inregs;
        inregs.h.bh=0; inregs.h.ah=3;
        int86(0x10,&inregs,&inregs);
        *row=inregs.h.dh;
        *col=inregs.h.dl;
        if(inregs.h.ch & 0x20)
        	return(1);
        else
        	return(0);
}
