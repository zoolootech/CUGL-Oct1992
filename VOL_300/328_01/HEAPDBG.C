/* HEAPDBG.C
 *
 *	a smaller version of the wheap.c program.
 *  This version only allocates memory from DOS 'far' ram. Expanded memory 
 *	and disk virtual memory are ignored. The variable wheap_ramlimit is ignored
 *
 *	The reason for this is that wheap.c allocates a 64-k disk IO buffer,
 *	which can make use of the TurboC debugger or codeview impossible 
 *  (too much ram)
 *
 *  USE:	
 *	the calling routine follows this sequence (details ommitted):
 *		WHEAP *heap_ptr, char far *data_ptr;
 *
 *	     	heap_ptr = wheap_alloc ();              ... acquire heap storage
 *         	data_ptr = wheap_access( heap_ptr,...); ... move to RAM
 *                  ... do something using data_ptr
 *			wheap_deaccess ( heap_ptr, ...); ... release back to heap
 *			wheap_free     ( heap_ptr );     ... free from heap.
 *
 *	method: WHEAP block is a double linked list,
 *			most recent near heap_top
 *		items added/removed at any time.
 *
 */



#include <stdlib.h>
#ifndef __TURBOC__
	/* Microsoft C */
	#include <malloc.h>
#else
	/* Turbo C */
	#include <alloc.h>
#endif


#include <string.h>





#define WHEAP_GLOBALS

#include "wtwg.h"


#ifdef __OVERLAY__
	/* TURBO C VROOM - may not use near calls */
	#define NEAR_CALL
#else 
	/* for large model, slightly faster */
	#define NEAR_CALL near
#endif



/* Limitation on how much dos far ram can be used as heap.
 */
       unsigned long wheap_ramlimit = (256L*1024L);


static WHEAP 		*current_access = NULL;




#ifdef __TURBOC__
	/* TURBOC is able to generate forward address refs.
	 * so heap_top can be initialized properly
	 * but in Microsoft, need to do this in wheap_init
	 * because microsoft C can't handle the forward address reference.
	 */
static WHEAP  NEAR_CALL heap_bottom;


static WHEAP  NEAR_CALL heap_top =
	{
	NULL,       	/* linked list -- _next, _prev */
	&heap_bottom,

	WHEAP_MAX,	/* num bytes -in xms systems, # reqrd for dskbase */
	WHP_DSK,	/* flag */

	255,    	/* priority */

	NULL,		/* disk access buffer ptr (xms or ram) */

	NULL,		/* double list - dskhi, then dsklo */
	&heap_bottom,

	0,

	0,
	0,

	0,
	0		/* filler     */

	};

static WHEAP  NEAR_CALL heap_bottom =
	{
	&heap_top,
	NULL,

	0,
	WHP_DSK,

	255,

	NULL,

	&heap_top,
	NULL,

	0,		/* lowest disk marker is start of file */

	0,
	0,

	0,
	0

	};

#else
	/* Microsoft C version - can't initialize the double linked list
	 * properly, so must do it at run-time.
	 */

static WHEAP  NEAR_CALL heap_top =
	{
	NULL,           /* linked list -- _next, _prev */
	NULL,           /* WILL BE INITIALIZED AT RUN TIME */

	WHEAP_MAX,      /* num bytes -in xms systems, # reqrd for dskbase */
	WHP_DSK,        /* flag */

	255,            /* priority */

	NULL,           /* disk access buffer ptr (xms or ram) */

	NULL, 
	NULL,           /* MICROSOFT - initialize at runtime */

	0,

	0,
	0,

	0,
	0               /* filler     */

	};

static WHEAP  NEAR_CALL heap_bottom =
	{
	&heap_top,
	NULL,

	0,
	WHP_DSK,

	255,

	NULL,

	&heap_top,
	NULL,

	0,

	0,
	0,

	0,
	0

	};


#endif  /* Microsoft verswion */





	/*------ XMS subroutines -------*/
	/*  removed from small version */

	/*------ DSK subroutines -------*/
	/*  removed from small version */



	/*------ RAM subroutines -------*/

static int  NEAR_CALL ram_alloc    ( WHEAP *hptr );


	/*------ memory management routines --------------*/

static void NEAR_CALL unchain_heap ( WHEAP *hptr );





	/*---------Disk and Expanded routines - removed -------------*/





	/*------ RAM subroutines -------*/

static int  NEAR_CALL ram_alloc ( WHEAP *hptr )
	{
	void far *ptr;
	int       retcode;
	size_t    nbytes;

	nbytes = hptr-> whp_nb;

	ptr = wfarmalloc ( nbytes, NULL );

	if ( ptr )
		{
		hptr-> whp_ram  = ptr;
		retcode = 0;
		}
	else
	 	{
		retcode = 1;
		}

	return (retcode);	/* ram_alloc */
	}









	/*----------- integrated (whole heap) routines -------------*/



	/* note that wheap-init is called before other windows routines,
	 * so it must not call window routines outside this module...
	 */
void 	wheap_init ( void )
	{

	#ifndef __TURBOC__
		/* Microsoft C is unable to initialize the double linked
		 * list properly at compile-time,
		 * so it has to be done at run-time
		 */
		heap_top.whp_prev = &heap_bottom;
		heap_top.whp_dsklo = &heap_bottom;

	#endif  /* Microsoft */

	/* initialize expanded memory */
	/* removed from smaller version */
	
	
	
	/* initilize disk file for I/O, acquire 64k buffer */
	/* removed from smaller version */


	return;		/* wheap_init */
	}


WHEAP	*wheap_alloc ( size_t nbytes,  unsigned char priority, char *errmsg)
	{
	WHEAP *hptr;
	int   need;
	int   low_priority =1;

	/* 80 byte errmsg, 31 bytes used, 39 left for caller
	 */
	#define MSG_OFFSET 30
	#define MSG_REMAIN 50


	char msgbuff[81];
	strcpy ( msgbuff, "Out of heap memory, call from  " );

	hptr = wmalloc ( sizeof (WHEAP), "WHEAP" );

	memset ( hptr, 0, sizeof (WHEAP) );


	if ( nbytes > WHEAP_MAX || nbytes == 0 )
		{
		werror ('W', "heap request too large");
		}

	/* double linked list
	 */
	hptr-> whp_nb   	= nbytes;
	hptr-> whp_priority = priority;

	hptr-> whp_next		= &heap_top;
	hptr-> whp_prev 	= heap_top.whp_prev;

	(heap_top.whp_prev)->whp_next    = hptr;
	(heap_top.whp_prev)              = hptr;




	need =  ram_alloc ( hptr );
	hptr->  whp_flag = WHP_RAM;



	if ( need && errmsg )
		{
		memcpy(msgbuff+MSG_OFFSET, errmsg, MSG_REMAIN);
		msgbuff[sizeof(msgbuff)-1] = 0;

		werror ( 'W', msgbuff );
		}


	return ( hptr );	/* wheap_alloc */
	}




void far 	*wheap_access   ( WHEAP *hptr,  int readwrite )
	{

	void far *ptr;

	if ( current_access )
		{
		wheap_deaccess ( current_access, 0 );
		}



	ptr = hptr -> whp_ram;

	current_access = hptr;


	return ( ptr );	/* wheap_access */
	}



void 		 wheap_deaccess ( WHEAP *hptr,  int readwrite )
	{


	current_access = NULL;

	return;		/* wheap_deaccess */
	}


/* unchain_heap() - remove WHEAP object from linked list
 *	and free memory used to store WHEAP item.
 *
 */
static void  NEAR_CALL unchain_heap ( WHEAP *hptr )
	{
	WHEAP   *pr, *nx;

	pr = hptr-> whp_prev;
	nx = hptr-> whp_next;

	pr-> whp_next = nx;
	nx-> whp_prev = pr;


	free ( hptr );

	return;		/* unchain_heap */
	}





void		 wheap_free  ( WHEAP *hptr )
	{


	if ( current_access )
		{
		wheap_deaccess ( current_access, 0 );
		}



		farfree ( hptr-> whp_ram );


	unchain_heap ( hptr );

	return;		/* wheap_free */
	}



unsigned long	 wheap_avail    ( void )
	{

	return ( wheap_ramlimit );   /* wheap_avail */
	}



unsigned long	 wheap_availxms ( void )
	{

	return (0);				/* wheap_availxms */
	}




void		 wheap_swap2dsk ( WHEAP *hptr )
	{

	return;		/* wheap_swap2dsk */
	}


/* swap all WHEAP blocks of given priority to disk
 */
void		 wheap_swapall2dsk ( int priority )
	{
	return;		/* wheap_swapall2dsk */
	}






void far 	*wheap_unlink ( WHEAP *hptr )
	{
	void far *ptr;

	size_t	numbytes;


	wheap_access ( hptr, 1 );

	numbytes = hptr-> whp_nb;

	ptr = hptr->whp_ram;


	/* now take item hptr out of the heap lists
	 */
	unchain_heap ( hptr );


	return (ptr);	/* wheap_unlink */
	}






void		 wheap_freeall ( void )
	{
	WHEAP *hptr, *pending;



    /* point to first allocated one
	 */
	hptr = heap_bottom.whp_next;



	/* run up chain, removing elelments one at a time
	 */
	while ( &heap_top != hptr )
		{

		pending  = hptr-> whp_next;

		wheap_free ( hptr );

		hptr = pending;

		}	/* end while */


	return;		/* wheap_freeall */
	}




	/*------------------- end of HEAPDBG.C -------------------- */


