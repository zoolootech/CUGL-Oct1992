// STRPPTOL.CPP  - contains String::toLower()
//	convert a string to lower case.
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>


#include "dblib.h"


void String::toLower() 
	{ 
	int sn =n;
	char *ss =s;
	if ( ss != NULL )
		for (int i=0; i<sn; ++i)  ss[i] = tolower(ss[i]);
	}	// end String::toLower()
				
