/*! wrestore
 *
 *  wrestore from user save area to screen
 *
 *	note this routine assumes the screen save area
 *	was set up using wsave
 *	and the pointer to it in winsave is NORMALIZED
 */

#include  "wscreen.h"
#include  "wsys.h"




void wrestore (void)

	{
	int 	extra, extra2;       	/* size of frame surrounding window*/
	unsigned char save_attr;	/* save attribute of window */

	unsigned char 	far *line_ptr;

	WHEAP	*hptr;
	unsigned char   far *area;


	int	l, t, r, b;	/* co-ords of screen image including frame */
	int 	col;

	int     movenum;

	struct _CHATTR 		/* 2 byte char/attr */
		{
		unsigned char c;
		unsigned char a;
		};
	typedef struct _CHATTR far *CHATTR;	/* far pointer to a pair */


#ifndef TEXTONLY

	unsigned int	hn;		/* hercules bank offsets */
	int		color;		/* EGA color bit mask */
	int		t_save;		/* save top line # for each color */

	/* graphics mode screen pointers */
	unsigned char far *colptr, far *base, far *base_save;
	unsigned char 	latch;

	int     t_pix, b_pix;   /* top and bottom pixel rows. */


#endif


	#ifndef __TURBOC__
		/* Microsoft C requires an 'intermediary' ptr 
		 * to generate correct code.
		 */
		void far *msc_ptr;
		
	#endif	/* microsoft fix */






	/* check address of save area - if NULL, nothing saved.
	 */
	hptr = w0-> winsave;




	/* extra lines needed to include the border ?
	 *   	remember that winleft, top etc refer to the text parts,
	 *	and if a border is present, need to restore 1 extra line
	 *      in each direction
	 */
	extra = extra2 = 0;
	if ( w0-> winbox )
		{
		extra  = 1;
		extra2 = 2;
		}


	l = (     (w0-> winleft)   -extra  );
	r = ( l + (w0-> winxmax)   +extra2 );
	t = (     (w0-> wintop)    -extra  );
	b = ( t + (w0-> winymax)   +extra2 );

	movenum = r-l+1;	/* #bytes in each row */

	if ( hptr == NULL )
		{
		/* nothing saved -- just clear the area to black
		 * but rmemeber to clear out the window border as well.
		 */
		save_attr = wgetattr();
		wsetattr (BLACK);
		wclearabs (l,t,r,b);
		wsetattr(save_attr);

		return;		/*wrestore */
		}	/* end of clearing window if not saved */


	/* access heap element from expanded, ram or disk.
	 */
	area = wheap_access ( hptr, 1 );


	if ( wmode == 'T' )
		{
		movenum *= 2;


		for (	line_ptr  = wpage_ram + 2*80*( t ) + 2*( l );
			t <= b;
			++t,  line_ptr += 2*80
		    )
			{

			farmemcpy ( line_ptr, area, movenum );
			area += movenum;

			}
		/* end text-mode moving screen image */

		}

#ifndef TEXTONLY
	else
		{
		/* restore graphics mode windows. put image function
		 * Don't use BGI putimage because 1) significantly slower
		 * 2) doesn't always work if you only wrote using wputc/wputs
		 * or wclear since BGI keeps a 'buffer' and if the buffer
		 *    thinks it contains the screen
		 *    it will avoid going direclty to the
		 *    EGA registers,
		 *    so getimage/putimage will destroy the windows.
		 */



		/* graphics mode restore.
		 *
		 */

		/* convert to pixel co-ords only for counting rows */
		t_pix  = t*wpychar;
		b_pix  = (b+1) * wpychar;  /* one beyond last row */



		/* put image */

		switch ( wmonitor )
			{
		case ( 'H'):
			/* NOTE addressing hercules graphics ram:
			 *	l, t are 'text-mode' addresses.
			 *	720 pixels/row, wpxchar pixels per 'x' incr
			 *	so 1st factor = wxabsmax = chars per row
			 *	divide wpychar by 4(=# herc banks) =
			 *		gives #of sets of banks (4 pixel rows each)
			 *	wpxchar/8 =1 always, included for clarity.
			 *
			 *	because charsize is mult of 4,
			 *		whbank starts at 0;
			 *
			 *	Note that t, b have been converted to pixels,
			 *    		so don't need to multiply by wpychar again.
			 *	However, r & b still count text columns,
			 *  		not pixels
			 */
			base	=  wpage_ram
				+ ( (720/wpxchar)*(t_pix/4) ) + l;


			for ( hn = 0; t_pix < b_pix;  ++t_pix, hn += 0x2000 )
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
					/*  move data from save area to screen
					 */
					farmemcpy ( base+hn, area, movenum );
				#else
					/* Microsoft C - preprocessor generates incorrect code
					 * in the farmemcpy for small model --
					 * needs an 'intermediary' ptr to get it right.
					 */
					msc_ptr = base+hn;
					farmemcpy ( msc_ptr, area, movenum );
				#endif		/* Microsoft fix */
				
				area += movenum;

				}
			break;   	/* end hercules mode */

		case ( 'E' ):
		case ( 'V' ):
			EGA_OUT (1, EGA_ENABLE );
			/* how to put point = EGA_OUT(3, putmode)
			 *   0x18 = XOR  0x10 = OR  0x08 = AND
			 */


			/* blacken window first
			 * so old contents don't show up underneath new ones.
			 */
			save_attr = w0->winattr;
			w0->winattr = BLACK;
			wclearabs (l,t,r,b);
			w0->winattr = save_attr;



			/* co-ord in screen buffer of top/left pixel
			 * note that t_pix & b_pix count pixel rows
			 * but r &b still count text (byte) rows
			 */

			base_save =
			base = wpage_ram + ( wegarowsize * t_pix ) + l;
			t_save = t_pix;

			EGA_OUT ( 3,  0x10 );	/* OR into existing bits */


			for ( color = 1; color < 0x09; color <<= 1  )
				{

				EGA_OUT ( 0,  color);   /* select color plane */

				/* loop once per row */
				for ( 	base = base_save, t_pix = t_save;
					t_pix < b_pix;
					++t_pix, base += 640/wpxchar )
					{
					for (	col = l, colptr =base;
						col <= r;
						++col, ++colptr)
						{
						latch = *(colptr);

						EGA_OUT ( 8, *area );

						*(colptr)  = *( area++ );
						}

					}	/* end for each rows */
				}/* end loop for each color */

				/* reset EGA */
				EGA_OUT (1, EGA_ENABLE);
				EGA_OUT (0,0);			/* BLACK */
				EGA_OUT (8, 0xff);              /* all bits */


				break;
				} 	/* end switch (wgdriver) */


		/* end of put image */

		}
#endif	/* ifdef TEXTONLY - end of graphics mode wrestore */



	wheap_deaccess ( hptr, 0 );

	return;	/* wrestore */
	}

/*----------------------  end of WRESTORE.C -------------------------*/


