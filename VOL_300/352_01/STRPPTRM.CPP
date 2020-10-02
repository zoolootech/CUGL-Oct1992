/* STRPPTRM.CPP		contains String::trim() 
 *		string trim operation.
 */
#include <stdlib.h>
#include <string.h> 

#include "dblib.h"

 
String& String::trim (char *tok)
	{
	char *src =s;			// hold this->s  locally for efficiency.
	
	if (! ( tok==NULL || src==NULL ) )
		{
		int ns = n;
		int ntok = strlen(tok);
	
		while (--ns >= 0)		// in the loop ns is index to last char=strlen()-1
			{
			if ( String::findchr ( tok, ntok, src[ns] ) >=0  )
				{
				src[ns] = 0;
				}
			else
				{
				break;
				}
			}
		n= ns+1;				// add 1, now gives # bytes, index to term. NULL
	
		if ( ns== -1 )  destruct();		// free string if no bytes remain.
		}
	return  *this; /* strim() */


	}
