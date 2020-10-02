// VLMATCH.CPP
// 		code to match  a String to a vlist.
#include "dblib.h"

int Vlist::match ( void *data )
	{
	int  vn = n;	
	for ( int i=0; i<vn; ++i )
		{
		if ( 0==Vlist::compare (data,i) )  break; 
		} 
	return i;				// Vlist::match()
	}
//--------------- end VLMATCH.CPP ------------------