// STRPPINS.CPP  - contains String::insert()
//		routine to insert one string into another.
//
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>

#include "dblib.h"

String&  String::insert ( char *ins, int pos )
	// insert string ins into current string at position pos.
	// alters the existing string.
	{
	char *old_s;
	int old_n;
	old_n =n;
	old_s = s;		// hold onto original string contents.
	int ins_n = strlen(ins);
	if (!( ins_n==0 || ins== NULL || pos > old_n ))
		{	
		n = old_n + ins_n;	// new size.
		construct ( old_s );	// larger buffer with old string in it.
		memcpy ( s+pos, ins, ins_n );
		memcpy ( s+pos+ ins_n, old_s +pos, old_n -pos );
		free ( old_s );
		}
	return *this;
	}		// end of String::insert()

