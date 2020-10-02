/*   DISK CATALOG : HEADERS.DOC
	
C Users Group Volume Number 125 - VIDEO TERMINAL LIRARIES
Cataloged By Ben Rockefeller 5/15/85    

*/

/*           HEADER: CUG125.09; 

	      TITLE: VIDEO TERMINAL LIBRARIES;
            VERSION: 1.0;
       	       DATE: 5/15/85;
        DESCRIPTION: "Library of video terminal routines
		      to use control codes for the Osborne 1";
	   KEYWORDS: Osborne 1, video terminal routines;
             SYSTEM: Osborne 1, CP/M;
           FILENAME: LIBO1.C;
	   WARNINGS: "Runs only on Osborne 1";
	   SEE-ALSO: LIBO1.NRO;
            AUTHORS: Stephen L. Browning;
          COMPILERS: BDC C;
*/


/*
*	Library of routines to utilize
*	control codes for the Osborne 1.
*
*	Stephen L. Browning
*	5723 North Parker Avenue
*	Indianapolis, Indiana 46220
*/

#include "crt.h"

#define ESC	033

/*
*	Home the cursor and clear the screen
*/

clrscrn()
{
	putchar('\032');
}

/*
*	Erase entire line
*/

eralin(_y)
int _y;
{
	putchar(ESC);
	putchar('=');
	putchar(23-_y+' ');
	putchar(' ');
	putchar(ESC);
	putchar('T');
}


/*
*	Erase from cursor to end of line
*/

erteol()
{
	putchar(ESC);
	putchar('T');
}


/*
*	Set terminal in graphics mode
*/

graphics()
{
	putchar(ESC);
	putchar('g');
}


/*
*	Move cursor to home position
*/

homcur()
{
	putchar('\036');
}


/*
*	Move cursor to one of eight adjacent
*	locations
*/

movcur(_dir)
int _dir;
{
	switch (_dir) {
	case N:
		putchar('\013');
		break;
	case NE:
		putchar('\013');
		putchar('\014');
		break;
	case E:
		putchar('\014');
		break;
	case SE:
		putchar('\012');
		putchar('\014');
		break;
	case S:
		putchar('\012');
		break;
	case SW:
		putchar('\012');
		putchar('\010');
		break;
	case W:
		putchar('\010');
		break;
	case NW:
		putchar('\013');
		putchar('\010');
		break;
	}
}

/*
*	Set terminal to its default
*	power up mode.
*/

normal()
{
	putchar(ESC);
	putchar('G');
}


/*
*	Set cursor to coordinate value.
*	0,0 is lower left corner of display
*	with "window" set to upper left
*	of the video ram.
*/

setcur(_x,_y)
int _x, _y;
{
	putchar(ESC);
	putchar('=');
	putchar(23 - _y + ' ');
	putchar(_x + ' ');
}
