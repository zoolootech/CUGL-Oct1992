
/* Cursor.c --> A Collection of Text Cursor Manipulation Tools
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

void CursorBwd(int N)
/* Move the cursor N cols to the left */
{
    int xx, yy;

    Getxy(&xx, &yy);
    if ((xx -= (N % 80)) < 1) { xx -= 80; if (--yy < 1) yy = 25; }
    Gotoxy(xx, yy);
}

void CursorDn(int N)
/* Move the down N rows & wrap to top if the cursor is on the last row */
{
    int xx, yy;
    
    Getxy(&xx, &yy); if ((yy += (N % 25)) > 25) yy -= 25; Gotoxy(xx, yy);
}

void CursorFwd(int N)
/* Move the cursor N cols to the right */
{
    int xx, yy;

    Getxy(&xx, &yy);
    if ((xx += (N % 80)) > 80) { xx = 1; if (++yy > 25) yy = 1; }
    Gotoxy(xx, yy);
}

void CursorNL(int N) 
/* Move the cursor to the beginning of the next row  */
{
    int xx, yy;
    
    Getxy(&xx, &yy); if ((yy += (N % 25)) > 25) yy = 1; Gotoxy(0, yy);
}

void CursorUP(int N)
/* Move the cursor N rows up */
{
    int xx, yy;

    Getxy(&xx, &yy); if ((yy -= (N % 25)) < 1) yy = 25; Gotoxy(xx, yy);
}

int Getxy(int *X, int *Y)
{
    union REGS rg;

    rg.h.bh = 0; rg.h.ah = 3; int86(0x10, &rg, &rg);
    *X = rg.h.dl + 1; *Y = rg.h.dh + 1;
    return !(rg.h.ch & 0x20);           /* True if Cd_Cursor */
}

void Gotoxy(X, Y)
/* RePosition cursor to (X, Y) */
{
    union REGS rg;

    rg.h.ah = 2; rg.h.dh = --Y; rg.h.dl = --X; rg.h.bh = 0;
    int86(0x10, &rg, &rg);
}

int HideCursor(void)
/* Turn the cursor off */
{
    int Visible;
    union REGS rg;

    rg.h.bh = 0;     rg.h.ah = 3; int86(0x10, &rg, &rg);
    if ((Visible = !(rg.h.ch & 0x20)) != 0) {
       rg.h.ch |= 0x20; rg.h.ah = 1; int86(0x10, &rg, &rg); }
    return Visible;
}

void ShowCursor(int Fat)
{
    static int Virgin = 0;
    union REGS rg;

    rg.h.bh = 0; rg.h.ah = 3; int86(0x10, &rg, &rg);    
    if (!Virgin) Virgin = rg.h.ch;
    rg.h.ah = 1; rg.h.ch = 0; if (!Fat) rg.h.ch = Virgin;
    rg.h.ch &= 0x1F; int86(0x10,&rg,&rg);
}

