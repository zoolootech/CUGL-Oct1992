// STRPPSUB.CPP  - contains String::substring()
//		routine to find a given string from within the 'this' string.
// 		creates a substring from position a for length b.
// 		if b not specified or less than 0, goes to end of string.
// 		NOTE: creates a new string, which must be explicitly deleted.
//		obeys String::caseSens
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>


#include "dblib.h"

String *String::substring(int a, int b)
	{
	String *tmp;
	
	tmp = new String;		// zero length for start.
	int sn = n;
	
	if ( a<0 )  { a = 0; }		// validate substring limits in bounds.
	if ( a>=sn ) { a = sn-1; }
	if ( b<0 )  { b = sn-a; }	// if no length set, go to end of string.

	tmp->n = b;		
	tmp->construct ( s+a );				// construct new string.
	tmp->strpad();						// pad with blanks.
	
	return tmp;
	}		// end String::substring()
