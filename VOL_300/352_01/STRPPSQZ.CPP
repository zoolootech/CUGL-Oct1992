// STRPPSQZ.CPP		contains String::squeeze 
//		string squeeze. Any chars in s2 are removed from s1.
//		METHOD: s pts to source string.
//				p starts out at same place.
//				for every char to keep, *s = *p and both are incremented.
//				for every char to loose, p is incremented, s left alone.
//	
//
#include <stdlib.h>
#include <string.h> 

#include "dblib.h"

 
String& String::squeeze (char *tok )
	{
	char *lag = s;		// ptr to remaining part of string
	char *lead=lag;		// ptr to exploring part of string
	if ( ! (lag ==NULL || tok==NULL ) )
		{
		int  ntok = strlen(tok);
		char c;
		
		int  offset =0;
			
		for ( ; *lead; ++lead)
			{
			c = *lead;
			if ( -1 == String::findchr ( tok, ntok, c )  )
				{
				/* character at p is not contained in s2, 
				 *	so copy it to s and move offset on lagging  ptr up one.
				 */
				lag[offset++] = c;
				}
			}

		lag[offset] =0;
		n = offset;	
	
		if ( offset ==0 )
			{
			// entire string was squeezed, 
			destruct();
			}
		}	

	return  *this; /* strsqz() */
	}
/*-------------------- end of strsqz() ----------------------------- */