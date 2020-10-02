// STRPPCHC.CPP - module contains String::chrcmp()
//		This routine compares two characters, testing for case sensitivity first
//
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>

#include "dblib.h"

// NOTE: routine declared static in class {}. No 'this' ptr.		
int String::chrcmp ( char a, char b )
	// compare two characters, testing for case sensitivity.
	{
	if ( ! String::caseSens )
		{
		a = toupper (a);
		b = toupper (b);
		}
	return a-b;
	}			// end String::chrcmp()

