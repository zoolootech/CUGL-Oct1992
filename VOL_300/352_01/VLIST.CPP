// VLIST.CPP
//
//	defines class Vlist which is a variable list of ptrs.
//		ptrs to strings or other objects may be added to the list or removed
//			(operates like a stack)
//		The list can also be accessed like an array of ptrs, with NULL at end.
//
//	NOTE: when you place an object in Vlist, the list makes a copy of the object
//			you may remove the object by calling  list.remove(ptr);
//			This frees the lists copy. 
//
//			If you place a heap object on the list,
//				you are still responsible for cleaning up your copy.
//			This is diferent from container classes in TurboC++ classlib 
//
#include <stdlib.h>
#include <string.h>
#include <alloc.h>



#include "wtwg.h"		/* needed for wmalloc() */


#include "dblib.h"


		// define number of ptrs to allocate for each time.	
		#define VSTEP 25		
		
		Vlist::Vlist (void)
			{
			max = n = 0;
			list = NULL;
			}		
			
		void Vlist::clear (void)
			{
			int vn=n; 
			void **vl=list;
			for ( int i=0; i<vn; ++i )
				{
				free (vl[i]);
				}
			n=0;
			if ( max > 0 ) { max=0; free (vl); list =NULL; }
			}
		
		int Vlist::compare ( void* a, int bn )
			{
			return  ( String::caseSens ? strcmp  ( (char*)a,(char*)(list[bn])) 
									   : stricmp ( (char*)a,(char*)(list[bn]))
									   );
			}
		void  Vlist::copy ( void *a )
			{
			
			int len = (a==NULL) ? 0 : strlen ( (char*)a );
			void *ptr = wmalloc ( 1+len, "Vlist" );
			if ( len > 0 )  memcpy ( ptr, a, len );
			((char*)ptr)[len]=0;

			void **vl =list;
			register int  vn=n;
			vl[vn] = ptr;
			vl[++vn] = NULL;	
			n = vn;
			}
		
		
		void Vlist::push ( void* data )		
			{
			int vn = n;
			int vmax =max;
			
			if ( vn == vmax )
				{
				// need to reallocate.
				max += VSTEP;
				vmax = max;
				// NOTE: must alloc enough mem for list and for terminal NULL.
				list = (void**)wrealloc (list,(vmax+1)*sizeof(void*), "Vlist");
				list[vn] = NULL;
				}	// end of reallocation.
				
			Vlist::copy ( data );
			
			return;		/* Vlist::push() */
			}
		
			

///////////////// end of VLIST.CPP

