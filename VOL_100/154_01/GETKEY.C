/* getkey.c:	single, unbuffered keypress via BIOS */

#include <dos.h>

char getkey()
{
	struct reg regs;

	regs.r_ax = 0;
	intcall(&regs,&regs,0x16);
	return((char) regs.r_ax & 0x00ff);  /* answer in AL */
}
