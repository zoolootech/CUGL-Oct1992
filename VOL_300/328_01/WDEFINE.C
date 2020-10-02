/*! wdefine
 *
 *
 *  open a window, save previous contents, draw borders
 *
 *
 *
 */
#include  "wscreen.h"
#include  "wsys.h"






WINDOW *wdefine (int x, int y,  int xmax,  int ymax,
		unsigned char color, int boxtype, unsigned char box_color )
	{
	WINDOW 	*Wnew;
	int 	extra;				/* extra lines for borders */
	int 	priorpage, priorcursor;		/*last winodw's state */

	int     xend, yend;


	extra = boxtype ? 1 : 0;




	/* note coords start counting at 0
	 * so adding number of columns would be off by 1
	 * ie, winxmax=9 for a 10 column wide window
	 */
	--xmax;
	--ymax;


	xend =  x + xmax;			/* right, bottom */
	yend =  y + ymax;




	/* validate input paramters */
	if   ( (x           < extra)
	    || (y           < extra)
	    || (xend +extra > wxabsmax)
	    || (yend +extra > wyabsmax)
	     )
		{
		werror ('W', "window out of bounds");
		}




	/* save page number of current (=old) window
	 * and, if graphics mode, save BGI info about graphics position.
	 */

	if ( w0 != NULL )
		{
		priorpage = w0->winpage;
		priorcursor = w0->winflag & WFL_CURSOR;

		#ifndef TEXTONLY
                        /* get the graphics driver current position
                         * in the old window.
                         */
			wherepxpy();

		#endif /* ! TEXTONLY */

		}
	else	{
		/* first call - no old window exists */
		priorpage  = 0;
		priorcursor= 1;
		}

	/* construct linked list of window save areas
	 *
	 */
	Wnew = (WINDOW *) wmalloc (sizeof( WINDOW ), "wopen");


	/* make the new area the active window
	 */
	Wnew-> winchain = w0;
	w0 = Wnew;







	/* initialize new window data
	 */
	w0-> winsave = NULL;
	w0-> winbutton = NULL;

	w0-> winleft		= x;
	w0-> winxmax 		= xmax;
	w0-> wintop 		= y;
	w0-> winymax 		= ymax;

	w0-> winx = w0-> winy	= 0;

	w0-> winbox      = boxtype;
	w0-> winboxcolor = box_color;
	w0-> winattr     = color;

	/* set default putstyle for new window
	 * ie, turn on everything except cursor
	 */
	w0->winflag     = 0;        /* cursor */
	w0->winputstyle = 0xff;






	/* setup new page # if requested
	 */


	if ( priorpage != wnextpage )
		{
		/* validate the page #, compute new display RAM addr,
		 * interface with graphics package.
		 *
		 */
		if ( wnextpage <0 )
			{
			/* too low, use page 0 */
			wnextpage = 0;
			}
		else
		if ( wnextpage > wlastpage )
			{
			/* too high, use highest valid number */
			wnextpage = wlastpage;
			}

		wpage_ram = wvideo_ram + wpage_size*wnextpage;


		}	/* end of changing displayed page */

	w0->winpage = wnextpage;



	#ifdef TEXTONLY
		if (priorcursor)
			{
			wcursor (OFF);
			}
	#else
		/* Set the graphics driver viewport to match the open window
		 */
                if (wmode == 'G' )
                          {
		          walign (priorpage);
			  }
		else	  {
			  /* mode = T */
			  if (priorcursor)
				{
				wcursor (OFF);
				}
                          }
	#endif	/* ! TEXTONLY */

	return (w0);		/* wdefine */
	}

