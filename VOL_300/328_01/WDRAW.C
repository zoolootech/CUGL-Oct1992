/* wdraw.c
 *	routines for mouse-controlled drawing of lines, squigles, squares
 *	use:
 *		call the procedure specifying type of drawing to do.
 *		(see definitions of WDR_LINE, WDR_RECT, etc for types,
 *		linestyle, thickness, color are set by TurboC graphics
 *		(user-defined linestyle is not supported).
 *
 * This program does not work in Microsoft C, which doesn't support 
 * drawing.
 */





/*--------------------------------------------*/




#ifndef TEXTONLY


#ifdef __TURBOC__


#include "wsys.h"
#include <graphics.h>


static W_NEAR xdraw ( WOBJECT_D_ART * );



WOBJECT_D_ART *wdraw ( char type, int color, int linestyle, int thickness )
	{
	WOBJECT_D_ART *wdr;
	int key;
	int first_pass =1;

	struct linesettingstype   save_line_set;
	int save_color;
	
	/* get keys until either Left mouse button pressed or ESCAPE
	 */

	wmouse_limit (ON);
	
	while ( ( (key=wgetc()) != MOUSE ) && (key != ESCAPE) )
		{
		/* empty */
		}
	if ( (key==ESCAPE ) || ( (wmouse.wms_used & WMS_LEFT_PRS) == 0 )  )
		{
		wmouse_limit (OFF);
		return (NULL);
		}
		
	wmouse_turn (OFF);	
	

	getlinesettings ( &save_line_set );
	save_color = getcolor ();


	wdr = wmalloc ( sizeof (WOBJECT_D_ART), "WDRAW");
	memset ( wdr, 0, sizeof (WOBJECT_D_ART) );
	wdr->wdr_type 		= type;
	wdr->wdr_linestyle 	= linestyle;
	wdr->wdr_thickness 	= thickness;
	wdr->wdr_color	   	= color;
	wdr->wdr_x1             = wdr->wdr_x2           = wmouse.wms_px;
	wdr->wdr_y1 	    	= wdr->wdr_y2		= wmouse.wms_py;


	setlinestyle ( linestyle, 0, thickness );
	setcolor (color);

	setwritemode (1);		/* XOR */

	if ( type == WDR_SQUIGGLE )
		{
		moveto (wdr->wdr_x1, wdr->wdr_y1);
		}

	do
		{

		if ( (! first_pass ) && (type != WDR_SQUIGGLE) )
			{
			xdraw (wdr);	/* erase old image */
			wmouse.wms_used |= WMS_MOVED;  /* force first draw */
			}
		first_pass =0;


		if ( ( wmouse.wms_used & WMS_MOVED ) != 0 )
			{
			wdr->wdr_x2 = wmouse.wms_px;
			wdr->wdr_y2 = wmouse.wms_py;

			xdraw (wdr);	/* redraw */
			}

		wmouse_location ();

		}
		/* continue loop until either left or right key is released.
		 */
	while  ( (wmouse.wms_used & (WMS_LEFT_RLS | WMS_RIGHT_PRS )) == 0 ) ;


	if 	( (wmouse.wms_used & WMS_RIGHT_PRS ) != 0 )
		{
		/* RIGHT BUTTON = ESCAPE, erase drawing.
		 */
		if ( (! first_pass ) && (type != WDR_SQUIGGLE) )
			{
			xdraw (wdr);
			}
		}


	/* restore prior drawing environment
	 */
	setwritemode (0);	/* overwrite */
	setlinestyle ( 	save_line_set.linestyle,
					save_line_set.upattern,
					save_line_set.thickness );
	setcolor (save_color);
	wmouse_turn (ON);
	wmouse_limit (OFF);

	/* flush mouse 
	 */
	do 
		{
		wmouse.wms_internal =0;
		wmouse_location ();
		}
	while ( wmouse.wms_used );
	

	return ( wdr );         /* wdraw, TurboC version */
	}







/* slave routine for wdraw() - uses XOR_PUT to either write or erase
 * the object using x2, y2 as new co-ords
 */
static W_NEAR xdraw ( WOBJECT_D_ART *wdr )
	{

	int ymax, xmax;

	xmax =w0->winpxmax;
	ymax =w0->winpymax;

	switch ( wdr-> wdr_type )
		{
	case ( WDR_LINE ):
		line ( wdr->wdr_x1, wdr->wdr_y1, wdr->wdr_x2, wdr->wdr_y2 );
		break;

	case ( WDR_RECT ):
		line ( wdr->wdr_x1, wdr->wdr_y1, wdr->wdr_x2, wdr->wdr_y1 );
		line ( wdr->wdr_x2, wdr->wdr_y1, wdr->wdr_x2, wdr->wdr_y2 );
		line ( wdr->wdr_x2, wdr->wdr_y2, wdr->wdr_x1, wdr->wdr_y2 );
		line ( wdr->wdr_x1, wdr->wdr_y2, wdr->wdr_x1, wdr->wdr_y1 );
		break;

	case ( WDR_XHAIR ):
		line ( wdr->wdr_x2, 0,           wdr->wdr_x2, ymax );
		line ( 0,           wdr->wdr_y2, xmax, wdr->wdr_y2 );
		break;

	case ( WDR_VERT ):
		line ( wdr->wdr_x2, 0,           wdr->wdr_x2, ymax );
		break;

	case ( WDR_HORZ ):
		line ( 0,           wdr->wdr_y2, xmax,        wdr->wdr_y2 );
		break;

	case ( WDR_SQUIGGLE ):
		lineto ( wdr->wdr_x2, wdr->wdr_y2 );
		break;

		}





	return;		/* xdraw */
	}


#endif          /* end TurboC */


#endif /*TEXTONLY */
