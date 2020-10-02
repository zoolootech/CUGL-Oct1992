// STRPPTRN.CPP  	contains String::translate()
// 			NOTE: construction of loop guarantees against NULL strings
//					(as long as the length field is 0 for them)
//					but does not guarantee against NULL translation strings.
#include <stdlib.h>
#include <string.h>
#include "dblib.h"

String& String::translate ( char *a, char *b )
	{
	int sn 	=n;
	char *ss=s;
	int  al = strlen (a);
	int i;
	_NORMALIZE (b, char*);
	while ( --sn >= 0 )			// run backwards through string.
		{
		if ( -1 != ( i= findchr ( a, al, ss[sn] ) ) )
			{
			//  found char from string a in position i within String this->s
			//  so replace it with equivalent char from b.		
			ss[sn] = b[i];		
			} 
		}
	return *this;	
	}		// end of String::translate()