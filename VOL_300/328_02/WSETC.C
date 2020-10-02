/* wsetc() - set all chars in window to a specified char
 * 			 starting at current position, going to end of window
 */
#include "wsys.h"

void wsetc( char c )
	{
	int x, y, l, t, r, b;
	unsigned char a;
	
	
	a = w0-> winattr;
	l = w0-> winleft;
	t = w0-> wintop;
	x = w0-> winx 	 + l;
	r = w0-> winxmax + l;
	y = w0-> winy 	 + t;
	b = w0-> winymax + t;
	
	do 
		{
		do 
			{
			wputcabs ( x, y, c, a, WGOVERWRITE );
			++x;
			}
		while ( x <= r );
		x=l;
		++y;
		}
	while ( y <= b );
	
	return;		/* wsetc */
	}