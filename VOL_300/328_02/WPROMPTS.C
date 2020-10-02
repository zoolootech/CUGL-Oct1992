/* wprompts.c
 *
 *
 *	display a promp string, get a string in reply
 *
 */
#include "wsys.h"


#define BUTTON_LENGTH  30


int wprompts ( char *title, char *prompt, char *reply, int nbytes)
	{
	int lastkey;

	int l = 0, r = 10 ;   	/* left and right borders of message */
	int t;           	/* position of top of window... */
	int linecnt;      	/* #lines in message */
	int len;

	char *buffer;		/* hold working copy of reply */


	unsigned char color;

	int  x_gets, y_gets;	/* start positions of string to get */



	/* 'normalize' the pointers  (if the memory model requires it)
	 * so we can add to them without wrapping around
	 * segment boundaries
	 */
	_NORMALIZE (title);
	_NORMALIZE (prompt);



	buffer = wmalloc (nbytes, "wprompts");




	/* count lines in message  and size of largest line
	 * allow 1 blank line below title, 1 below prompt, 1 below instring
	 * and 1 at bottom for ESCAPE/ENTER/ORIGINAL buttons
	 */

	wstrrowcol (prompt, &linecnt, &r );
	r       += 2;
	linecnt += 4;


	/* now check length of title and button lines,
	 *  see if it's longer than the prompt
	 */
	len = strlen (title) +2;
	r = max ( r, len );

	r = max ( r, BUTTON_LENGTH );



	/* if nbytes is too long too fit on one line,
	 * use several lines
	 */
	if ( nbytes >= r  )
		{
		/* add number of lines needed to hold
		 * a long wrappping line
		 */
		linecnt += ( nbytes / r );
		}



	/* set location for message box
	 */
	r += 2;

	wlocate ( &l, &t, r, linecnt+2 );


	color = wgetattr();

	wopen (l, t, r, linecnt+2 /*allow for frame */,
		color, DOUBLE_BORDER, BRIGHT | color, WSAVE2RAM);



	if (title)
		wtitle (title);




	wbutton_add ( "ESCAPE",   1, linecnt, 7, ESCAPE,    0);
	wbutton_add ( "ENTER",   11, linecnt, 6, ENTER,     0);
	wbutton_add ( "ORIGINAL",19, linecnt, 9, CTRL('O'), 0);

	wgoto (0,1);
	wputs(prompt);

	x_gets = wherex ();
	y_gets = wherey ();

	memcpy ( buffer, reply, nbytes );

	do
		{
		wgoto ( x_gets, y_gets );
		lastkey = wgets ( nbytes, buffer, 0 );
		}
	while ( ! ( lastkey == ENTER || lastkey == ESCAPE ) );


	if ( lastkey == ENTER )
		{
		memcpy ( reply, buffer, nbytes );
		}

	free (buffer);


	wclose ();

	return (lastkey);
	}