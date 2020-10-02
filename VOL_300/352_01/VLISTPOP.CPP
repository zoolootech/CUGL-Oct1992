// VLISTPOP.CPP 
//		code for Vlist::pop(), removes top item from vlist stack.
#include "dblib.h"

		void  Vlist::pop ( void )
			{
			void **vl = list;
			register int vn =n;
			
			if ( vn>0 )
				{
				n = --vn;
				free ( vl[vn] );
				vl [vn] = NULL;
				}
			return;	/* Vlist::pop() */
			}

	//----------------- end VLISTPOP.CPP -------------------