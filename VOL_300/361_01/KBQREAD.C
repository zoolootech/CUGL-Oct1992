
/* Kbqread.c ---> A TSR compliant getch() w/ Keys.h Reporting.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <dos.h>

int Kbq_read(void)
{
    int c = 0;
    union REGS rg;

    do {
       rg.h.ah  = 1; int86(0x16, &rg, &rg);
       if (rg.x.flags & 0x40) { int86(0x28, &rg, &rg); continue; }
       rg.h.ah = 0; int86(0x16, &rg, &rg);
       if (rg.h.al == 0) c = rg.h.ah | 128; else c = rg.h.al;
    } while (c == 0);
    return(c);
}

