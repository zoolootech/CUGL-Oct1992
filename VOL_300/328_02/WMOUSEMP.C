/* wmousemv.c = routine to move mouse to a specified position
 */
#ifndef TEXTONLY
	/* this is strictly a graphics-mode program. */
	
#include "wsys.h"
	 

void wmouse_move_pixel ( int x, int y )
	{
	int xabs, yabs;
	PSEUDOREGS
	
	/* graphics mode translate 'characters' into pixels, 1 pixel=1 mickey
	 */
	xabs = x+ wpxchar * ( w0-> winleft);
	yabs = y+ wpxchar * ( w0-> wintop );
	
	_DX = yabs;
	_CX = xabs;
	_AX = 0x04;
	INTERRUPT ( 0x33 );
	
	return;		/* wmouse_move_pixel */
	}
#endif 	/* TEXTONLY */
