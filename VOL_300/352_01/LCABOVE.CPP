// LCABOVE.CPP - contains LinkClass::InsertAbove()
//		routine for inserting a node above the 'this' node for linked lists.
//	
#include <stdlib.h>
#include <alloc.h>
#include <iostream.h>
#include "wtwg.h"

#include "dblib.h"
	
	
void LinkClass::insertAbove ( LinkClass &existing )
	{
	nx = existing.nx;
	pv = &existing;
	pv->nx = this;
	nx->pv = this; 
	};	// end LinkClass::insertAbove()
	
	
