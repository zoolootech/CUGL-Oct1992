#include <dos.h>
clr_eol(int last_col)
/* This will clear from the current cursor location the the end of the line.
   The last column of the line is specified as last_col
*/

 {
	union REGS inregs;
	int cur_row, cur_col;
    inregs.h.bh=0; inregs.h.ah=3;					/*get cursor position*/
    int86(0x10,&inregs,&inregs);
    cur_row=inregs.h.dh;
    cur_col=inregs.h.dl;
    if(last_col < cur_col) return(1);
    inregs.h.ah=8; inregs.h.bh=0;					/*get current attr*/
    int86(0x10,&inregs,&inregs);
    inregs.h.bh=inregs.h.ah;						/*clear to endof line*/
	inregs.h.ah=6; inregs.h.al=0;
	inregs.h.ch=cur_row;
	inregs.h.cl=cur_col;
	inregs.h.dh=cur_row;
	inregs.h.dl=last_col;
	int86(0x10,&inregs,&inregs);
	return(0);
}
