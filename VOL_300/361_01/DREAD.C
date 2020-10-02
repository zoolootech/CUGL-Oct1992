
/* Dread.c --> Direct Video CRT Reads in Text Mode.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <stdlib.h>
#include <stdek.h>
#include <gadgets.h>

void Dread(int X, int Y, char *Text, int N)
{
    int Addr;

    Addr = Vaddr(X, Y);
    for (*Text = NULL; N-- && X++ < 80; *Text = NULL, Addr += 2)
        *Text++ = Vpeek(Addr) & 0xFF;
}

