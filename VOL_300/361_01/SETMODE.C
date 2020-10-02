
/* Taken from CUG-273 */

#include <stdek.h>
#include <gadgets.h>
#include <dos.h>

void SetVideoMode(int Mode)
{
    union REGS rg;
    
    rg.h.ah = 0x00; rg.h.al = Mode; int86(0x10, &rg, &rg);
}

