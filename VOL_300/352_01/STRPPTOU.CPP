// STRPPTOU.CPP  - contains String::toUpper()
//	convert a string to upper case.
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>


#include "dblib.h"

void String::toUpper() 
	{ 
	int sn =n;
	char *ss =s;
	if ( ss != NULL ) 
		for (int i=0; i<sn; ++i)  ss[i] = toupper(ss[i]);
	}	// end String::toUpper()
