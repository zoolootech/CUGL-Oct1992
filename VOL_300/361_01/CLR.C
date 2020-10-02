

/* Clr.c --> Using Scroll (Interrupt 0x10h, 6/7h) to Clear the CRT.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <stdek.h>
#include <gadgets.h>
#include <dos.h>

void ClrTo(int X, int Color)
/* Clear from the current cursor location thru X. */
{
    int xx, yy;
        
    Getxy(&xx, &yy); if (X < xx) return; Scroll(xx, yy, X, yy, Color, 0, 0);
}

void Scroll(int Left, int Top, int Right, int Bottom, int Color, int N, 
   int Flag)
{
   union REGS rg;

   rg.h.bh = Color; rg.h.cl = --Left; rg.h.ch = --Top; rg.h.dl = --Right;
   rg.h.dh = --Bottom; rg.h.al = N; rg.h.ah = 7 - !Flag; int86(0x10,&rg,&rg);
}

