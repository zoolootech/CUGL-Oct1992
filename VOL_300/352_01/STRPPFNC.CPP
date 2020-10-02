// STRPPFNC.CPP  contains String::findchr()
//		this routine searches a string for first occurance of a char
//		obeys String::caseSens
//		
// NOTE: routine declared static in class {}. No 'this' ptr.		
//
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>

#include "dblib.h"

int String::findchr ( char *a, int na, char c )
	// search for first char c in string a. return offset.
	{
	if ( a != NULL )
		{
		_NORMALIZE ( a, char* );
		for ( int i=0; i<na; ++i )
			{
			if ( 0==String::chrcmp (a[i], c) )	return i;
			}
		}
	return -1;
	};			// end String::findchr() 

