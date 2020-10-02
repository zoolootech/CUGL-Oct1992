/* wmousemv.c = routine to move mouse to a specified position
 */
 
#include "wsys.h"


void wmouse_move ( int x, int y )
	{
	int xabs, yabs;
	PSEUDOREGS
	
	
	#ifdef TEXTONLY
	/* TEXT MODE - translate characters into mickeys.
	 */
	xabs = 8* ( x + w0-> winleft );
	yabs = 8* ( y + w0-> wintop  );
	
	#else	/* TEXTONLY */
	
	if ( wmode == 'T' )
		{
		xabs = 8* ( x + w0-> winleft );
		yabs = 8* ( y + w0-> wintop  );
		}
	else
		{
		/* graphics mode translate 'characters' into pixels, 1 pixel=1 mickey
		 */
		xabs = wpxchar * (x + w0-> winleft);
		yabs = wpxchar * (y + w0-> wintop );
		}
	
	#endif /* ! TEXTONLY */
	
	
	_DX = yabs;
	_CX = xabs;
	_AX = 0x04;
	INTERRUPT ( 0x33 );
	
	return;		/* wmouse_move */
	}
