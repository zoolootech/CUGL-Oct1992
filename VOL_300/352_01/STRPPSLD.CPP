// STRPPSLD.CPP
//		contains String::slide.
//		routine to 'slide' portion of a string from right to left.
//		input string:    abcdefghi0
//			positions:   0123456789		n =9 letters in string.
//     cut ( 2, 5 ):     abghi0			position 6 moved right to position 3
//										new length n=5.
#include <stdlib.h>
#include <string.h>
#include "dblib.h"

void String::slide ( int start, int stop )
	{
	char *ss = s;   
	int  sn = n;
	if ( ss == NULL || sn ==0 ) return;
	
	memmove ( ss+start, ss+ stop+1, sn-stop );		// also moves terminal /0
	if ( 0 >= (n = sn - stop + start -1) )
		destruct();
	}
	
//------------------ end STRPPSLD.CPP --------------------