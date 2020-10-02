// LCBELOW.CPP -
// 		contains LinkClass::insertBelow()
//		routine for inserting a new node below the 'this' node.		
#include <stdlib.h>
#include <alloc.h>
#include <iostream.h>
#include "wtwg.h"

#include "dblib.h"
	void LinkClass::insertBelow ( LinkClass &existing )
	{
	pv = existing.pv;
	nx = &existing;
	pv->nx = this;
	nx->pv = this; 
	};	// end LinkClass::insertBelow()
	