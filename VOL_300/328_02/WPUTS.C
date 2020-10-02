/*! wputs
 *
 *
 *   put a string to video at the current cursor position
 *   uses wputstyle to control output.
 *
 */
#include  "wsys.h"






int wputs(char *s)
	{

	int count = 0;
	unsigned char save_cur;
	unsigned char more, inline_attr;


	_NORMALIZE(s);	/* model-dependent normalization */

	#ifdef __LARGE__
		if ( s==NULL ) return (0);
	#endif	/* __LARGE__ */
	#ifdef __COMPACT__
		if ( s==NULL ) return (0);
	#endif	/* __COMPACT__ */

	if ( wmode == 'T' )
		{
		save_cur = (w0->winflag) & WFL_CURSOR;
		if ( save_cur  )
			{
			wcursor (OFF);
			}
		}


	/* see if we're doing in-line attributes
	 *	use \a for inline attributes if:
	 *		the ALARM flag is off and the ATTR flag is on
	 */
	#define THREE_FLAGS 	(WPUTANSI | WPUTATTR | WPUTALARM)
	#define TWO_FLAGS	(WPUTANSI | WPUTATTR )


	inline_attr = ( ( w0-> winputstyle & THREE_FLAGS ) == TWO_FLAGS ) ?
			1: 0;

	for  ( count=0, more = 1  ; (*s) && more ; ++s, ++count )
		{
		if  ( inline_attr && (*s == '\a') && (*(s+1) != 0)  )
			{
			wsetattr ( *(++s) );
			}
		else
			{
			more = wputc (*s);
			}
		}


	if (wmode == 'T')
		{
		if ( save_cur )
			{
			wcursor (ON);
			}

		}


	return(count);

	}  /*end of wputs */



