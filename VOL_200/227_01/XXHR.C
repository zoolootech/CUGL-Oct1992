/*
 *	x x h r . c
 *	-----------
 *	Low-level driver for the Else PC - XXHR, DXHR and XXHR vector
 *	graphics board.
 *	Has only very little functionality and should be enhanced to use
 *	all features of this boards, if you decide to use it permanently.
 *
 *	Written by	Rainer Gerhards
 *			Petronellastr. 6
 *			D-5112 Baesweiler
 *			West Germany
 *			Phone (49) - 2401 - 1601
 */
#include <stdio.h>
#include <dos.h>

#define NOLLHINC

#include "xxhr.h"
#include "graphics.h"

#define	COLORS		0x10		/* set fore- and background colr*/
#define	AMOVE		0x80		/* absolute move		*/
#define	DOT		0xa2		/* write dot (pixel)		*/
#define	RDOT		0xa3		/* read dot (pixel)		*/
#define	FSCRN		0xc0		/* fill screen			*/

void	initgraf(dummy)
int dummy;
/**
* name          initgraf
*
* synopsis      initgraf(dummy);
*               int dummy           dummy for compatibility reasons
*
* description   This routine initializes the graphics board.
**/
{
union REGS regs;

regs.h.ah = FSCRN;
regs.h.dl = BLACK;
int86(XXHRINT, &regs, &regs);
}


getpixel(x, y)
int x, y;
/**
* name          getpixel
*
* synopsis      getpixel(x, y);
*               int x, y    pixel coordinate
*
* description   This routine reads the color of the specified pixel.
*               The color is return as function return value.
**/
{
union REGS regs;

regs.h.ah = AMOVE;
regs.x.bx = x;
regs.x.cx = y;
int86(XXHRINT, &regs, &regs);
regs.h.ah = RDOT;
int86(0x10, &regs, &regs);
return((int) regs.h.al);
}


void	setpixel(x, y, color)
int x, y;
int color;
/**
* name          setpixel
*
* synopsis      setpixel(x, y, color);
*               int x, y	pixel coordinate
*		int color	requested pixel color
*
* description   This routine writes a pixel of "color" at coordinate "x", "y".
**/
{
union REGS regs;
static int oldcolor = -1;		/* previous set color		*/

regs.h.ah = AMOVE;
regs.x.bx = x;
regs.x.cx = y;
int86(XXHRINT, &regs, &regs);
if(color != oldcolor)
	{
	regs.h.ah = COLORS;
	regs.h.dl = color;
	regs.h.dh = color;
	int86(0x10, &regs, &regs);
	}
regs.h.ah = DOT;
int86(0x10, &regs, &regs);
}
