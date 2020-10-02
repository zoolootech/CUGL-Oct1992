// VLISTREM.CPP - contains Vlist::Remove()
//		removes a data item from the Vlist.
//

#include "dblib.h"

	void  Vlist::Remove ( int k )
			// remove a specfied item from list
			{
			void **vl =list;
			int vn = n;
			
			if ( k >=0 && k < vn )	
				{
				free ( vl[k] );
				for ( register int i =k; i<vn; ++i )
					{
					vl[i] = vl[i+1];		// dont try to combine with ++i
					}
				}
			return;		/* Vlist::remove()*/
			}
//	end of VLISTREM.CPP			
