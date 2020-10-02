// STRPPTOK.CPP  String::tokenize()  parse original string into tokens.
//			RETURNS: ptr to newly constructed String.			
//					NOTE: caller has to delete each returned string
//						  or they will accumulate on the heap.
//
//			the chars from the token string are at the end of the new String
//			tokenize progressively decimates 'this' down to a \0.
//		example:
//		String  x="a+b-3";	String *y;
//				y = x.tokenize("+-");	Now x="b-3", y="a"
//				delete y;
//				y = x.tokenize("+-");	Now x="3";   y="b"
//				delete y;
//				y = x.tokenize("+-");	Now x=NULL,  y="3"
//				delete y;
//
//		if no tokens are found, the entire string is returned 
//			and 'this' is decimated.
//			
//		obeys String::caseSens.
//
#include <stdlib.h>
#include <string.h>

#include "dblib.h"

String *String::tokenize ( char *tok )
	{
	String *tmp;
	char *ss=s;  int   sn=n;
	
	int i = findAny (tok);		// findAny protects against NULL.
			
	if ( i == -1 )		// ie, no tokens found in parent string.
		{
		tmp = new String;
		tmp->s = ss;			// transfer entire string over.
		tmp->n = sn;			//      ie: this->s now owned by *tmp
		s=NULL;					// DONT let destructor free this->s !!!
		n=0;
		}
	else
		{						// token found at position i.
		tmp = substring ( 0, i );		// make new string with that part.
		slide ( 0, i );					// remove that part from original.
		}
	return tmp;
	};		// end String::tokenize()	

