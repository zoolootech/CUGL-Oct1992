// STRPPFNT.CPP		contains String::findNot() 
//		routine finds first occurance in 'this' of any char not in string A
//		obeys String::caseSens
#include <stdlib.h>
#include <string.h>

#include "dblib.h"

int String::findNot ( char *a )
 	{
	int na = strlen (a);
	
	if ( a== NULL || na==0 ) return 0;
	
	int sn = n;
	char *ss = s;
	for ( int i=0; i<sn; ++i )
		{
		// examine each letter in String 'this',  for any char not in pattern a.
		if ( -1 == String::findchr(a, na, ss[i]) ) return i;
		}
	return -1;
	}			// end String::findNot()
