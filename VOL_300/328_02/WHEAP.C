/* wheap.c
 * 	memory management for handling large blocks of data.
 *	each allocated large block is controlled by an object of type WHEAP.
 *	a block may be stored on expanded mem, DOS RAM, or disk.
 *
 *	the calling routine follows this sequence (details ommitted):
 *		WHEAP *heap_ptr, char far *data_ptr;
 *
 *	     	heap_ptr = wheap_alloc ();
 *           	data_ptr = wheap_access( heap_ptr,...); ... move to RAM
 *                  ... do something using data_ptr
 *		wheap_deaccess ( heap_ptr, ...); ... release back to heap
 *		wheap_free     ( heap_ptr );     ... free from heap.
 *
 *	method: WHEAP block is a double linked list,
 *			most recent W_NEAR heap_top
 *		items added/removed at any time.
 *		at end of program, run through list to free xms and disk
 *
 *		disk items have a separate double linked list to
 *		allow finding 'holes' left by previous wheap_free()
 *
 *
 *
 */



 /*	TURBOC 	specific functions (non_ANSI):
  *		  open, read, write, close, lseek, mktemp
  *         	and definition that start with:
  *		  S_I..., O_...,
  * 	Microsoft has similar functions.
  *		  farmalloc, farfree  microsoft uses halloc()/hfree
  *			(taken care of in msc.h)
  */
#include "wsys.h"

#ifndef __TURBOC__
	#include <sys\types.h>
	#include <errno.h>
	#include <direct.h>
#else
	/* Turbo C */
	#include <dir.h>
#endif


#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys\stat.h>






#ifdef W_SMALLHEAP
	/* special option to allow debuggin heap functions
	 * if SMALLHEAP is not defined, then the largets heap element is
	 * 64K. This prohibits using a source-level debugger 
	 * defining SMALLHEAP restricts the largest heap element to 16k,
	 * useful only for text-mode testing.
	 */
	#undef WHEAP_MAX
	#define WHEAP_MAX  (16*1024)

#endif	/* W_SMALLHEAP */


/* Limitation on how much dos far ram can be used as heap.
 */
       unsigned long wheap_ramlimit = (256L*1024L);
static unsigned long wheap_ramused  = 0L;

/* expanded memory mamagenemt
 */
#define  XMS_PG		(16*1024)			/* size of expanded memory page  */
static void far 	*xmsbase = NULL;	/* base address of page frame */
static unsigned char xmspresent	= 0;


static char 		 dskname[13]	= {0};
static char 		 dskname_template[13] = "HEAP--XXXXXX";
static unsigned long 	 hiwater = 0L;	/* high file location yet written*/


static WHEAP 		*current_access = NULL;



	/* flage values for whd_typewrite
	 */
#define WHPTP_LO		'l'	/* lowest file ptr  (=dummy)  */
#define WHPTP_HI		'h'	/* hihgest file ptr (=dummy)  */
#define WHPTP_NOTYET	'n'	/* not yet written to disk    */
#define WHPTP_APPEND	'a'	/* append to end of file      */
#define WHPTP_UPDATE	'u'	/* update existing block      */




#ifdef __TURBOC__
	/* TURBOC is able to generate forward address refs.
	 * so heap_top can be initialized properly
	 * but in Microsoft, need to do this in wheap_init
	 * because microsoft C can't handle the forward address reference.
	 */
static WHEAP  W_NEAR heap_bottom;


static WHEAP  W_NEAR heap_top =
	{
	NULL,       	/* linked list -- _next, _prev */
	&heap_bottom,

	WHEAP_MAX,	/* num bytes -in xms systems, # reqrd for dskbase */
	WHP_DSK,	/* flag */

	255,    	/* priority */

	NULL,		/* disk access buffer ptr (xms or ram) */

	NULL,		/* double list - dskhi, then dsklo */
	&heap_bottom,

	ULONG_MAX,	/* disk position is highest possible */

	0,		/* xms handle - set to non-zero if using xms base */
	0,		/* xms npages */

	WHPTP_HI, 	/* flags this dsk element as the disk high marker */
	0		/* filler     */

	};

static WHEAP  W_NEAR heap_bottom =
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

	WHPTP_LO,
	0

	};

#else
	/* Microsoft C version - can't initialize the double linked list
	 * properly, so must do it at run-time.
	 */

static WHEAP  W_NEAR heap_top =
	{
	NULL,           /* linked list -- _next, _prev */
	NULL,           /* WILL BE INITIALIZED AT RUN TIME */

	WHEAP_MAX,      /* num bytes -in xms systems, # reqrd for dskbase */
	WHP_DSK,        /* flag */

	255,            /* priority */

	NULL,           /* disk access buffer ptr (xms or ram) */

	NULL,           /* double list - dskhi, then dsklo */
	NULL,           /* MICROSOFT - initialize at runtime */

	ULONG_MAX,      /* disk position is highest possible */

	0,              /* xms handle - set to non-zero if using xms base */
	0,              /* xms npages */

	WHPTP_HI,       /* flags this dsk element as the disk high marker */
	0               /* filler     */

	};

static WHEAP  W_NEAR heap_bottom =
	{
	&heap_top,
	NULL,

	0,
	WHP_DSK,

	255,

	NULL,

	&heap_top,
	NULL,

	0,              /* lowest disk marker is start of file */

	0,
	0,

	WHPTP_LO,
	0

	};


#endif  /* Microsoft verswion */





	/*------ XMS subroutines -------*/

static void 	  W_NEAR  xms_check    ( void );

static int  	  W_NEAR 	xms_alloc    ( WHEAP *hptr );

static void       W_NEAR 	xms_access   ( WHEAP *hptr );

static void 	  W_NEAR 	xms_free     ( WHEAP *hptr );


	/*------ DSK subroutines -------*/


static void       W_NEAR	dsk_findhole ( WHEAP *hptr );

static void       W_NEAR  dsk_access   ( WHEAP *hptr, int readwrite );

static void 	  W_NEAR	dsk_deaccess ( WHEAP *hptr, int readwrite );

static void 	  W_NEAR	dsk_free     ( WHEAP *hptr );

static void 	  W_NEAR 	dsk_write    ( WHEAP *hptr );

static void 	  W_NEAR 	dsk_read     ( WHEAP *hptr );



	/*------ RAM subroutines -------*/

static int  W_NEAR	ram_alloc    ( WHEAP *hptr );


	/*------ memory management routines --------------*/

static void W_NEAR	unchain_heap ( WHEAP *hptr );





	/*--------------------------------*/

	/* routines to allow accessing far memory in small model
	 */
#undef FAR_DATA_MODEL
#ifdef __LARGE__
	#define FAR_DATA_MODEL
#endif

#ifdef __COMPACT__
	#define FAR_DATA_MODEL
#endif

#ifdef __HUGE__
	#define FAR_DATA_MODEL
#endif

#ifdef FAR_DATA_MODEL
	#define farmemcmp(aa,bb,cc)  memcmp(aa,bb,cc)
	#define farwrite(aa, bb, cc) write (aa,bb,cc)
	#define farread(aa, bb, cc)  read  (aa,bb,cc)


#else

	/* model is small or medium - need a far * equivalent of
	 * write, read, memcmp, (note farmemcpy is defined in wsys.h)
	 */



	#define BUFSZ	512


	static int farmemcmp(char far *aa, char far *bb, int n);
	static int farmemcmp(char far *aa, char far *bb, int n)
		{
		int retcode =0;
		int count;
		NORMALIZE (aa);
		NORMALIZE (bb);



		for ( count = 0; count < n; ++count )
			{
			if ( aa[count] < bb[count] )
				{
				retcode = -1;
				break;
				}
			else
			if ( aa[count] > bb[count] )
				{
				retcode = +1;
				break;
				}

			}



		return (retcode);  	/* farmemcmp */
		}


	static int farwrite (int, void far *, size_t);
	static int farwrite
		(int handle, void far *distant_buff, size_t n)
		{
		char close_buff[BUFSZ];


		int  total =0;
		int  code;

		size_t 	write_size = BUFSZ;


		NORMALIZE(distant_buff);


		while ( n>0 )
			{

			if ( n > BUFSZ )
				{
				n -= BUFSZ;
				}
			else
				{
				write_size = n;
				n = 0;
				}

			farmemcpy ( close_buff, distant_buff, write_size );

			code = write ( handle, close_buff, write_size );

			if ( code == -1 )
				{
				/* error, force exit with -1 return
				 */
				total = -1;
				n =0;
				}
			else
				{
				total += code;
				}


			(char far *) distant_buff += BUFSZ;


			}

		return (total);	/* farwrite - no, not John Birch */
		}


	static int farread (int, void far *, size_t);
	static int farread (int filehandle, void far *distant_buff, size_t n)
		{
		char   close_buff[BUFSZ];

		int  total =0;
		int  code;

		size_t  read_size = BUFSZ;

		NORMALIZE(distant_buff);


		while ( n>0 )
			{

			if ( n > BUFSZ )
				{
				n -= BUFSZ;
				}
			else
				{
				read_size = n;
				n = 0;
				}


			code =read ( filehandle, close_buff, read_size );


			if ( code== -1 )
				{
				/* error - force exit & return -1*/
				n = 0;
				total = -1;
				}
			else
				{
				total += code;
				farmemcpy ( distant_buff, close_buff, read_size );
				}

			(char far *) distant_buff += BUFSZ;


			}

		return (total);	/* farwrite - no, not John Birch */

		}





#endif		/* end far functions for small model */




	/*--------------- XMS routines ------------------*/


	/* special note: these routines are called during winit()
	 * before remainder of window system initialized.
	 * Therefor, can't call any w..() funcitons that do i/o.
	 */




static void W_NEAR	xms_check (void)
	{

	char far *xms_name;
	unsigned int seg;
	unsigned char retcode;

	PSEUDOREGS

	/* get address of xms interrupt handler,
	 * check at that address to see if it's the real thing
	 */
	_AH = 0x35;	/* subfunction to get interrupt vector */
	_AL = 0x67;	/* XMS interrupt number */
	INTERRUPTX ( 0x21 ); /* call DOS for interrupt vector, uses seg regs */
	seg = _ES;

	xms_name = MK_FP ( seg , 0x0A );		/* offset 10 bytes into seg */

	/*look for name of EMM driver.
	 */
	xmspresent = farmemcmp ( xms_name, "EMMXXXX0", 8 ) ? 0 : 1;





	/* make sure it's functional
	 */
	if ( xmspresent )
		{
		_AH = 0x40;
		INTERRUPT ( 0x67 );
		if ( _AH  != 0 )
			{
			xmspresent = 0;	/* not OK */
			}
		}

	/* setup page frame address
	 */
	if ( xmspresent )
		{
		_AH = 0x41;
		INTERRUPT ( 0x67 );
		retcode = _AH;
		seg     = _BX;

		if ( retcode  != 0 )
			{
			xmspresent = 0;
			}
		else
			{
			xmsbase = MK_FP ( seg, 0);
			}

		}



	return;		/* xms_check */
	}












static int W_NEAR 	xms_alloc ( WHEAP *hptr )
	{
	int handle, npages, retcode;


	PSEUDOREGS


	if ( ! xmspresent )
		{
		return (1);
		}



	/* calc # of pages needed - round up to W_NEARest 16k
	 *	note casts to long, in case allocating > 3* WHEAP_MAX
	 */
	npages =(unsigned)
			( ((long)(hptr-> whp_nb) + (long)(XMS_PG -1) ) / XMS_PG );

	_BX = npages;
	_AH = 0x43;
	INTERRUPT ( 0x67 );
	retcode = _AH;
	handle  = _DX;


	if ( retcode == 0 )
		{
		hptr-> whp_handle = handle;
		hptr-> whp_npages = npages;
		hptr-> whp_ram    = xmsbase;
		}


	return (retcode);	/* 0 = allocated OK */
	}



/* xms_access ()
 * 	this routine maps the logical (xms) pages into physical (ram) pages
 *	the number of pages to map is given by the WHEAP object.
 *	the location of the physical pages is given by xmsbase
 */
static void W_NEAR 	xms_access   ( WHEAP *hptr )
	{
	int n, nmax, handle;


	PSEUDOREGS



	handle = hptr->whp_handle;
	nmax   = hptr->whp_npages;

	for ( n=0; n<nmax; ++n )
		{
		_DX = handle;
		_BX = n;
		_AL = n;

		_AH = 0x44;	/* function to map pages */
		INTERRUPT ( 0x67 );
		}


	return;		/* xms_map */
	}



static void W_NEAR 	xms_free ( WHEAP *hptr )
	{
	int handle;

	PSEUDOREGS



	handle = hptr->whp_handle;

	_DX = handle;
	_AH = 0x45;	/* release handle */
	INTERRUPT ( 0x67 );


	return;		/* xms_free */
	}










	/*------ DSK subroutines -------*/



static void  W_NEAR	dsk_findhole ( WHEAP *hptr )
	{
	WHEAP  *peg, *next_peg;

	unsigned long  hole, holesize, required;





	/* run up double linked list of allocated disk blocks
	 * looking for a 'hole' large enough to hold the block
	 */

	hole     =0;
	holesize =0;
	required = hptr-> whp_nb;

	next_peg = &heap_bottom;

	/* the loop terminates when an adequate hole has been found
	 * which may not occur until next_peg points to heap_top,
	 * which has a position artificially set to ULONG_MAX,
	 * so any size block will fit in
	 */
	do 	{
		peg      = next_peg;
		next_peg = peg-> whp_dskhi;


		/* new hole = first byte beyond current block
		 */
		hole = peg-> whp_position + peg-> whp_nb;

		holesize =  (next_peg-> whp_position) - hole;

		}
	while (  holesize < required );


	/* link the new block into the list of allocated disk areas
	 */
	hptr -> 	whp_dsklo = peg;
	hptr ->  	whp_dskhi = next_peg;

	peg->   	whp_dskhi = hptr;
	next_peg-> 	whp_dsklo = hptr;


	hptr-> 		whp_position = hole;

	hptr-> 		whp_typewrite = WHPTP_NOTYET;	/* not yet written */

	return;		/* dsk_findhole */
	}






static void W_NEAR   dsk_access   ( WHEAP *hptr, int readwrite )
	{

	hptr-> whp_ram = heap_top.whp_ram;

	if ( xmspresent )
		{
		/* using xms as base area for disk access...
		 * swap out prev. contents, swap in the disk buffer pages
		 */
		xms_access ( &heap_top );
		}


	if ( readwrite )
		{
		/* need to read data in from disk
		 */

		dsk_read ( hptr );
		}



	return;		/* dsk_access */
	}






static void  W_NEAR  dsk_deaccess ( WHEAP *hptr,  int readwrite )
	{

	if ( readwrite )
		{
		dsk_write (hptr);
		}

	hptr-> whp_ram = NULL;

	return;
	}



static void W_NEAR	dsk_free  ( WHEAP *hptr )
	{
	WHEAP *pr, *nx;


	/* remove this block from the list of disk ptrs
	 * this creates a 'hole' in the file,
	 * which can be reused later
	 */
	pr = hptr-> whp_dsklo;
	nx = hptr-> whp_dskhi;

	pr-> whp_dskhi  = nx;
	nx-> whp_dsklo = pr;

	return;		/* dsk_free */
	}




	#define  UPDATE_ACCESS	(O_RDWR | O_BINARY)

	#define  APPEND_ACCESS  (O_APPEND | O_RDWR | O_BINARY)

	#define  CREATE_ACCESS  (O_CREAT | O_BINARY)

	#define  CREATE_MODE	(S_IFREG | S_IREAD | S_IWRITE)

static void W_NEAR 	dsk_write    ( WHEAP *hptr )
	{

	unsigned int access;
	unsigned int mode;
	int filehandle;

	unsigned long last_byte;

	if ( hptr-> whp_typewrite  == WHPTP_NOTYET )
		{
		dsk_findhole (hptr);
		}


	last_byte =  hptr->whp_position  + hptr->whp_nb;



	/* need to open the file differently depending on
	 * whether this is first time ever,
	 *                 updating previous block
	 *		   or appending new block to file
	 */
	if ( hiwater == 0 )
		{
		/* file has not yet been created.
		 */
		memcpy ( dskname, dskname_template, sizeof (dskname) );
		mktemp ( dskname );

		mode   = CREATE_MODE;
		access = CREATE_ACCESS;

		filehandle =open ( dskname, access, mode);

		hiwater = last_byte;
		}
	else
		{
		if ( hiwater >= last_byte )
			{
			access =  UPDATE_ACCESS;
			}
		else
			{
			/* Block lies beyond current size of file.
			 * next line is in case start of block
			 * is within current file but end of block is beyond
			 */
			hptr-> whp_position = hiwater;

			access = APPEND_ACCESS;
			}

		if ( -1== (filehandle = open ( dskname, access ) ))
			{
			werror ('W', "HEAP memory -cant open to read");
			}

		if ( access == UPDATE_ACCESS )
			{

			if ( -1 ==
			     lseek(filehandle,hptr->whp_position,SEEK_SET) )
				{
				werror ('W' , "seek error in heap memory");
				}


			}


		}


	if ( filehandle == -1 )
		{
		werror ('W', "unable to open heap file on disk");
		}


	if ( -1== farwrite ( filehandle, hptr->whp_ram, hptr->whp_nb ) )
		{
		werror ('W', "unable to write to heap file");
		}


	close ( filehandle );

	hptr-> whp_typewrite = WHPTP_UPDATE;

	if ( hiwater < last_byte )
		{
		hiwater = last_byte;
		}

	return;		/* dsk_write */
	}







static void W_NEAR 	dsk_read     ( WHEAP *hptr )
	{
	int filehandle;

	if ( hptr-> whp_typewrite != WHPTP_UPDATE )
		{
		werror ('W', "attempting to access unsaved heap block");
		}


	if ( -1 == (filehandle = open ( dskname, O_RDONLY ) ) )
		{
		werror ('W', "unable to read from heap file");
		}


	if ( -1L ==lseek ( filehandle, hptr-> whp_position, SEEK_SET ) )
		{
		werror ('W', "unable to read from heap file");
		}






	if ( -1 ==farread ( filehandle, hptr->whp_ram, hptr-> whp_nb ) )
		{
		werror ('W', "unable to read from heap file");
		}

	close ( filehandle );


	return;		/* dsk_read */
	}






	/*------ RAM subroutines -------*/

static int  W_NEAR	ram_alloc ( WHEAP *hptr )
	{
	void far *ptr;
	int       retcode;
	size_t    nbytes;
	unsigned long ram_wanted;

	nbytes = hptr-> whp_nb;
	ram_wanted   = wheap_ramused + nbytes;

	if  ( ram_wanted > wheap_ramlimit )
		{
		/* don't allocate ram beyond the ram limit
		 */
		return (1);
		}

	ptr = wfarmalloc ( nbytes, NULL );

	if ( ptr )
		{
		hptr-> whp_ram  = ptr;
		wheap_ramused = ram_wanted;		
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
	int rambase = 1;

	#ifndef __TURBOC__
		/* Microsoft C is unable to initialize the double linked
		 * list properly at compile-time,
		 * so it has to be done at run-time
		 */
		heap_top.whp_prev = &heap_bottom;
		heap_top.whp_dsklo = &heap_bottom;

	#endif  /* Microsoft */

	/* see if xms support is present, get page frame address, etc...
	 */
	xms_check ();


	/* get a buffer for disk access...
	 * use the xms base page if enough xms is avail (need 64k)
	 */

	if ( xmspresent )
		{
		/* allocate 64k of xms
		 */
		rambase = xms_alloc ( &heap_top );
		if ( rambase == 0 )
			{
			heap_top.whp_ram  = xmsbase;
			}
		}

	if ( rambase == 1 )
		{
		/* must use ram for disk-based heap data...
		 */
		xmspresent = 0;		/* too small to bother */

		/* can't use wfarmalloc(),  windows not initialized.
		 */
		heap_top.whp_ram = farmalloc ( WHEAP_MAX );
		if ( ! heap_top.whp_ram )
			{
			exit (ENOMEM);
			}
		NORMALIZE ( heap_top.whp_ram );

		}


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
	hptr-> whp_priority     = priority;

	hptr-> whp_next		= &heap_top;
	hptr-> whp_prev 	= heap_top.whp_prev;

	(heap_top.whp_prev)->whp_next    = hptr;
	(heap_top.whp_prev)              = hptr;





	if ( priority > 0 )
		{
		while (    ( nbytes > wheap_avail () )
			&& ( low_priority < priority )  )

			{
			/* need more ram than is presently avail,
			 * so swap lower priority objects to disk
			 * until either enough ram or all objects swapped
			 */
			wheap_swapall2dsk ( low_priority );
			++low_priority;
			}


		need = xms_alloc ( hptr );
		hptr-> whp_flag = WHP_XMS;


		if ( need )
			{
			need =  ram_alloc ( hptr );
			hptr->  whp_flag = WHP_RAM;
			}

		}


	if ( need  && (priority < 255) )
		{
		/* unable to allocate from xms or ram,
		 * and not highest priority item,
		 * so allocate from disk -
		 * all work here is done at time of saving data.
		 */
		hptr->  whp_flag      = WHP_DSK;
		hptr -> whp_typewrite = WHPTP_NOTYET;
		need = 0;
		}


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


	switch (  hptr-> whp_flag )
		{
	case ( WHP_XMS ):
		xms_access ( hptr );
		break;

	case ( WHP_DSK ):
		dsk_access ( hptr, readwrite );
		break;

		}

	ptr = hptr -> whp_ram;

	current_access = hptr;


	return ( ptr );	/* wheap_access */
	}



void 		 wheap_deaccess ( WHEAP *hptr,  int readwrite )
	{

	/* expanded memory and ram don't need to be de-accessed
	 */

	if ( hptr-> whp_flag  == WHP_DSK )
		{
		dsk_deaccess (hptr, readwrite);
		}


	current_access = NULL;

	return;		/* wheap_deaccess */
	}


/* unchain_heap() - remove WHEAP object from linked list
 *	and free memory used to store WHEAP item.
 *
 */
static void  W_NEAR  unchain_heap ( WHEAP *hptr )
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



	switch (  hptr-> whp_flag )
		{
	case ( WHP_XMS ):
		xms_free ( hptr );
		break;

	case ( WHP_RAM ):
		farfree ( hptr-> whp_ram );
		wheap_ramused -= (hptr-> whp_nb);
		break;

	case ( WHP_DSK ):
		dsk_free ( hptr );
		break;

		}

	unchain_heap ( hptr );

	return;		/* wheap_free */
	}



unsigned long	 wheap_avail    ( void )
	{
	unsigned long avail;

	/* note problems associated with subtracting unsigned long -
	 * can get large positive when what you want is negative.
	 */
	avail = ( wheap_ramlimit > wheap_ramused ) ? 
					(wheap_ramlimit - wheap_ramused) 
				/* else */
					: 0;

	return ( avail + wheap_availxms() );   /* wheap_avail */
	}



unsigned long	 wheap_availxms ( void )
	{
	unsigned long availxms = 0;
	unsigned int  npages;


	PSEUDOREGS

	if ( xmspresent )
		{

		_AH = 0x42;
		INTERRUPT ( 0x67 );
		npages = _BX;	/* number of unallocated pages */

		/* _DX gives total number of XMS pages in system
		 */

		availxms = ( ( unsigned long ) npages ) * XMS_PG;
		}

	return (availxms);				/* wheap_availxms */
	}




void		 wheap_swap2dsk ( WHEAP *hptr )
	{
	char oldflag;

	oldflag = hptr-> whp_flag;



	if ( oldflag == WHP_DSK )
		{
		return;		/* already is disk */
		}

	wheap_access ( hptr, 0 );


	/* now setup values in WHEAP block
	 * to make it llok like an nsaved disk block element, and then save.
	 */
	hptr-> whp_flag      = WHP_DSK;
	hptr-> whp_typewrite = WHPTP_NOTYET;

	dsk_write ( hptr );



	/* see if any ram needs to be free, or xms pages released
	 */
	if ( oldflag == WHP_RAM )
		{
		farfree ( hptr-> whp_ram );
		hptr-> whp_ram = NULL;
		}
	else
		{
		/* must be xms, release handle.
		 */
		xms_free ( hptr );
		}


	return;		/* wheap_swap2dsk */
	}


/* swap all WHEAP blocks of given priority to disk
 */
void		 wheap_swapall2dsk ( int priority )
	{
	WHEAP *hptr;

        hptr =heap_bottom.whp_next;
	while ( &heap_top != hptr )
		{
		if  ( hptr-> whp_priority == priority )
			{
			wheap_swap2dsk ( hptr );
			}
		hptr =hptr->whp_next;
		}

	return;		/* wheap_swapall2dsk */
	}






void far 	*wheap_unlink ( WHEAP *hptr )
	{
	void far *ptr;

	size_t	numbytes;


	wheap_access ( hptr, 1 );

	numbytes = hptr-> whp_nb;

	ptr = hptr->whp_ram;


	if ( xmspresent && (hptr->whp_flag != WHP_RAM) )
		{
		/* XMS items are kept in the XMS page frame
		 * and need to be copied to FAR memory
		 *
		 * DSK items are accessed by reading into XMS page,
		 * (but not if xms is not present)
		 * and need to be mapped into FAR memory
		 */
		ptr = wfarmalloc ( numbytes, "HEAP" );

		farmemcpy ( ptr, hptr->whp_ram, numbytes );


		}

	switch ( hptr-> whp_flag )
		{
	case ( WHP_DSK ):
		dsk_free (hptr);
		break;
	case ( WHP_XMS ):
		xms_free ( hptr );
		break;
		}


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

	/* if needed, erase the disk file
	 * (and clean up for another pass)
	 */
	if ( hiwater >0 )
		{
		/* disk storage has been used, clean up disk and internals
		 */
		remove ( dskname );
		hiwater = 0;

		}


	if ( xmspresent )
		{
		/* we used xms as base memory for disk access...
		 * need to free that handle, too.
		 */
		xms_free ( &heap_top );
		}



	return;		/* wheap_freeall */
	}





	/*------------------- end of WHEAP.C -------------------- */


