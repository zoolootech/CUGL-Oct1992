/* wmouse_limit ()
 *    routine to limit mouse movement to the current window.
 *		PARAMETER = OFF (=0) turns off limitation, mouse may roam freely
 *				  = ON       limits mouse to current window.
 */
#include "wsys.h"


	/* values for mouse driver to set limits
	 */
#define X_LIMIT 0x07
#define Y_LIMIT 0x08


static void W_NEAR setlimit ( int min, int max, int xy, int charsize);

void wmouse_limit (int);		/* ON=limit to current window */
void wmouse_limit (int onoff)
	{
	int max, min;
	
	
	/* calc max & min in mickeys ( see wmouse.c for relation to mickeys )
	 */
	if ( onoff )
		{
		/* setting limits ON 
		 */
		min = w0->winleft;  
		max = w0->winxmax + min;
		}
	else
		{
		/* setting limits OFF
		 */
		min = 0;
		max = wxabsmax;
		}
	

	setlimit ( min, max, X_LIMIT, wpxchar );





	/* now do same for y-coord */

	if ( onoff )
		{
		/* setting limits ON
		 */
		min = w0->winleft;
		max = w0->winymax + min;
		}
	else
		{
		/* setting limits OFF
		 */
		min = 0;
		max = wyabsmax;
		}
	setlimit ( min, max, Y_LIMIT, wpychar );
		
	
	return;		/* wmouse_limit */
	}


static void W_NEAR setlimit ( int min, int max, int xy, int charsize )
	{
	PSEUDOREGS
	
	if ( wmode == 'T' )
		{
		/* in text modes, 8 mickeys per character onscreen.
		 */
		min *= 8;
		max *= 8;
		}
	#ifndef TEXTONLY
	else	
		{
		min *=  charsize;
		++max;			/* include all bits of last character in gr. mode */
		max *=  charsize;
		}

	#endif	/* TEXTONLY */


	_DX = max;
	_CX = min;
	_AX = xy;
	INTERRUPT ( 0x33 );	
	
	


	return;	/* setlimit */
	}
	
	/*------------------- end of WMOUSELM.C ------------------*/