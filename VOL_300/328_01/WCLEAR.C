/*! wclear
 *	clear an area of screen to current background
 *
 */
#include  "wscreen.h"
#include  "wsys.h"



void wclear  ( void )
	{
	wclearabs ( 	w0->winleft, 		    w0->wintop,
			w0->winleft + w0->winxmax,  w0->wintop + w0->winymax
		  );
	wgoto ( 0,0 );
	return;
	}

void wclearline  ( void )
	{
	wclearabs ( 	w0->winleft + w0->winx,     w0->wintop + w0->winy,
			w0->winleft + w0->winxmax,  w0->wintop + w0->winy
		  );
	return;
	}



void wclearabs ( int l, int t, int r, int b )
	{
	unsigned char far *p, far *s;	/* to video ram */
	int 	 col;			/* column counter */


	struct _CHATTR
		{
		unsigned char c;
		unsigned char a;
		}
		chattr;
	typedef struct _CHATTR far *CHATTR;	/*ptr to a far ch/attr pair*/





	#ifndef TEXTONLY

	unsigned char a;		/* background attribute */
	unsigned int 	hn;	     	/* hercules bank offset */
	unsigned char far *base;	/* base of each row */
	unsigned char far *colptr;	/* pointer to column */

	unsigned char latch;
	#endif	/* TEXTONLY - end graphics-mode declarations */



	if  ( l < 0 ||  r > wxabsmax ||  t < 0 || b > wyabsmax )
		{
		return;
		}




	if ( wmode == 'T' )
		{
		s = wpage_ram + 2*80*t + 2*l;

		chattr.c =' ';
		chattr.a = wgetattr();



		for ( ; t <= b; ++t, s += 2*80 )
			/* s pts to start of each line */
			{
			for ( col = l, p = s; col <= r; ++col )
				{
				*((CHATTR) p) = chattr;
				p += 2;
				}
			}
		/* end text mode absolute clear */

		return;
		}	/* end text mode */


#ifndef TEXTONLY
	/* graphics mode wclearabs */



	a = ( wgetattr() ) >> 4;	/* background color */



	switch ( wmonitor )
		{
	case ( 'H'):
		/* NOTE addressing hercules graphics ram:
		 *	l, t are 'text-mode' addresses.
		 *	720 pixels/row, wpxchar pixels per 'x' increment
		 *		so 1st factor = wxabsmax = chars per row
		 *	divide wpychar by 4(=# herc banks) =
		 *		gives #of sets of banks (4 pixel rows each)
		 *	wpxchar/8 =1 always, included only for clarity.
		 *
		 *	because charsizes are mults of 4, whbank starts at 0;
		 */
		base	=  wpage_ram
			+ ( (720/wpxchar)*(wpychar/4)*t ) + l;

		/* in hercules, 'dither' blue and green bits
		 * note this line changes 'a' from attr to bitmask.
		 * and sets 8 adjacent bits on screen
		 */
		a = (a & 0x03) ? 0xff : 0x00;

		/* convert to pixel co-ords only for counting rows */
		t *= wpychar;
		b  = (b+1) * wpychar;  /* includes last pixel row */


		for ( hn = 0; t < b;  ++t, hn += 0x2000 )
			{
			/* loop iterates once per row
			 */
			if ( hn == 0x8000 )
				{
				/* finished one set of 4 banks,
				 * move up to next tier.
				 */
				hn = 0;
				base += 90;
				}
			for ( 	col = l, colptr = base;
				col <=r;
				++col, ++colptr)
				{
				/* once per column within each row */
				*( colptr + hn )  = a;
				}
			}
		break;   	/* end hercules mode */

	case ( 'E' ):
	case ( 'V' ):

		base = wpage_ram + ( wegarowsize * wpychar * t ) + l;

		/* convert to pixel co-ords to count rows
		 */
		t *= wpychar;
		b  = (b+1)* wpychar; /* last pixel row in last char row*/

		EGA_OUT (1, EGA_ENABLE);
		EGA_BITPLANE (0x0f);


		EGA_OUT (3, 0x00);	/* OVERWRITE */
		/* write attribute to 8 bits */
		EGA_OUT (0, a);
		EGA_OUT (8, 0xFF);


		for ( 	;	/* loop once per row */
			t < b;
			++t, base += 640/wpxchar )
			{
				for (	col = l, colptr =base ;
					col <= r;
					++col, ++colptr)
					{

					*(colptr) = 0xff;

					}
			}	/* end for each rows */

		/* reset EGA */
		EGA_OUT (0,0);			/* BLACK */


		break;
		} 	/* end switch (wgdriver) */

#endif	/* TEXTONLY - end graphics mode wclearabs */
	return; /* wclearabs */


	}


/*---------------------- end of WCLEAR.C -----------------------------*/

