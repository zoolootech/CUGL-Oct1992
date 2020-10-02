/*! wscroll
 *
 * move text up one line in current window
 *
 *
 */

#include "wscreen.h"
#include "wsys.h"

void wscroll(void)
	{


	unsigned char far *target_line;
	unsigned char far *source_line;

	int nrow, ncol, col;


	char    clr_attr;

	#ifndef TEXTONLY
		/* graphics mode declarations
		 */
		int right, bottom;
	#endif	/* ! TEXTONLY */


	clr_attr = wgetattr();



	if (wmode == 'T')
		{
		/* point to top line, then work done one line at at time
		 */
		target_line = wpage_ram + 2*80*(w0->wintop) +2*(w0->winleft);

		/* number of rows to scroll is 1 less than current row
		 * but we start counting from zero.
		 */
		nrow = w0->winy;
		ncol = 2*(w0-> winxmax +1);

		while (nrow--)
			{
			source_line = target_line + 2*80;
			for (col =0; col < ncol; ++col)
				{
				*(target_line +col) = *(source_line +col);
				}
			target_line =source_line;
			}
		/* write a new blank line */
		for (col = 0; col < ncol ;  col += 2)
			{
			*(source_line + col +1) = clr_attr;
			*(source_line + col   ) = ' ';
			}
		}
#ifndef TEXTONLY
	/* graphics mode scrolling
	 */
	else 	{
		right  = w0->winleft +w0->winxmax;
		bottom = w0->wintop +w0->winy;
		wgscrollv	( w0->winleft, w0->wintop +1, right, bottom,
				-wpychar );

		wclearabs 	( w0->winleft, bottom, right, bottom  );

		}

#endif	/* ifndef TEXTONLY - end graphics mode scrolling */

	/*position to start of line */
	wgoto(0,-1);

	return; 		/* end of wscroll */
	}

