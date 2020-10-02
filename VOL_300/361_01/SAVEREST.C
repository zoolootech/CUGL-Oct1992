
/*SaveRest.c ---> Save/Restore a Rectangular Area of CRT Text.
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

void RestoreBox(int Left, int Top, int Right, int Bottom, char *FootPrint)
{
    int X, Y, Addr, *tp1;

    for (Y = Top, tp1 = (int *) FootPrint; Y <= Bottom; Y++) {
       Addr = Vaddr(Left, Y);
       for (X = Left; X++ <= Right; Addr += 2) Vpoke(Addr, *tp1++);
    }
}

void SaveBox(int Left, int Top, int Right, int Bottom, char *FootPrint)
{
    int X, Y, Addr, *tp1;

    for (Y = Top, tp1 = (int *) FootPrint; Y <= Bottom; Y++) {
       Addr = Vaddr(Left, Y);
       for (X = Left; X++ <= Right; Addr += 2) *tp1++ = Vpeek(Addr);
    }
}

