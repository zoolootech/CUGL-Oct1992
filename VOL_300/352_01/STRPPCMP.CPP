//	STRPPCMP.CPP - contains String::cmp()
//		This routine in the String class compares two strings.
//		Case sensitive or not, as set by String::caseSens.
//
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>

#include "dblib.h"

// NOTE: routine declared static in class {}. No 'this' ptr.		
int String::cmp (char *a, char *b)
		// String::cmp is a utility function for class String.
		// routine compares two strings 
		// checks for caseSens compares
		{
		if ( a==NULL )
			return ( b==NULL ) ? 0 : -1;
		else 
		if ( b== NULL ) 
			return +1;
		else
		return String::caseSens ? strcmp (a,b) : stricmp (a,b);
		}		// end String::cmp()
	