#include <dos.h>
get_xa()
/* Read a keystroke.
   An int will be returned as follows:
	  return value will be ascii code of key pressed or
			 value will be scan code of key press + 256 if the key press
			 was an extended key.  i.e. function key etc.

  The definitions of the extended keys are in the TCUTIL.H file.
*/
{
		int ret;
		union REGS inregs, outregs;
        inregs.h.ah=0;
		int86(0x16,&inregs,&outregs);
		if(outregs.h.al == 0x00) return(outregs.h.ah + 256);
		return(outregs.h.al);
}
