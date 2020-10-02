// STRPPFNS.CPP - contains String::findstr()
//		this module searches one char string for another contained within
//		similar to strstr() but obeys String::caseSens
//		and returns offset rather than ptr.
//
// NOTE: routine declared static in class {}. No 'this' ptr.		
//
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>

#include "dblib.h"

int String::findstr ( char *a, int na, char *b )
	// search for string b in string a. return offset of pattern
	{
	_NORMALIZE (a, char*);
	_NORMALIZE (b, char*);
	
	int nb = strlen (b);
	if (! ( a==NULL || b== NULL || nb== 0 ) )
		{
		int limit = na-nb+1;
	
		for ( int i=0; i<limit; ++i )
			{
			if ( 0==String::memcmp(a+i, b, nb) ) return i;
			}
		}
	return -1;	// not found.
	};			// end String::findstr()

