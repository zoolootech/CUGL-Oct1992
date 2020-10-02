
/*! wgoto
 *
 *
 *    position the cursor, hide and restore the cursor
 *
 *
 * use : valid positions are 0  <= row <= winymax
 *			 and 0t <= col <= winxmax
 *
 * calling with -1 produces no movement
 * this is used internally to move the cursor in other routines
 *
 *		if row = -1 and col = nn, will set to col nn on current row
 *              			 (allows horizontal movement)
 *		if col = -1 and row = nn, will set to row nn at current col
 *					 (allows vertical alignment)
 *
 *
 */
#include  "wscreen.h"
#include  "wsys.h"


#include <dos.h>


void wgoto(int col, int row)
	{
	PSEUDOREGS


	int page;
	int rowabs, colabs;





	/* calls with -1 mean use current row or col
	 */
	if ( row == -1 )
		{
		row = w0-> winy;
		}

	if  ( col == -1)
		{
		col = w0-> winx;
		}



	/* is request out of current window ? */
	if (  col > w0-> winxmax
	   || row > w0-> winymax
	   || row < 0
	   || col < 0            )
		{
		return;
		}

	/* use new co-ords
	 */
	w0-> winy 	= row;
	w0-> winx 	= col;




	if (wmode == 'T' && ( (w0-> winflag) & WFL_CURSOR ) )
		{

		/* NOTE that using TurboC's pseudoregs
		 * means all far * calculations have to
		 * be done before loading regs.
		 */

		page = w0-> winpage;
		rowabs = row + w0-> wintop;
		colabs = col + w0-> winleft;

		_DH = rowabs;
		_DL = colabs;
		_BH = page;  /* video page taken from active window */
		_AH = 2;  /* function to set cursor */
		INTERRUPT(0x10);

		}

	return;


	}	/*end of wgoto routine */
