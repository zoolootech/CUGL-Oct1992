
/* These Interfaces to the Mouse Driver Interrupt 0x33h will function
 * in both Text and Graphics Modes.
 *
 * Note: You MUST Link w/ Graphics.Lib ON even for text-only use.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 16 September 91/EK
 */

#include <dos.h>
#include <graphics.h>
#include <gadgets.h>
#include <graphics.h>
#include <mouse.h>
#include <stdek.h>

static _Mouse = 0;

static int Handles(int N);

static int Handles(int N)
{
    int i;

    if (N < 0)                  /* Button Status to Character */
       return "Åº¹¶¸ª"[7+N] & 0xFF;
    else {                      /*      Character to Button Handle */
       for (i = 0; ("ª¶"[i] & 0xFF) != N && i++ < 2; );
       return i;
    }
}
       
void Mouse(int *A, int *B, int *C, int *D)
{
    union REGS rg;

    rg.x.ax = *A; rg.x.bx = *B; rg.x.cx = *C; rg.x.dx = *D;
    int86(0x33, &rg, &rg);
    *A = rg.x.ax; *B = rg.x.bx; *C = rg.x.cx; *D = rg.x.dx;
}

int MouseAtxy(int *X, int *Y)
{
    union REGS rg;

    if (!MouseOK()) return 0;
    rg.x.ax = 3; int86(0x33, &rg, &rg); *X = rg.x.cx; *Y = rg.x.dx;
    if (IsTextMode()) { *X = (*X) / 8 + 1; *Y = (*Y) / 8 + 1; }
    else if (getmaxx() IS 319) (*X) /= 2;
    return (rg.x.bx) ? Handles(-rg.x.bx) : 0;
}

int MouseClicked(int Button)
{
    int i, n = 0;
    union REGS rg;

    if (! _Mouse) return 0; i = Handles(Button);
    do { int86(0x28, &rg, &rg); rg.x.ax = 3; int86(0x33, &rg, &rg);
    }  while (rg.x.bx);
    if (i < 3) {
       rg.x.ax = 6; rg.x.bx = i; int86(0x33, &rg, &rg); n = (rg.x.bx > 0);
    } else while (i--) {
       n *= 2; rg.x.ax = 6; rg.x.bx = i; int86(0x33, &rg, &rg);
       n -= (rg.x.bx > 0);
    }
    return (n < 0) ? Handles(n) : n;
}

int MouseClickedxy(int Button, int *X, int *Y)
{
    union REGS rg;
    
    if (!MouseOK() || (rg.x.bx = Handles(Button)) IS 3) return 0;
    rg.x.ax = 6; int86(0x33, &rg, &rg);
    *X = rg.x.cx; *Y = rg.x.dx;
    if (IsTextMode()) { *X = (*X) / 8 + 1; *Y = (*Y) / 8 + 1; }
    else if (getmaxx() IS 319) (*X) /= 2;
    return (rg.x.bx) ? Handles(-rg.x.bx) : 0;
}

void MouseDeltaxy(int *X, int *Y)
{
    int Dx, Dy;
    union REGS rg;

    if (!MouseOK()) { *X = *Y = 0; return; }
    rg.x.ax = 0x1B; int86(0x33, &rg, &rg); Dx = rg.x.bx; Dy = rg.x.cx;
    rg.x.ax = 0x0B; int86(0x33, &rg, &rg); 
    *X = (rg.x.cx * Dx) / 8; *Y = (rg.x.dx * Dy) / 8; 
    if (IsTextMode()) { *X = (*X) / 8 + 1; *Y = (*Y) / 8 + 1; }
    else if (getmaxx() IS 319) (*X) /= 2;
}

int MouseOK(void) { return _Mouse; }

void MousePageSet(int Which1)
{
      union REGS rg;
      
      rg.x.ax = 0x1D; rg.x.bx = Which1; int86(0x33, &rg, &rg);
}

int MouseReSet(void)
/*  Resets the mouse.  Returns # of Buttons if Present or 0 if No Mouse. */
{
    union REGS rg;

    rg.x.ax = 0; int86(0x33, &rg, &rg); if (rg.x.ax IS 0) return 0;
    _Mouse = 1; MouseSwitch(-1);
    return (rg.x.bx IS 0) ? 2 : (rg.x.bx IS 3) ? 3 : 1;
}

void MouseSpeed(int Dx, int Dy, int Zoom)
{
      union REGS rg;
      
      rg.x.ax = 0x1A; rg.x.bx = Dx; rg.x.cx = Dy; rg.x.dx = Zoom;
      int86(0x33, &rg, &rg);
}

int MouseSwitch(int Flag)
{
    union REGS rg;
    static int Cd_mouse = 1;

    if (Flag IS -1) Cd_mouse = 0;
    if (!Flag && !Cd_mouse) return 0;
    if (Flag && Cd_mouse) return 1;
    rg.x.ax = 1 + !Flag; int86(0x33, &rg, &rg); Cd_mouse = !Cd_mouse;
    return !Cd_mouse;
}

void MouseToxy(int X, int Y)
{
    union REGS rg;
    
    if (!MouseOK()) { if (IsTextMode()) Gotoxy(X, Y); return; }
    if (IsTextMode()) { rg.x.cx = --X * 8; rg.x.dx = --Y * 8; }
    else {
       if (getmaxx() == 319) X *= 2;    /*      Adjust for virtual coordinates */
       rg.x.cx = X; rg.x.dx = Y; 
    }
    rg.x.ax = 4; int86(0x33, &rg, &rg);
}

int MouseTrap(int Left, int Top, int Right, int Bottom)
{
    int X, Y;
    
    if (MouseOK()) MouseAtxy(&X, &Y); else return 0;
    return((X >= Left && X <= Right && Y >= Top && Y <= Bottom) ? 1 : 0);
}

int MouseWait4User(void)
{
    int c, X, Y;

    if (IsTextMode()) { Getxy(&X, &Y); MouseToxy(X, Y); }
    for (c = 0; !c; )
       if (!(c = Kbq_poll()) && !(c = MouseClicked(BTN_ANY)) &&
          IsTextMode()) { MouseAtxy(&X, &Y); Gotoxy(X, Y); }
    return c;
}

