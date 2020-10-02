// STRPPMCP.CPP - this module contains String::memcmp()
//		This routine compares two areas of memory 
//		exactly like ANSI C memcmp() but determines whether to use
//		case Sensitive compare or not bassed on whether String::caseSens is ON
//
#include <stdlib.h>
#include <string.h>

#include "dblib.h"
			
	
// NOTE: routine declared static in class {}. No 'this' ptr.		
int String::memcmp (char *a, char *b, int nb)
		// String::memcmp compares two memory areas
		// checking for caseSens comparison. Note scope resolution.
		{
		return String::caseSens ? ::memcmp(a,b,nb) : memicmp(a,b,nb);
		}		// end String::memcmp()
