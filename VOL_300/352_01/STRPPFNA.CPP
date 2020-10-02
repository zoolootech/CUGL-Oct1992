// STRPPFNA.CPP		contains String::findAny() 
//		routine finds first occurance in 'this' of any char of the string a
//		obeys String::caseSens
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>

#include "dblib.h"

int String::findAny ( char *a )
 	{
	if ( a==NULL ) return -1;
	
	int nb = strlen (a);
	int sn = n;
	char *ss = s;
	for ( int i=0; i<sn; ++i )
		{
		// examine each letter in String 'this' for any char in pattern a.
		if ( 0<= String::findchr(a, nb, ss[i]) ) return i;
		}
	return -1;
	}			// end String::findAny()
