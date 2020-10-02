/*  wpromptc
 *	dialog box routine.
 *
 *	displays a message in a window, waits for the user to press a key
 *	window is auto_sizing and auto_centering
 *
 *	parameters:
 *		title = ptr to title string. Ignored if null
 *		msg   = ptr to message for main body of window.
 *			may be multiple lines. Window sized to fit.
 *		response = NULL terminated set of ptr to response choices.
 *			The possible user responses are the first letters
 *			of each of the response choices. Lowercase letters
 *			are translated to UPPERCASE.
 *
 * The response line will be placed at the bottom
 *	of the window; the word Escape will be placed at left
 *
 *	returns: The keypressed by the user, as limitted by response chioces
 *
 *	ex: key=wpromptc("title", "How are you?", "Fine", "Lousy", NULL);
 *
 *		the only possible values returned by this call are 'F', 'L'
 *		    or ESCAPE.
 *
 */

#include "wsys.h"
#include <ctype.h>
#include <stdarg.h>


static char ESC_msg [] = "ESCAPE";
static char ANY_KEY [] = "Press any key";



int wpromptc(char *title, char *msg,  ...)
	{
	int l, t; 	/* position of window */
	int linecnt, width;     /* size of msg */
	char first_char;		/* first letter of a choice */
	int key;
	unsigned char color;

	int     any_response;		/* ON = responses set by caller */

	va_list response;
	int 	resplen,		/* length of a particular response */
		resprow, respcol;



	/* 'normalize' the pointers  (if the memory model requires it)
	 * so we can add to them without wrapping around
	 * segment boundaries
	 */
	_NORMALIZE (title);
	_NORMALIZE (msg);




	/* count lines in message  and size of largest line
	 */
	wstrrowcol ( msg, &linecnt, &width );

	linecnt += 6;	/* allow room for top & bottom margins plus choices*/
	width   += 2;

	width = max ( width, strlen (title) );

	/* allow room for all the responses -
	 *  loop through and count lengths of each one,
	 *  allow 3 bytes for spaces in between.
	 */
	resplen = sizeof (ESC_msg) +2;
	any_response = 0;

	for ( va_start (response, msg);
	      *((char**)response);
	      va_arg (response, char *)  )
		{
		resplen  +=  strlen (*((char**)response)) + 3;
		++any_response;
		}
	va_end (response);


	if ( any_response == 0 )
		{
		/* no responses were provided.
		 * Use default message...
		 */
		resplen = sizeof (ANY_KEY) + 2;
		}


	width = max ( width, resplen );

	/* get color of current screen
	 */
	color = wgetattr();


	/* window location
	 */
	wlocate ( &l, &t, width, linecnt );


	wopen (l, t, width, linecnt,
		color, DOUBLE_BORDER, BRIGHT | color, WSAVE2RAM);

	/* turn off scrolling and wrappping
	 */
	w0-> winputstyle &= (0xff - WPUTSCROLL - WPUTWRAP);

	if (title)
		{
		wtitle (title);
		}

	wgoto (0,1);
	wputs(msg);

	if ( any_response )
		{


		/* setup response buttons and create a string of choices.
		 */
		resprow = linecnt -3;

		wbutton_add (ESC_msg, 1, resprow, sizeof(ESC_msg),ESCAPE,
					WBTN_BOX);

		respcol = 3 + sizeof (ESC_msg);

		for ( va_start (response, msg);
		      *((char**)response);
		      va_arg (response, char *)  )
			{
			resplen = strlen (*((char**)response));

			first_char = toupper (**((char **)response));

			wbutton_add (*((char**)response),
					respcol, resprow, resplen+1,
					first_char, WBTN_BOX );


			respcol += resplen +3;		/* for next one */
			}
		va_end (response);


		do	{
			key = wgetc();
			if ( isascii (key) ) key = toupper (key);
			}
		while ( ! wbutton_test (key) );

		}
	else
		{
		/* no specific responses set by user, so
		 * place non-specific message and get a key.
		 * if key is MOUSE, wait for button release.
		 */
		wgoto (  (width- sizeof(ANY_KEY))/2, linecnt-2 );
		wputs (   ANY_KEY  );
		key = wgetc ();
		wmouse_flush ();
		}

	wclose();

	return (key);  /* wpromptc */
	} 
/*--------------------- END OF WPROMPTC.C ---------------------*/

