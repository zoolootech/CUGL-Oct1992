#include <dos.h>
write_tty(char *ch)
/* This will put the character string pointer to by *ch on the screen
   at the current cursor location using attribute attr.
*/
{
union REGS inregs;
        while(*ch) {
			    inregs.h.bh=0;
			    inregs.h.bl=0;
			    inregs.h.al=*ch;
			    inregs.h.ah=0x0e;
			    int86(0x10,&inregs,&inregs);
			    ch++;
        }
	   inregs.h.bh=0;
	   inregs.h.bl=0;
	   inregs.h.al=0x0d;
	   inregs.h.ah=0x0e;
	   int86(0x10,&inregs,&inregs);

	   inregs.h.bh=0;
	   inregs.h.bl=0;
	   inregs.h.al=0x0a;
	   inregs.h.ah=0x0e;
	   int86(0x10,&inregs,&inregs);
}


