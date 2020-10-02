/*
HEADER:		;
TITLE:		Bios function to set height of video cursor;
VERSION:	1.0;

DESCRIPTION:	This function generates an interrupt 16 to set the type of
		cursor and any that is displayed on the video screen;

KEYWORDS:	Bios, interrupt, cursor settings, video functions;
SYSTEM:		MSDOS;
FILENAME:       CURSOR_TYPE;
WARNINGS:	None;

AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C;
*/
/***************************************************************************
 *   Function to set the type of cursor displayed on screen.  If ch = 32   *
 *   and cl = 0 then the cursor will be invisible.  Use ch = 6 and cl = 7  *
 *   for normal cursor.  Ch = starting scan line, cl = ending scan line.   *
 ***************************************************************************/

void Cursor_type(int ch, int cl)
{
#include <dos.h>
#define VIDEO  0x10
     union REGS reg;
     reg.h.ah = 1;
     reg.h.ch = ch;
     reg.h.cl = cl;
     int86(VIDEO,&reg,&reg);
}