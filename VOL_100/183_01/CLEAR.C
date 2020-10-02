#include <dos.h>
#include <stdio.h>

main()
{

  struct REGS clscr;

    clscr.x.ax = 0x0600;
    clscr.x.bx = 0x0700;
    clscr.x.cx = 0x0000;
    clscr.x.dx = 0x184F;

    int86(0x10, &clscr);
}
