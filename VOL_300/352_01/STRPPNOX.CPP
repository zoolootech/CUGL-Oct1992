// STRPPNOX.CPP		contains String::noExtra() 
//		remove extra chars from string.  
//		chars are extra if they occur twice or more in a consecutive run.
//
#include <stdlib.h>
#include <string.h> 

#include "dblib.h"

 
String& String::noExtra (char *tok)
	{
	char *lag = s;		// ptr to remaining part of string
	if ( lag==NULL || tok==NULL ) return *this;

	char *lead=lag;		// ptr to exploring part of string
	int  ntok = strlen(tok);
	int  offset =0;
	char ctok = tok[0];	// first char from tok string.
	char c;				// character to examine.
	int  keep_one =0;	// flag that we are waiting for first char of a run

		
	for ( ; *lead; ++lead)
		{
		c = *lead;
		if ( -1 == String::findchr ( tok, ntok, c )  )
			{
			/* character at p is not contained in s2, 
			 *	so copy it to s and move offset for lagging ptr up one.
			 */
			lag[offset++] = c;
			keep_one = 1;	// keep next char from tok that we find.
			}
		else
			{
			if ( keep_one )
				{
				// first char in tok from a possible run
				lag[offset++] = ctok;	// replace it with first char from tok.	
				keep_one = 0;		 	// don't keep any more.
				} 
			keep_one = 0;		// don't keep any more.
			}
		}
		
	n = offset;
	lag[offset] =0;

	if ( offset == 0 )
		{
		// entire string was squeezed, 
		destruct();
		}

	return  *this; // String::noExtra() 
	}
