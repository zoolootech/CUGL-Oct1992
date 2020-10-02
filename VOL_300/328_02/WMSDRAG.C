/* WMSDRAG.C - contains routines for dragging windows by a mouse.
 */
#include "wsys.h"

static int (*old_trap)(int) = NULL;
static char installed =0;
static int 	handle_drag (int); 
static void W_NEAR draw_borders ( int l, int t, int w, int h );
static void W_NEAR remove_borders (void);

void wmsdrag (void)		/* install keytrap */
	{
	if ( !installed )
		{
		old_trap = wkeytrap;
		wkeytrap = handle_drag;
		installed = 1;
		}
	return;		/* wmsdrag() */		
	}
	
static int handle_drag (int key)
	{
	int 	l, t,					/* new top and bottom */ 
			w, h;					/* width and height */  
	int done;	
	

	/* did user 1) click left mouse button 2) on upper left corner of frame
	 *			and 3) is this window draggable ?
	 */
	if 	(  ( key==MOUSE ) && ( wmouse.wms_used & WMS_LEFT_PRS ) 
		&& ( wmouse.wms_x == -1 ) && ( wmouse.wms_y == -1 ) 
		&& (w0->winbox !=0) && ( w0-> winsave != NULL )
		)
		{
		w = w0-> winxmax +3;		/* actual # of columns + 2 for border */
		h = w0-> winymax +3;

		done = 0;	

		draw_borders ( wmouse.wms_xabs, wmouse.wms_yabs, w, h );
		do 
			{
		
			wmouse_location ();
			l = wmouse.wms_xabs;
			t = wmouse.wms_yabs;

			if ( (l+w+1 < wxabsmax) && (t+h+1 < wyabsmax) )
				{
				/* remove prev. borders and redraw in new location */
				remove_borders ();
				draw_borders ( l,t,w,h );
				}
			done = wmouse.wms_used & (WMS_LEFT_RLS | WMS_RIGHT_RLS );
				
			}
		while ( ! done );	/* end do () */
		remove_borders ();
				
		if ( 0== (done & WMS_RIGHT_RLS ) )
			{
			wrelocate ( l+1, t+1 );		/* move current window */
			} 	
		key = 0;
		
		wmouse_flush ();		
		}
	else
	if ( old_trap != NULL )
		{
		key = (*old_trap)(key);		/* chain previous handler */
		}	
	
	return (key);		/* handle_drag () */
	}
	
static void W_NEAR draw_borders ( int l, int t, int w, int h )
	{
	int r, b;
	r = l + w -1;
	b = t + h -1;	

	wopen (l, t, 1, h, LIGHTGRAY, NO_BORDER, 0, WSAVE2RAM ); 		
	wsetc ( 247 );
	wopen (l,  t, w, 1, LIGHTGRAY, NO_BORDER, 0, WSAVE2RAM ); 		
	wsetc ( 247 );
	wopen (r, t, 1, h, LIGHTGRAY, NO_BORDER, 0, WSAVE2RAM ); 		
	wsetc ( 247 );
	wopen (l, b, w, 1, LIGHTGRAY, NO_BORDER, 0, WSAVE2RAM ); 		
	wsetc ( 247 );
	return;		/* draw_borders */
	}	
	
	
static void W_NEAR remove_borders (void)
	{
	wclose(), wclose(), wclose(), wclose();
	return;
	}
/*------------------ end of WMSDRAG.C ----------------*/
