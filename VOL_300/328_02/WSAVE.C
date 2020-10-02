/* wsave
 *
 * wsave text/graphics in current window frame,
 * move it to 'wheap' storage, store address of the saved text in winsave
 * if out of memory store NULL to winsave.
 *
 */
 

#include  "wscreen.h"
#include  "wsys.h"




void wsave ( void )
	{
	int extra, extra2;		/* need extra lines for borders ? */



	/* text_mode screen pointers */
	unsigned char   far *line_ptr;

	/* pointers to screen save area */
	WHEAP 		    *hptr;
	unsigned char   far *area;


	int		l, t, r, b;	/* co-ords inlcuding window frame */

	int 		movenum;

	unsigned 	long int  nbytes;


	struct _CHATTR 		/* 2 byte char/attr */
		{
		unsigned char c;
		unsigned char a;
		};
	typedef struct _CHATTR far *CHATTR;	/* far pointer to a pair */


	#ifndef __TURBOC__
	/* Microsoft C needs an 'intermediary' ptr to handle syntax correctly.
	 */
	void far *msc_ptr;
	#endif  /* Microsoft 'idiot' fix */

#ifndef TEXTONLY
	unsigned int	hn;		/* hercules bank offsets */
	int		color;		/* EGA color bit mask */
	int		t_save; 	/* save top row # for each color */

	/* graphics mode screen pointers */
	unsigned char far *base, far *base_save;
#endif





	/* if there is going to be a border
	 * then the area to save has to be larger than the text window
	 * (border occupies 1 line extra in each direction)
	 */
	extra = (w0-> winbox ) ? 1 : 0;
	extra2 = 2*extra;

	l = (  (w0->winleft)    -extra );
	t = (  (w0->wintop)     -extra );
	r = (l+(w0->winxmax)    +extra2 );
	b = (t+(w0->winymax)    +extra2 );


	/* number of characters in the window
	 */
	movenum = r-l+1; 		/* one text row */
	nbytes =  movenum*(b-t+1);




	if ( wmode == 'T' )
		{

		/* number of bytes to allocate
		 * multiply by 2 (= text+attributes)
		 */
		hptr =  wheap_alloc ( 2*nbytes, WIN_HP_PRIORITY, NULL );

		w0->winsave = hptr;

		if (hptr == NULL)
			{
			return;
			}

		area = wheap_access ( hptr, 0 );

		movenum *= 2;	/* char and attr */


		for (	line_ptr  = wpage_ram + 2*80*( t ) + 2*( l );
			t<=b;
			++t,  line_ptr += 2*80
		    )
			{
			/* move one line per pass thru this loop
			 * loop limit is one char beyond end of line
			 */
			farmemcpy ( area, line_ptr, movenum );
			area += movenum;
			}

		}

#ifndef TEXTONLY
	else
		{
		/* graphics mode save
		 *
		 * Note that r and b point to the last line INCLUSIVE
		 * so loops need to go up to and include.
		 */

		/* convert single chars (=1 byte in text mode)
		 *	to blocks of pixels in graphics mode
		 */
		nbytes *= (wpxchar/8) * wpychar;


		if (wmonitor != 'H')
			{
			/* EGA and VGA -- 4 colors */
			nbytes *= 4;
			}



		if ( nbytes >= WHEAP_MAX )
			{
			/* too large to save...
			 * convert window to WSAVE2NULL
			 */
			w0->winsave  = NULL;
			return;
			}

		hptr = wheap_alloc ( nbytes, WIN_HP_PRIORITY, NULL );

		w0-> winsave = hptr;

		if ( hptr == NULL )
			{
			return;
			}

		area = wheap_access ( hptr, 0 );


		/* convert top and bottom counters
		 * to pixel co-ords only for counting rows
		 */
		t *= wpychar;
		b  = (b+1) * wpychar;  /* includes last pixel row */


		/* number of bytes in each row of pixels
		 */
		movenum = r-l+1;



		/* get image */

		switch ( wmonitor )
			{
		case ( 'H'):
			/* NOTE addressing hercules graphics ram:
			 *	l, t are 'text-mode' addresses.
			 *	720 pixels/row, wpxchar pixels per 'x' incr
			 *	   so 1st factor = wxabsmax = chars per row
			 *	divide wpychar by 4(=# herc banks) =
			 *	  gives #of sets of banks (4 pxl rows each)
			 *	wpxchar/8 =1 always, included for clarity.
			 *
			 *	because charsizes are mults of 4,
			 *	whbank starts at 0;
			 *
			 *	also note that l and t have been converted
			 *		from character rows to pixel rows
			 *              so don't need wpychar.
			 *	however, r & l still count text cols, not pxl
			 */
			base	=  wpage_ram
				+ ( (720/wpxchar)*(t/4) ) + l;


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
				#ifdef __TURBOC__
					/* much easier to move all the data in TurboC
					 */
					farmemcpy ( area, base+hn, movenum );
				#else
					/* Microsoft C preprocessor generates wrong code 
					 * in small model.
					 */
					msc_ptr = base+hn;

					farmemcpy ( area, msc_ptr, movenum );
				#endif	/* Microsoft vs Turbo C */
				
				area  += movenum;	/* move ptr to target dn one row */
				}
			break;   	/* end hercules mode */

		case ( 'E' ):
		case ( 'V' ):
			EGA_OUT (1, EGA_ENABLE );
			/* how to put point = EGA_OUT(3, putmode)*/
			/*   0x18 = XOR  0x10 = OR  0x08 = AND */


			/*  
			 *	note that t, b have been converted to pixel rows
			 *		rather than char rows.
			 *	however, r&l still count text columns,
			 *		 not pixel columns
			 */
			base_save =
			base = wpage_ram + ( wegarowsize * t ) + l;

			t_save = t;

			/* default for most graphics drivers is to leave
			 * write mode = 0 and EGA output enabled.
			 * so you don't need these, but in future, ...
			 * EGA_OUT ( 1, EGA_ENABLE ), EGA_OUT ( 5, 0 );
			 *
			 */

			for ( color = 0; color < 4; ++color )
				{
				/* becuase the size of the save area is <64k
				 *	(imposed above), the pointers are
				 * always correctly normalized
				 * If larger save areas are desired,
				 * renormalize (area) for each new color
				 * which will suffice even for super VGA
				 *	(800*600 = 60k per color)
				 */
				base = base_save;
				t = t_save;

				EGA_OUT (4, color);

				for ( 	;	/* loop once per row */
					t < b;
					++t, base += 640/wpxchar )
					{
					farmemcpy ( area, base, movenum );
					area += movenum;
					}	/* end for each rows */

				}/* end loop for each color */

				/* reset EGA */
				EGA_OUT (0,0);			/* BLACK */
				EGA_OUT (8, 0xff);              /* all bits */


				break;
				} 	/* end switch (wgdriver) */


		/* end of get image in graphics mode */

		}

#endif	/* ifdef TEXTONLY - end of graphics mode save to RAM */

	wheap_deaccess ( hptr, 1 );

	return;
	}

/*-------------------------end of wsave---------------------------*/



