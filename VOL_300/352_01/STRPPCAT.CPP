// STRPPCAT.CPP - string concatenation routines.
//		module includes String::operator+= and String::operator+
//
#include <stdlib.h>
#include <string.h>
#include <alloc.h>

#include "wtwg.h"			// for wmalloc()
#include "dblib.h"

	
	
String& operator+=(String& a, String& b)
	{
	char *oldptr, *newptr;
	register int newlen, oldlen, sourcelen;

	sourcelen = b.n;
	oldptr = a.s;					// hold orig. string info.
	oldlen =a.n;
	newlen =oldlen +sourcelen;			// compute larger string size.
	newptr = (char *) wmalloc(newlen+1, "String");
	if ( oldlen >0 )  memcpy ( newptr, oldptr, oldlen );
	memcpy ( newptr+oldlen, b.s, sourcelen );
	newptr[newlen] = 0;
	if ( oldptr != NULL ) free (oldptr);
	a.s = newptr;
	a.n = newlen;
	return a;
	}

String operator+(String& a, String& b)
	{
	String tmp =a;
	tmp +=b; 		// use of operator += defined above
	return tmp; 
	}
	
String& operator+=(String& a, char *b)
	{
	String Sb = b;
	a+= Sb;
	return a;
	}
	
String  operator+(String& a, char *b)
	{
	String Sb = b;
	return (a+Sb);			
	}
			
String  operator+(char *a, String& b)
	{
	String Sa = a;
	return (Sa+b);			
	}
			
//--------------- end of STRPPCAT.CPP ----------------------	