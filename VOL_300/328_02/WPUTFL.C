/* WPUTFL.C - fixed length string put.
 */
#include "wsys.h"

void wputfl ( char *s, int l )
	{
	char ch;
	char done =0;
	
	#ifdef __LARGE__
		if ( s==NULL ) s= "";	/* protect against ptr to NULL */
	#endif
	#ifdef __COMPACT__
		if ( s==NULL ) s= "";	/* protect against ptr to NULL */
	#endif
	#ifdef __HUGE__
		if ( s==NULL ) s= "";	/* protect against ptr to NULL */
	#endif
	
	while ( l-- )
		{
		if ( ! done )
			{
			ch = *(s++);		/* get value first, then incr. ptr */
			if ( ch == 0 )
				{
				done =1;
				ch = ' ';
				}
			}
		wputc ( ch );
		}
	
	return;		/* wputfl() */	
	}
/*---------------- WPUTFL.C -----------------*/