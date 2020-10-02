//	LC.CPP -
//		this module contains the basic routines for class LinkClass.
// 
#include <stdlib.h>
#include <alloc.h>
#include <iostream.h>
#include "wtwg.h"

#include "dblib.h"
		
void LinkClass::initBase ()
				  {
				  if ( nx == NULL )
					{
					// make sure a previously empty node pts to itself.
					// ensures that static nodes are correctly initialized
					// ...when the first element in the list is added.
					nx = pv = this;
					}
				  };


LinkClass::LinkClass(LinkClass& base, void *data)
			{ 
			base.initBase();
			dt=data; 
			this->insertBelow(base);  // chain grows so oldest = base.first() 
			}; 
	
	
void LinkClass::unLink () 
	{
	pv->nx = nx;
	nx->pv = pv;
	};				// end LinkClass::unLink()
	
//----------------- end of LC.CPP --------------------
