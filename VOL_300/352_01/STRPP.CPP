// STRINGPP.CPP  - basic modules for C++ String class
//		This file contains 	String::construct()
//							String::Destruct()
//							String::~String()
//							String::String()
//							String::copy()
//

#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <iostream.h>
#include <ctype.h>

#include "wtwg.h"

#include "dblib.h"

char String::caseSens = OFF;


void String::construct ( char *p )	
	// local utility for construction - allocate memory, copy into, ensure \0
	{
	register int sn =n;
	if ( (sn >0) && (p != NULL) )
		{
		char *ptr = (char *)wmalloc (sn+1, "String");
		s=ptr;
		memcpy (ptr, p, sn);
		ptr[sn] = 0;
		}
	else  
		{
		s=NULL;
		n=0;
		} 
	}
void String::destruct ( ) 
	// destructor routine to free allocated memory, 
	// also called from routines that shorten the string
	// It's the latter use that prevents merely testing this->n.
	{
	char *ptr =s;
	if ( ptr!=NULL ) { free(ptr); s=NULL; };
	n = 0;
	}

void String::copy( int l, char *p )	
	// private utility for copy
	{
	if ( (l==0) || (p==NULL) )
		{
		destruct();
		}
	else
	if ( n!=l )			// strings of unequal length, reallocate.
		{
		destruct();
		n= l;
		construct (p);
		}
	else
		{		// Strings of equal length-	terminal NULL already set. 
		memcpy (s, p, l);
		}
	}	// end String::copy()

//--------------------- end STRINGPP.CPP -----------------------	
