// STRPPPAD.CPP - contains String::strpad()
//		this is a 'slave routine' called by String::pad().
//
//		routine pads string with extra blanks out to length set by n
//
#include <stdlib.h>
#include <string.h>

#include "dblib.h"

			

void String::strpad ( )
	// pad string with blanks if needed.	
	{
	int sn=n;
	char *ss=s;
	if ( ss==NULL ) return;
	for ( int i=strlen(ss); i<sn; ++i )  ss[i] =' ';
	}	// end String::strpad()
