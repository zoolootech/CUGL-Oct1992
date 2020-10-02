/* WTITLE.C contatins wborderwrite()  ( called by macros wtitle and also wps )
 *
 * write a string on the border of the window
 *
 *	if place = 't' write on the top of the window
 *	if place = 'b' write on the bottom
 *
 *	if justify = r/c/l justify the string right/center/left
 *
 *	using the #defines wtitle and wfootnote or wps simplifies this
 *
 * 	If the string is NULL the previous border is restored.
 */
#include  "wscreen.h"
#include  "wsys.h"




void  	wborderwrite (char place, char justify, char *t)
	{
	#ifdef TEXTONLY
		unsigned char far *ptr;
	#endif	/* end TEXTONLY - variable definitions */

	int 	row, col, lastcol, offset;
	char  	boxchar;
	char    boxattr;

	int 	x;	/* x-position */


	_NORMALIZE (t);		/* model-dependent ptr normalization */

	if ( ! w0->winbox )
		{
		/* cant do it if no border
		*/
		return;
		}



	/* select which row to write the title in.
	 * move up/down one line to the border area
	 *  if a border is present
	 */
	row = w0->wintop-1;
	if (place == 'b')
		{
		row += w0->winymax+2;
		}
	col 	=        w0-> winleft +1;
	lastcol = col + (w0-> winxmax);


	/* attribute for text
	 */
	boxattr =       w0-> winboxcolor;


	/* calling with NULL argument means erase the current title
	 */
	if ( t == NULL )
		{
		/* what to restore
		 */
		boxchar = wbox[ w0-> winbox ].horiz;


		#ifdef TEXTONLY

			ptr = wpage_ram + 2*80*(row) + 2*(col);
			do
				{
				*(ptr++) = boxchar;
				*(ptr++) = boxattr;
				}
			while ( ++col <= lastcol );
			/* end text mode restore old border */
		#else
			/* graphics mode restore old border */
			do
				{
				wputcabs (col, row, boxchar, boxattr,
					WGOVERWRITE );
				}
			while ( ++col <= lastcol );
		#endif	/* ifndef TEXTONLY - end graphics restore border */

		return;
		} /*end if for restoring old border */



	/* calculate the x- position of the string
	 *	for left/center/right positions.
	 */
	x = strlen(t);

	if ( justify == 'c' )
		{
		offset =  (w0->winxmax -x)/2;
		}
	else
	if ( justify == 'r' )
		{
		offset =  (w0->winxmax -x);
		}
	else 	/* left justify the string */
		{
		offset = 0;
		}

	/* x position for text output
	 */
	col += offset;




	#ifdef TEXTONLY
		ptr = wpage_ram + 2*80*row + 2*( col ) ;
	#endif


	do	{
		#ifdef TEXTONLY
			*(ptr++) = *t;
			*(ptr++) = boxattr;
		#else
			/* graphics mode
			 */
			wputcabs ( col, row, *t, boxattr,
				WGOVERWRITE );
		#endif

		}
	while  ( *(++t)  &&  (++col <=lastcol) );

	return;
	} /*end wborderwrite */

