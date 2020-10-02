/*
HEADER:		;
TITLE:		Border color setting function;
VERSION:	1.0;

DESCRIPTION:	Function to set the border colors on IBM pcs and compatibles;

KEYWORDS:	Video functions, interrupts;
SYSTEM:		MSDOS;
FILENAME:	Border;
WARNINGS:	None;

SEE-ALSO:	Cursor_type;
AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C;
*/
/**************************************************************************/
/*  Border() will set the border color on IBM pcs and compatibles, however*/
/*  it will not work on ps/2s equipped with MCGA adapters since the border*/
/*  can only be black for those types.                                    */
/**************************************************************************/

#include<dos.h>

void Border(int color)
{
     union REGS regs;
     regs.h.ah = 11; regs.x.bx = color;
     int86(16,&regs,&regs);
}