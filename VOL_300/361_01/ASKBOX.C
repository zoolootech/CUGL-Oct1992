
/* AskBox.c --> A "Stacked Window" Simulation via Direct Video Writes.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 16 September 91/EK
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdek.h>
#include <gadgets.h>

void Box(int Left, int Top, int Right, int Bottom, int Fill, int Color)
/* Draw a single line box using Color & Clear it to Fill. */
{
    int i;
    char Lid[81];

    Dwrite(Left, Top, Color, "Ú"); Dwrite(Right, Top, Color, "¿");
    Dwrite(Left, Bottom, Color, "À"); Dwrite(Right, Bottom, Color, "Ù");
    *Lid = NULL; PadRight(Lid, 'Ä', Right - Left - 1);
    Dwrite(Left+1, Top, Color, Lid); Dwrite(Left+1, Bottom, Color, Lid);
    for (i= Top + 1; i < Bottom; i++) {
        Dwrite(Left, i, Color, "³"); Dwrite(Right,      i, Color, "³"); }
    ClrBox(++Left, ++Top, --Right, --Bottom, Fill);
}

void CcolorSet(int X, int Y, int Color, int N)
{ while (N--) if (Y < 26) SetAttr(X, Y++, Color); }

void MkAskBox(char *FootPrint, int X, int Y, int Width, int Tall, char Shadow,
    char *TopTitle, char *FootTitle, char *Prompt, BYTE Color)
{
    int Xc, Xsh, Xg, Y1, X1;

 /* Handle Shadow */
    X1 = X + Width - 1; Y1 = Y + Tall - 1; Xg = Xsh = X;
    if (Y+Tall > 25);
    else if (Shadow IS 'L' && X > 1)   { Xsh--; Xg--; Y1++; }
    else if (Shadow IS 'R' && X < 80)  { Xsh++; X1++; Y1++; }

 /* Build Window w/ Titles */
    HideCursor(); if (FootPrint != NULL) SaveBox(Xg, Y, X1, Y1, FootPrint);
    Box(X, Y, (X1 = X+Width-1), (Y1 = Y+Tall-1), Color, Color);
    if (Shadow) MkShadow(X, Y, X1, Y1, Shadow);
    if (*TopTitle) Dwrite(X+(Width-strlen(TopTitle)+1)/2, Y, Color, TopTitle);
    if (*FootTitle)
       Dwrite (X+(Width-strlen(FootTitle)+1)/2, Y1, Color, FootTitle);
    if (*Prompt) Dwrite(X+1, Y+1, Color, Prompt); else Gotoxy(X+1, Y+1);
    ShowCursor(0);
}

void MkShadow(int Left, int Top, int Right, int Bottom, char Which1)
/* Create the illusion of a shadow under the specified window */
{
    int X, Y, Z;

    if (++Bottom > 25) return;
    if (Which1 IS 'L' && --Right && --Left < 1) return;
    if (Which1 != 'L' && ++Left && ++Right > 80) return;
    if ((Z = Bottom - Top++) < 1) return;
    if (Which1 IS 'L') CcolorSet(Left, Top, 0, Z);
    else CcolorSet(Right, Top, 0, Z);
    if ((Z = Right - Left + 1) > 0) RcolorSet(Left, Bottom, 0, Z);
}

void RcolorSet(int X, int Y, int Color, int N)
{
    int addr;

    Color <<= 8;
    for (addr = Vaddr(X, Y); N--; addr += 2)
        Vpoke(addr, (Vpeek(addr) & 0xFF) + Color);
}

void ZapAskBox(char *FootPrint,int X,int Y,int Width,int Tall,char Shadow)
{
    int X1, Y1;

 /* Handle Shadow */
    X1 = X + Width - 1; Y1 = Y + Tall - 1;
    if (Y+Tall > 25);
    else if (Shadow IS 'L' && X > 1)   { X--;  Y1++; }
    else if (Shadow IS 'R' && X < 80)  { X1++; Y1++; }

 /* Zap Window */
    HideCursor(); RestoreBox(X, Y, X1, Y1, FootPrint);
}

