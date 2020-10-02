/*! wframe
 *
 *
 *	draw a  box on the screen using direct video RAM access
 *
 */
#include  "wsys.h"



void wframe (	int left, int top, int right, int bottom,
		int boxtype, unsigned char box_attr)
	{
	int 	x,y;
	struct WBOX_PATTERN  box;
	
	/* copy the box to local memory, 
	 * so it can be accessed without using index methods
	 * and faster in LARGE model.
	 */
	box = wbox[boxtype];
	

	/* draw top of box */
	wputcabs ( left, top, box.nw, box_attr, WGOVERWRITE );
	for ( x = left+1; x <right; ++x )
		{
		wputcabs ( x, top, box.horiz, box_attr,
			WGOVERWRITE );
		}
	wputcabs ( right, top,  box.ne, box_attr, WGOVERWRITE );

	/* draw lines with vertical edge borders and clear centers */
	for ( y = top+1; y < bottom; ++y )
		{
		/* one loop iteration per horizontal line */

		/* draw left hand edge of box */
		wputcabs ( left, y, box.vert, box_attr, WGOVERWRITE );

		/* draw right-hand edge of box */
		wputcabs ( right, y, box.vert, box_attr, WGOVERWRITE);
		} /*end of loop to draw lines with vertical edges */


	/*  draw bottom edge of box */
	wputcabs ( left, bottom, box.sw, box_attr, WGOVERWRITE );
	for ( x = left+1; x <right; ++x )
		{
		wputcabs ( x, bottom, box.horiz, box_attr,
			WGOVERWRITE );
		}
	wputcabs ( right, bottom, box.se, box_attr, WGOVERWRITE );





return;  /* end of wframe  */
}

