
/* Dwrite.c --> Direct video Writes.
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

void DputChr(char ch, int Color)
{
    int xx, yy;

    Getxy(&xx, &yy); Vpoke(Vaddr(xx,yy), ch + (Color << 8)); CursorFwd(1);
}

void Dwrite(int X, int Y, int Color, char *Text)
{
    int  addr;

    Color = Color << 8; Gotoxy(X, Y); CursorFwd(strlen(Text));
    for (addr = Vaddr(X, Y); *Text; addr += 2)
       Vpoke(addr, (*Text++ & 0xFF) + Color);
}

void DwriteEnd(int X, int Y, int Color, char *Text, int N)
{
    int  addr;

    Color = Color << 8; Gotoxy(X, Y); CursorFwd(strlen(Text));
    for (addr = Vaddr(X, Y); *Text; addr += 2, --N)
       Vpoke(addr, (*Text++ | Color));
    for (Color |= ' '; N--; addr += 2) Vpoke(addr, Color);
}

void SetAttr(int X, int Y, int Color)
{
    int Addr;
  
    Addr = Vaddr(X, Y); 
    Vpoke(Vaddr(X, Y), (Vpeek(Addr) & 0xFF) | (Color << 8));
}

