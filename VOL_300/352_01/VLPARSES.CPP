// VlistParseString () -  parse a String into a Vlist.
//		params: Vlist& v is list to hold output (=parsed string)
//				char* s is input String to parse ( or may use String& s )
//		returns: number of token words found.    
//		NOTE this code checks for 2 consecutive tokens, does not add blanks.
#include "dblib.h"

int VlistParseString ( Vlist& V, char *s, char *sepchars )
	{
	int count =0;
	if ( !( s == NULL || s[0] == 0 ) )
		{
		String *Sptr;
		String Str = s;
		while ( Str != NULL )
			{
			Sptr = Str.tokenize (sepchars);		// tokenize() returns a String*
			if ( (*Sptr).len() > 0 )	V.push ( (char*)(*Sptr) );		
			delete Sptr;						//... which must be deleted	
			++count;
			}
		}
	return count;
	}
//------------------- end of VLPARSES.CPP ------------------------ 