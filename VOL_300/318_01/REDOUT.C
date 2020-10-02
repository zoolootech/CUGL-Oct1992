/*
	RED terminal output module -- Customized for IBM PC.
	Source:  redout.c
	Version: September 8, 1986; January 18, 1990

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.

	See red.h for a disclaimer of warranties and other information.
*/
#include "red.h"

#ifdef TURBOC
#include <graphics.h>
#include <conio.h>
#endif

#define USE_ASM

#ifndef USE_ASM
#ifndef TURBOC
#ifndef DESMET
#define PORTABLE
#endif
#endif
#endif

/*
	Initialize the globals which describe the terminal.
*/
void
outinit(void)
{

	outxy(0,0);

#ifdef USE_ASM
	hasdn  = FALSE;
	hasup  = FALSE;
	hasins = TRUE;
	hasdel = TRUE;
	hasint = TRUE;
	hascol = TRUE;
	haswrap = FALSE;
#else
#ifdef PORTABLE
	hasdn   = FALSE;
	hasup   = FALSE;
	hasins  = FALSE;
	hasdel  = FALSE;
	hasint  = TRUE;
	hascol  = TRUE;
	haswrap = TRUE;
	hasword = FALSE;
#else
	hasdn   = FALSE;
	hasup   = FALSE;
	hasins  = TRUE;
	hasdel  = TRUE;
	hasint  = TRUE;
	hascol  = TRUE;
	haswrap = TRUE;
	hasword = FALSE;

#endif /* PORTABLE */
#endif /* USE_ASM  */

#ifndef USE_ASM
#ifdef DESMET
	scr_setup();	/* 9/8/86 */
	#ifdef PCJR
		scr_mode(2);
	#endif
#endif

#ifdef TURBOC
	/* 12/12/89 */

	directvideo = 0;	/* Use Bios Calls, not video ram. */
	textmode(C80);		/* Use previous text mode.	  */

#endif /* TURBOC */
#endif /* USE_ASM  */

}

/*
	The rest of the file is used only if the file redouta.asm 
	is not used.
*/

#ifndef USE_ASM

/*
	Output one printable character to the screen.
*/
int
outchar(char c)
{
	SL_DISABLE();

	outch(c);
	outx++;
	return c;
}

/*
	Erase the entire screen.
	Make sure the rightmost column is erased.
*/
void
outclr(void)
{
	int i;

	SL_DISABLE();

#ifdef DESMET
	scr_clr();
#endif

#ifdef TURBOC
	clrscr();
#endif

#ifdef PORTABLE
	for (i = 0; i < SCRNL; i++) {
		outxy(0, i);
		outdelln();
	}
	outxy(0,0);
#endif

}

/*
	Hardware delete line.
*/
void
outdel(void)
{
	SL_DISABLE();

#ifdef DESMET
	scr_scrup(1,outy,0,24,79);
#endif

#ifdef TURBOC
	delline();
#endif

}

/*
	Delete the line on which the cursor rests.
	Leave the cursor at the left margin.
*/
void
outdelln(void)
{
	SL_DISABLE();

	outxy(0,outy);
	outdeol();
}

/*
	Delete to end of line.
	Assume the last column is blank.
*/
void
outdeol(void)
{
	SL_DISABLE();

#ifdef DESMET
	scr_clrl();
#endif

#ifdef TURBOC
	clreol();
#endif

}

/*
	Hardware insert line.
*/
void
outins(void)
{
	SL_DISABLE();

#ifdef DESMET
	scr_scrdn(1,outy,0,24,79);	
#endif

#ifdef TURBOC
	insline();
#endif

}

/*
	Scroll screen down.
	Assume the cursor is on the top line.
*/
void
outsdn(void)
{
	SL_DISABLE();

#ifdef PORTABLE
	outxy(0,0);
#endif

}

/*
	Scroll the screen up.
	Assume the cursor is on the bottom line.
*/
void
outsup(void)
{
	SL_DISABLE();

#ifdef PORTABLE
	/* auto scroll */
	outxy(0,SCRNL1);
	syscout(10);
#endif

}

/*
	Position cursor to position x,y on screen.
	0,0 is the top left corner.
*/
void
outxy(int x, int y)
{
	SL_DISABLE();

	outx = x;
	outy = y;

#ifdef DESMET
	scr_rowcol(y,x);
#endif

#ifdef TURBOC
	gotoxy(x+1, y+1);
#endif

}

#endif /* USE_ASM */
