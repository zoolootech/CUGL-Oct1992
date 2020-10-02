/*! wcursor
 *
 *
 *    turn cursor on or off
 *
 * 	use : 	1 = ON  turns cursor on
 *       	0 = OFF turns cursor off
 *
 */
#include "wscreen.h"
#include "wsys.h"


#include <dos.h>



void wcursor(int want_cursor)
	{
	int row, col, page;

	PSEUDOREGS;




	#ifndef TEXTONLY

		if (wmode == 'G')
			{
			return;
			}

	#endif	/* TEXTONLY */




	if    	(want_cursor == ON)
		{
		w0-> winflag |= WFL_CURSOR;

		/* move the physical cursor to the logical location
		 * 	NOTE that using TurboC's pseudoregs
		 * 	means all far * calculations have to
		 * 	be done before loading regs.
		 */
		page = w0-> winpage;
		col = w0-> winleft +w0->winx;
		row = w0-> wintop  +w0->winy;
		_DH = row;
		_DL = col;
		_BH = page;
		_AH = 2;       /* function to set cursor position */
		INTERRUPT(0x10);

		/*load new cursor size
		 */
		_CX = wcurscanln;

		_AH = 1;    /* indicates cursor size request */
		INTERRUPT (0x10);



		}

	else 	/* want_cursor OFF */
		{
		w0-> winflag &= (0xff - WFL_CURSOR);

		_CH = 0x2E;  /* stop cursor */
		_CL = 0x20;
		_AH = 1;
		INTERRUPT (0x10);
		}

	return;
	}	/*end wcursor routine */






