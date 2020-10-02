/*! wgscrollv ()
 *	scroll an area of graphics screen vertically by nr # of pixel rows.
 *	Note that in dealing with text, only multiples of 8 make sense,
 *		but the more general routine is provided for graphic images.
 *
 *	The area is defined by text mode co=ords l,t,r,b
 *	nr is + to scroll down, - to scroll up.
 * 	The bottom lines are not changed.
 */
#include  "wscreen.h"
#include  "wsys.h"





void 	wgscrollv ( int l, int t, int r, int b, int nr )
	{



	/* variables used as loop index (counters & ptrs)
	 */
	int col;
	unsigned char far *coltarget;
	unsigned char far *colsource;
	unsigned char far *target;
	unsigned char far *source;



	/* virtual loop control ( to set loop bounds and direction )
	 *
	 *	needed because loops go either top to bottom (scroll up)
	 *				    or bottom to top (       dn)
	 *
	 *	'virtual top' may be actual top or bottom,depending...
	 *	similar for bottom.
	 *	'virtual_1' is +1 if increasing rows (top->bottom)
	 *		       -1 if decreasing rows.
	 *	'virtual_htarget, _hsource are hercules bank offsets
	 *			for target and source rows.
	 *	'virtual_hincr' is either +0x2000 or -0x2000, as above.
	 *			(hercules bank offsets)
	 *	'virtual_h_limit is either 0x8000 or 0x0000 = bank limit.
	 */
	int v_t, v_b, v_1, v_htarget, v_hsource, v_hincr, v_hlimit, v_hreset;
	int v_rowincr;

	/* convert to pixel co-ords only for counting rows */
	t *= wpychar;
	b = (b+1) * wpychar;




	switch ( wmonitor )
		{
	case ('H'):
		/* NOTE addressing hercules graphics ram:
		 *	l, r are 'text-mode' addresses.
		 *	720 pixels/row, wpxchar pixels per 'x' increment
		 *		so 1st factor = wxmax = chars per row
		 *	divide t, b, (t+nr) are row numbers in pixel co=ords
		 *	 	( of bottom, top for source, top for target)
		 *	wpxchar/8 =1 always, included only for clarity.
		 *
		 *	because charsizes are mults of 4, whbank starts at 0;
		 *		hnsource starts at 0.
		 *		hntarget is  - (nr mod 4)
		 */



		if ( nr < 0 )
			{
			/* scrolling up:  want to go top to bottom */
			v_t = t;
			v_b = b;
			v_1 = 1;
			v_hincr = 0x2000;               /* loop up in banks*/
			v_hlimit = 0x8000;
			v_hreset = 0;
			v_rowincr = 720/8;		/* next row, up */
			v_htarget = ( -nr ) % 4 ;	/* herc bank number*/

			}
		else
		if ( nr > 0 )
			{
			/* scrolling down: want to go bottom to top */
			v_t =  b;	/* virtual top = actual bottom */
			v_b =  t;
			v_1 = -1;	/* minus one causes row# decrement */
			v_hincr = -0x2000;           	/* loop down in banks */
			v_hlimit = 0x0000;
			v_hreset = 0x8000;
			v_rowincr = -720/8;	/* next row, down */
			v_htarget = ( nr ) % 4 ;

			}
		else 	{
			/* request to scroll 0 rows */
			return;
			}
		v_htarget *= 0x2000;	/* convert bank number to offset */
		v_hsource = 0;		/* herc bank number for source
					 * note source row is mult of 8
					 */

		source	=  wpage_ram
			+ ( (720/wpxchar)*(  v_t    /4) ) +l;
		target  =  wpage_ram
			+ ( (720/wpxchar)*( (v_t+nr)/4) ) +l;


		while ( v_t != v_b )
			/* loop until virtual top equals virtual bottom */
			{
			/* loop iterates once per row
			 */
			if ( v_htarget == v_hlimit )
				{
				/* finished one set of 4 banks,
				 * move up/down to next tier.
				 */
				v_htarget  = v_hreset;
				target     += v_rowincr;
				}
			if ( v_hsource == v_hlimit )
				{
				/* finished one set of 4 banks,
				 * move up to next tier.
				 */
				v_hsource = v_hreset;
				source   += v_rowincr;
				}

			for ( 	col = l, 	coltarget = target,
						colsource = source;
				col <=r;
				++col, ++coltarget, ++colsource)
				{
				/* once per column within each row */
				*( coltarget + v_htarget ) =
					*( colsource + v_hsource );
				}
			v_htarget += v_hincr;	/* if scroll up, subtracts */
			v_hsource += v_hincr;   /* if scroll dn, adds */
			v_t       += v_1;       /* if up, -1, if down +1 */
			}
		break;   	/* end hercules mode */

	case ( 'E' ):
	case ( 'V' ):
		/* use write mode 1 to scroll the screen */


		if ( nr < 0 )
			{
			/* scrolling up:  want to go top to bottom */
			v_t = t;
			v_b = b;
			v_1 = 1;
			v_rowincr = 640/wpxchar;		/* next row, up */
			}
		else
		if ( nr > 0 )
			{
			/* scrolling down: want to go bottom to top */
			v_t =  b;	/* virtual top = actual bottom */
			v_b =  t;
			v_1 = -1;	/* minus one causes row# decrement */
			v_rowincr = -640/wpxchar;	/* next row, down */
			}
		else 	{
			/* request to scroll 0 rows */
			return;
			}


		source	=  wpage_ram
			+ (wegarowsize)*(  v_t     *wpychar/8)
			+ l * (wpxchar/8) ;
		target  =  wpage_ram
			+ (wegarowsize)*( (v_t+nr) *wpychar/8)
			+ l * (wpxchar/8) ;


		while ( v_t != v_b )
			/* loop until virtual top equals virtual bottom */
			{
			/* loop iterates once per row
			 */

			EGA_OUT (2, 0x0F);
			EGA_OUT (5, 1);		/* write mode 1 */


			for ( 	col = l, 	coltarget = target,
						colsource = source;
				col <=r;
				++col, ++coltarget, ++colsource)
				{
				/* once per column within each row */
				*( coltarget ) = *( colsource );
				}
			target    += v_rowincr;	/* if scroll up, subtracts */
			source    += v_rowincr; /* if scroll dn, adds */
			v_t       += v_1;       /* if up, -1, if down +1 */
			}

			EGA_OUT (5,0);		/* write mode 0 */




		break;
		}	/* end switch on monitor type */






	return;			/* end wgscrollv */
	}
