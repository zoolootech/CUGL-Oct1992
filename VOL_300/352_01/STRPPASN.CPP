// STRPPASN.CPP
//		String::assign ()  transfers ownership of data to the String
//				( allows constructing Strings from allocated buffers
//					... without having to 'strdup' or make extra copies. )
//
#include "dblib.h"

void String::assign ( char *data )
	{
	typedef char *ASSIGN_DATA_PTR;
	destruct ();
	if ( data != NULL )
		{
		_NORMALIZE( data, ASSIGN_DATA_PTR );
		s=data;
		n=strlen (data);
		}
	}
	
//----------------- end of STRPPASS.CPP -----------------------