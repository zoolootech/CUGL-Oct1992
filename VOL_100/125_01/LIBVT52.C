/*   DISK CATALOG : HEADERS.DOC
	
C Users Group Volume Number 125 - VIDEO TERMINAL LIRARIES
Cataloged By Ben Rockefeller 5/15/85    

*/

/*           HEADER: CUG125.11; 

	      TITLE: VIDEO TERMINAL LIBRARIES;
            VERSION: 1.0;
       	       DATE: 5/15/85;
        DESCRIPTION: "Library of video terminal routines
		      to use control codes for the DEC vt52";
	   KEYWORDS: DEC vt52, video terminal routines;
             SYSTEM: DEC vt52, PDP-11, VAX;
           FILENAME: LIBVT52.C;
	   WARNINGS: "Drives only DEC vt52; does not drive IBM PC
		     video";
	   SEE-ALSO: LIBVT52.NRO;
            AUTHORS: Stephen L. Browning;
          COMPILERS: BDC C;
*/


/*
 *	Library of routines to utilize control codes
 *	for the DEC VT-52 terminal.
 *
 *	Stephen L. Browning
 *	5723 North Parker Avenue
 *	Indianapolis, Indiana 46220
 */

#include "a:crt.h"

#define ESC 033

/*
 *	Home the cursor and clear the screen
 */

clrscrn()
{
	putchar(ESC);
	putchar('H');
	putchar(ESC);
	putchar('J');
}

/*
 *	Erase entire line
 */

eralin(_y)
int _y;
{
	putchar(ESC);		/* setcur(0,_y) */
	putchar('Y');
	putchar(23-_y+' ');
	putchar(' ');
	putchar(ESC);		/* erteol() */
	putchar('K');
}

/*
 *	Erase from cursor to end of line
 */

erteol()
{
	putchar(ESC);
	putchar('K');
}

/*
 *	Erase from cursor to end of screen
 */

erteos()
{
	putchar(ESC);
	putchar('J');
}

/*
 *	Set terminal in graphics mode
 */

graphics()
{
	putchar(ESC);
	putchar('F');
}


/*
 *	Move cursor to home position
 */

home()
{
	putchar(ESC);
	putchar('H');
}

/*
 *	Move cursor to one of eight adjacent locations
 */

movcur(_dir)
int _dir;
{
	switch (_dir) {
	case N:
		putchar(ESC);
		putchar('A');
		break;
	case NE:
		putchar(ESC);
		putchar('A');
		putchar(ESC);
		putchar('C');
		break;
	case E:
		putchar(ESC);
		putchar('C');
		break;
	case SE:
		putchar(ESC);
		putchar('B');
		putchar(ESC);
		putchar('C');
		break;
	case S:
		putchar(ESC);
		putchar('B');
		break;
	case SW:
		putchar(ESC);
		putchar('B');
		putchar(ESC);
		putchar('D');
		break;
	case W:
		putchar(ESC);
		putchar('D');
		break;
	case NW:
		putchar(ESC);
		putchar('A');
		putchar(ESC);
		putchar('D');
		break;
	}
}

/*
 *	Set terminal to its default power up mode
 */

normal()
{
	putchar(ESC);
	putchar('G');
}


/*
 *	Set cursor to coordinate value.
 *	0,0 is lower left corner of display
 */

setcur(_x,_y)
int _x, _y;
{
	putchar(ESC);
	putchar('Y');
	putchar(23-_y+' ');
	putchar(_x+' ');
}
