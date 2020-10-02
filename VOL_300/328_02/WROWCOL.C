/*! wstrrowcol
 *
 * 	a 2-dimensional strlen() function, this routine computes the
 *	number of rows and columns needed to display a string.
 *
 *	Assumptions:    new lines are flagged with \n
 *                      Lines do not wrap around the window edge.
 *			(routine may be used to compute size of next window)
 *
 *	This routine will FAIL
 *		if the string contains embedded \t tab chars
 *              if the string is longer than the current window width
 *
 *
 *	parameters: text is ptr to a string.
 *		the number of rows and columns needed are placed in
 *		*rows, *cols.
 *	RETURN: void.
 */



#include "wsys.h"





void wstrrowcol (char *text, int *rows, int*cols )
	{
	int linecnt;      	/* #lines in text */
	int longest;		/* longest line in text */
	int len;


	char *ptr;
	char *last_ptr;

	/* 'normalize' the pointers  (if the memory model requires it)
	 * so we can add to them without wrapping around
	 * segment boundaries
	 */
	_NORMALIZE (text);




	/* count lines in text  and size of largest line */

	ptr = last_ptr = text;

	linecnt = longest = 0;

	while (NULL != (ptr = strchr (ptr+1, '\n')) )
		{
		++linecnt;

		if ( longest < (int)(ptr - last_ptr) )
			{
			/* longest line so far */
			longest = (int) (ptr - last_ptr);
			}
		last_ptr = ptr;
		}

	/*now look over the last line (or only line) for line length.
	*/
	len = strlen(last_ptr);
	if ( longest < len )
		{
		longest = len;
		}

	*rows = linecnt;
	*cols = longest;

	return;
	}  /*end of wstrrowcol */


