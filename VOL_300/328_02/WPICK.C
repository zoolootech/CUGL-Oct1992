/* WPICKLIST.C
 *
 *	General-purpose menu.
 *
 *	displays a menu, which may be multiple pages (scrolling)
 *	gets user input.
 *
 *
 *
 *	PARAMETERS:
 *		char *title = title for menu.
 *		char *list[] = array of string pointers to choices.
 *				terminated by NULL
 *
 *	RETURNS:
 *		N, the number of the choice, ranges from 0 to last on list
 *		if user hit ESCAPE, N will indicate the terminal NULL
 *
 *	NOTE: in the large memory model, be sure you've NORMALIZED the list
 *		otherwise list[N] may be garbage.
 *
 */


#include "wsys.h"



#define MAX_WIDTH	20
#define MIN_WIDTH	5
#define MAX_N		10


static void W_NEAR draw_menu ( char **list, int nfirst, int nlast, int npick );


/* draw the  '#nnn/nnnn' at bottom */
static void W_NEAR draw_npick ( int width, int npick );



/* draw one line of the menu
 */
static void W_NEAR draw_one_line (char **list, int nfirst, int n,
		unsigned char attr );


int wpicklist ( char *title, char **list )
	{
	int     more   =1,
		width  =MIN_WIDTH,	/* menu choices, screen dimensions */
		n, w,		/* scratch */
		ntotal,  	/* total # of choices */
		nfirst =0,	/* number of first on-screen choice */
		nlast, 		/* number of last on-screen choice */
		nvisible,	/* number of choices onscreen  */
		npick  =0,	/* number of currently highlighted choice */
		newpick=0,	/* number of next choice */
		up_buttons =0,	/* are PageUp buttons being displayed ? */
		dn_buttons =0,	/* are PageDn ...			*/
		redraw    =0,	/* need to rewrite list of choices 	*/
		have_scrollbar =0;	/* have ascrollbar onscreen */


	WINDOW *bigW, *smallW;		/* onscreen windows */
	int l,t, ncols ;		/* window postions  */
	int extra =0;			/* extra columns on right ? */

	WBUTTON *scrollbar;		/* mouse-able scrollbar */

	int key;	/* user input */


	/* letter typing buffer for matching to strings.
	 * note that initializing type_len to MAX_WIDTH
	 * forces matching routine to initialize the buffer.
	 */
	int 	type_len = MAX_WIDTH;
	char 	typematch [ MAX_WIDTH+1 ];
	int     type_cnt;

	_NORMALIZE (list);


	/* count lines and find widest line 
	 */
	for ( ntotal = 0;  list[ntotal] != NULL; ++ntotal )
		{
		w = strlen ( list[ntotal] );

		width = max ( width, w );
		}

	if ( title )
		{
		width = max  (width, strlen (title) +2 );
		}



	width = min ( width, MAX_WIDTH ) +2;	/* extra for frames R */



	/* decide if menu is shorter than 1 page...
	 */
	if ( ntotal <= MAX_N )
		{
		nlast = ntotal;
		}
	else
		{
		nlast = MAX_N;
		extra = 4;	/* enable extra for right-side buttons */
		}
	nvisible = nlast;


	/* pick window position
	 */
	ncols = width + extra ;
	wlocate ( &l, &t, ncols, nlast );



	bigW = wopen ( l, t, ncols, nlast, wmenuattr,
			DOUBLE_BORDER, wmenuattr, 1 );
	w0-> winputstyle = (0xFF - WPUTWRAP - WPUTSCROLL );

		
	/* create scrollbar
	 */
	if ( nlast > 4 )
		{
		have_scrollbar =1;
		scrollbar = wscrollbar_add ( -1, width-1, 0, nlast-1, ntotal-1, 0 );
		}


	/* actual number of text columns is width -1 (for border)
	 */
	smallW = wopen ( l,t, width-1, nlast, wmenuattr, NO_BORDER, 0, 0);
	w0-> winputstyle = (0xFF - WPUTWRAP - WPUTSCROLL );






	wreopen ( bigW );
	if ( title )
		{
		wtitle ( title );
		}

	if ( extra )
		{
		wbutton_add ("END ", width,     nvisible-1, 5, END,    0);
		wbutton_add ("HOME", width,     0,          5, HOME,   0);
		}

	newpick = npick;
	redraw = ON;


	/* loop thru user input.
 	 */
	while ( more )
		{
		/*next line added for protection from wmsdrag() hotkey
		 *		mouse may move the bigW during a wgetc() call,
		 *		so may need to re-align positions of the 2 windows. 
		 */
		smallW->winleft = bigW->winleft; 	smallW->wintop  = bigW->wintop;
		wreopen (smallW);

		if ( redraw )
			{
			draw_menu ( list, nfirst, nlast, newpick );
			redraw = OFF;
			}
		else
		if ( npick != newpick )
			{
			/* only thing changed is line# picked
			 * the other lines haven't changed
			 * so redraw old line, draw new one highlighted.
			 */
			draw_one_line(list, nfirst, npick,  wmenuattr);
			draw_one_line(list, nfirst, newpick,wbuttonattr);
			}
		wreopen (bigW);

		if ( npick != newpick )
			{
			npick = newpick;
			if ( have_scrollbar )
				{
				wscrollbar_reset ( scrollbar, npick );
				}
			}



		if ( extra )
			{
			draw_npick ( width, npick ); /* '#nnn' */

			/* handle the 'dn' buttons, if need to change them
			 */
			if ( nlast != ntotal )
				{
				if ( ! dn_buttons )
					{
					/* not at bottom, so draw 'down' buttons
					 */
					wbutton_add ("PgDn", width,
					   nvisible-2,5,PAGE_DN, 0);
					wbutton_add ("Dn 1", width,
					   nvisible-3,5,CTRL_PAGE_DN,0);
					dn_buttons = ON;
					}
				}
			else
				{
				/* (nlast == ntotal )
				 */
				if ( dn_buttons )
					{
					/* if reached end of list,
					 * and dn_buttons are on,
					 * then need to erase them
					 */
					wbutton_delete ( PAGE_DN, 0 );
					wbutton_delete ( CTRL_PAGE_DN, 0 );
					dn_buttons = OFF;
					}
				}

			/* now the  'up' buttons, if need to change them
			 */
			if ( nfirst > 0 )
				{
				if ( ! up_buttons )
					{
					/* not at bottom, so draw 'down' buttons
					 */
					wbutton_add ("PgUp", width, 1,  5,
							PAGE_UP, 0 );
					wbutton_add ("Up 1", width, 2,  5,
							CTRL_PAGE_UP, 0 );
					up_buttons = ON;
					}
				}
			else
				{
				/* (nfirst == 0 )
				 */
				if ( up_buttons )
					{
					/* if reached top of list,
					 * and up_buttons are on,
					 * then need to erase them
					 */
					wbutton_delete ( PAGE_UP, 0 );
					wbutton_delete ( CTRL_PAGE_UP, 0 );
					up_buttons = OFF;
					}
				}
			}



		key = wgetc ();


		/* did user type value to match ?
		 */
		if ( isascii(key) && isprint(key) )
			{
			if ( type_len == MAX_WIDTH )
				{
				/* filled type match buffer, restart from 0
				 */
				memset (typematch, 0, sizeof (typematch) );
				type_len = 0;
				}
			/* insert typed char in match buffer
			 * & add one to length of buffer.
			 * start search from current position
			 */
			typematch [type_len++] = key;
			for (type_cnt =npick; type_cnt < ntotal; ++type_cnt)
				{
				if (0==memicmp
					(list[type_cnt],typematch, type_len))
					{
					newpick = type_cnt;
					redraw  = ON;
					nfirst  = max (0,newpick-1);
					nlast   = nfirst + nvisible;
					if ( nlast > ntotal)
						{
						nlast  = ntotal;
						nfirst = nlast - nvisible;
						}
					break;  /* break from for(...) */
					}
				}


			}	/* end of printable char match */
		else
		if ( type_len > 0 )
			{
			/* match buffer has characters in it,
			 * but user is no longer trying to match
			 */
			memset (typematch, 0, sizeof (typematch) );
			type_len = 0;
			}



		/* used mouse to make a selection on menu ?
		 */
		if (  ( key == MOUSE )
		   && ( wmouse.wms_inwindow )
		   && ( wmouse.wms_x < width )	        /* in a line l to r*/
		   && ( wmouse.wms_y < (nlast-nfirst) ) /* on a line t to b*/
		   )
		   {
		   n = nfirst + wmouse.wms_y;	/* menu line # chosen */
		   if ( n != npick )
			{
			newpick = n;

			}
		   else
			{
			/* double_click means accept */
			if ( wmouse.wms_used & WMS_LEFT_RLS )
				{
				key = ENTER;
				}
			}
		   }
		else
		if ( key == UP_ARROW )
			{
			/* no on_screen buttons for up arrow
			 * so have to handle it here
			 */
			--newpick;
			if ( newpick < 0 )
				{
				newpick =0;
				}

			if ( newpick < nfirst )
				{
				/* have moved offscreen */
				nfirst = newpick;
				nlast  = nfirst + nvisible;
				redraw = ON;
				}
			}
		else
		if ( key == DN_ARROW )
			{
			/* no on_screen buttons for dnup arrow
			 */
			++newpick;
			if ( newpick >= ntotal )
				{
				newpick = ntotal-1;
				}
			if ( newpick >= nlast )
				{
				/* have moved down offscreen */
				nlast  = newpick +1;
				nfirst = nlast-nvisible;
				redraw = ON;
				}

			}




		switch ( key )
			{
			/* NOTE ESCAPE falls through to ENTER,
			 *      there is no break
			 */
		case (ESCAPE):
			npick = ntotal;	/* offset to NULL string */
		case (ENTER):
			more = 0;
			break;

		case ( HOME ):
			nfirst = 0;
			nlast  = nvisible;
			newpick= 0;
			redraw = ON;
			break;

		case ( END ):
			nlast  = ntotal;
			nfirst = nlast - nvisible;
			newpick= nlast -1;
			redraw = ON;
			break;

		case ( PAGE_DN ):
			if ( dn_buttons )
				{
				/* dn_buttons are onscreen,
				 * implies valid choice
				 */
				nlast += nvisible;
				if ( nlast > ntotal )
					{
					nlast = ntotal;
					}
				nfirst  = nlast - nvisible;
				newpick = nfirst;
				redraw  = ON;
				}
			break;

		case ( PAGE_UP ):
			if ( up_buttons )
				{
				nfirst -= nvisible;
				if ( nfirst < 0 )
					{
					nfirst = 0;
					}
				nlast   = nfirst + nvisible;
				newpick = nlast -1;
				redraw  = ON;
				}
			break;




		case ( CTRL_PAGE_UP ):
			if ( up_buttons )
				{
				nfirst -= 1;
				if ( nfirst < 0 )
					{
					nfirst = 0;
					}
				nlast   = nfirst + nvisible;

				if ( npick  >= nlast )
					 {
					 newpick = nlast-1;
					 }
	
				redraw  = ON;
				}
			break;


		case ( CTRL_PAGE_DN ):
			if ( dn_buttons )
				{
				nlast += 1;
				if ( nlast >ntotal )
					{
					nlast = ntotal;
					}
				nfirst   = nlast - nvisible;
				if ( npick  < nfirst )
					 {
					 newpick = nfirst;
					 }
				redraw  = ON;
				}
			break;


		case ( -1 ):
			/* scrollbar was selected.
			 */
			newpick = wscrollbar_scroll ( scrollbar );
			if ( newpick < nfirst )
				{
				redraw  = ON;
				nfirst  = newpick;
				nlast   = nfirst + nvisible;
				}
			else
			if ( newpick >= nlast )
				{
				redraw = ON;
				nlast  = newpick +1;
				nfirst = nlast  - nvisible;
				}
			break;

			}	/* end switch on key */



		}	/* end while ... more */






	/* close the small window first, then the big window
	 */
	wreopen ( smallW );
	wabandon ();
	wclose ();
	
	return (npick);	/* wmenu */
	}



static void W_NEAR draw_npick ( int width, int npick )
	{
	wgoto   ( width, (w0->winymax)/2 );
	wprintf ( "#%-3.3i" , npick+1 );
	return;
	}



static void W_NEAR draw_menu ( char **list, int nfirst, int nlast, int npick )
	{
	int     n;
	unsigned char attr;

	for ( n= nfirst; n<nlast; ++n )
		{
		if ( n==npick )
			{
			attr = wbuttonattr;
			}
		else
			{
			attr = wmenuattr;
			}

		draw_one_line ( list, nfirst, n, attr );
		}

	return;
	}




/* draw one line of the menu */
static void W_NEAR draw_one_line (char **list, int nfirst, int n,
	unsigned char attr )
	{

	wgoto ( 0, n-nfirst );
	wsetattr ( attr );
	wclearline ();
	wputs ( list[n] );
	wsetattr (wmenuattr);

	return;
	}




/*--------------------------- end of WPICK.C -------------------------*/
