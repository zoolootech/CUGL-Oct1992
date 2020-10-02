// STRPPFIX.CPP - contains String::String(int) = fixed length constructor
//
#include <stdlib.h>
#include <string.h>
#include "wtwg.h"
#include "dblib.h"

String::String (int nb)
	{
	char *ptr = (char *)wmalloc ( nb+1, "String" );
	memset ( ptr, ' ', nb );
	ptr[nb] = 0;
	s = ptr;
	n = nb;
	}
//--------------- end of String::String(int) ---------------------------