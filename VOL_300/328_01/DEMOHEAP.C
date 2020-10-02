/* demoheap.c
 *	this file shows how to allocate blocks on the heap,
 *	free some of them, allocate more later, 
 *	access them at different times, etc...
 */
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include <dos.h>

#include "wtwg.h"

#define farmemcpy(dest,src,num)  \
	movedata (FP_SEG(src), FP_OFF(src), FP_SEG(dest), FP_OFF(dest), num )


typedef WHEAP  *WHEAP_PTR;

main ()
	{
	#define HPMAX    50			/* number of heap items. */
	#define ELSZ	 100		/* bytes per item. */
	WHEAP_PTR  hlist[HPMAX];	/* array of ptrs to WHEAP elements */

	int n;
	int priority;
	char far *ptr;

	char buff[ELSZ];


	winit ('T' );
	
	/* The heap manager allocates first to expanded memory until that is full,
	 * then to far ram, and when that runs out, allocates to disk.
	 * 
	 * The amount of far ram used is limitted by the global variable
	 * wheap_ramlimit. 
	 * Setting wheap_ramlimit to 0 shuts off use of ram (only xms & disk used)
	 * Setting wheap_ramlimit to very large (1 MEG) allows use of all avail ram
	 * If you have data that MUST be stored in regular (far) ram,
	 * or in LARGE model want enough ram left over for important data, 
	 *		set wheap_limit large enough to hold that data.
	 *  
	 */
	 wheap_ramlimit = 1000000L; /* shuts off use of ram for demo prog. */
	

	wclear ();

	for ( n= 0; n<HPMAX; ++n )
		{
		priority = n%3 +1;	/* use priorities that rotate 1-2-3 */
		
		
		hlist[n] = wheap_alloc ( ELSZ, priority, "main" );
		
		wprintf("item #%3.3i. Stored %s. Remaining expanded memory = %0.7ul\n", 
				n, 	( hlist[n]->whp_flag == WHP_DSK )? "on disk": "in xram", 
				wheap_availxms() );

		ptr = wheap_access ( hlist[n] , 0);

		sprintf ( buff, "heap element #%i\n",n);

		farmemcpy ( ptr, buff, 1+strlen (buff) );


		wheap_deaccess ( hlist[n], 1 );

		if ( n==3 )
			{
			/* Demo how to move a heap item from RAM/XMS to disk
			 */
			wheap_swap2dsk ( hlist[n] );
			}
			

		if ( n==2  )
			{
			wheap_free ( hlist[n] );
			}

		}
	/* Demo how to swap all heap elements of a specified priority to disk
	 */
	wheap_swapall2dsk ( 2 );		/* priority 2 items get swapped */



	/* read the items back from the heap
	 */
	for ( n=0; n<HPMAX; ++n )
		{
		if (n !=2  )	/* number 2 was freed earlier */
			{

			ptr = wheap_access ( hlist[n], 1 );
			

			farmemcpy ( buff, ptr, ELSZ );

			wputs (buff);
			wheap_free ( hlist[n] );

			}
		}


	wputs ( "\nthats all" );
	
	
	/*	The disk file is automatically deleted 
	 *  and expanded memory returned to the system 
	 *  by the shutdown routine. 
	 */
	return (0);
	}