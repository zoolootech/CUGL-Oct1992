 // LCempty.cpp  contains code for testing for empty linked lists.
 // part of LinkClass:: routines.
 
#include <stdlib.h>
#include <alloc.h>
#include <iostream.h>
#include "wtwg.h"

#include "dblib.h"
	
int  LinkClass::isEmpty () 
		{
		return (nx==this || nx==NULL); 
		}
