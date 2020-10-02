/*! wputc
 *
 *
 *   put a char to video at the current cursor position
 *   uses wputstyle to control output.
 *
 */

#include  "wscreen.h"
#include  "wsys.h"






/* reduce ugliness of code */
#define style   (w0->winputstyle)






int wputc (char c)
	{

	int domore = 1;	/* set to 0 if fill window prematurely */

	char ansi_char;


	if (style & WPUTANSI)
		{
		ansi_char = c;
		}
	else    {
		ansi_char = 0;
		}



	switch(ansi_char)
	{
	case('\a'):   /*ring bell*/
		break;
	case('\b'): 	/* backspace */
		if (--(w0-> winx) <0 )
			if (w0-> winy >0) {
				/* go to last column, prior line
				 */
				w0-> winx =  w0->winxmax;
				--(w0-> winy);
				}
			else    /*ignore illegal backspace */
				w0-> winx = 0;
		break;
	case('\f'):   /*formfeed = clear page */
		wclear();
		break;
	case('\n'):   /*new line*/
		/* for new lines,
		 * need to scroll if at bottom of window and putstyle=scroll
		 * need to linefeed if putstyle = n2rn ( ie: /n ===> /r/n)
		 */
		if (    w0-> winy == w0->winymax )
			{
			if (style & WPUTSCROLL )
				{
				wscroll();
				}
			else 	{
				domore =0;
				}
			}
		else	{
			++(w0-> winy);
			if ( style & WPUTN2RN )
				{
				/* want to line feed after \n */
				(w0-> winx) =0;
				}
			}
		break;
	case('\r'):	/*carriage return*/
		(w0-> winx) = 0;
		break;
	case('\t'):	/*tab*/
		w0-> winx = wtabwidth* (1+ (w0-> winx)/wtabwidth) ;
		if (w0-> winx > w0-> winxmax )
			{
			w0-> winx = 0;
			}
		break;
	case('\v'):	/* vertical tab*/
		if (w0-> winy < w0-> winymax)
			{
			++(w0-> winy);
			}
		else
			{
			/* at last line of window */
			if (style & WPUTSCROLL )
				{
				wscroll();
				}
			else	{
				domore =0;
				}
			}
		break;
	default:
		/* printable character
		 */
		wputcabs ( w0->winleft + w0->winx,
				   w0->wintop  + w0->winy,
				   c, w0-> winattr,
				   WGOVERWRITE );


		if (++(w0->winx) > w0->winxmax )
			{
			/* move to first char of next line
			 */
			w0-> winx = 0;
			if ( ++(w0-> winy) > w0->winymax )
				{
				/* gone past last line
				 * move up to last line again.
				 */

				--(w0->winy);

				if ( style & WPUTSCROLL )
					{
					wscroll();
					}
				else    {
					/* no scrolling requested -
					 */
					domore =0;
					}
				}

			if ( ! ( style & WPUTWRAP ) )
				{
				/* crossed right edge of window,
				 * but user doesn't want to wrap around
				 */
				domore =0;
				w0->winx = w0->winxmax;
				}
			}
	}  /* end switch on *s for putting single char */




	/* move cursor and calculate new screen buffer ptrs
	 */
	wgoto (w0->winx, w0->winy);






	return (domore);
	}

/*-------------------- END WPUTC ------------------------*/