// STRPPCUT.CPP
//		contains String::cut.
//		routine to remove portion of a string from middle.
//		input string:    abcdefghi0
//			positions:   0123456789		n =9 letters in string.
//     cut ( 2, 5 ):     abghi0			position 6 moved right to position 3
//										new length n=5.
#include <stdlib.h>
#include <string.h>
#include "dblib.h"

String&  String::cut ( int start, int stop )
	{
	
	register int  sn = n;
	if ( stop == -1 ) stop = sn-1;						// excl. \0 at end.
	if (!( sn <= stop || stop < 0 || stop < start ) ) 	// bounds are OK
		{
		if  ( stop != start )		// if they're equal, no cut necessary
			{
			slide (start, stop );	
			}
		}
	return *this;
	}
	
//------------------ end STRPPCUT.CPP --------------------