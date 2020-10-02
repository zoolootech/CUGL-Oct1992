/* WSCRLLBR.C - contains routines for vertical mouse-driven scrollbars.
 *
 * 		wscrollbar_add () - create a scrollbar
 *			PARAMS: key = a non-keyboard keyvalue (negative is best) for ID
 *						  	This value will be returned by wgetc()
 *							when the mouse clicks on the scrollbar image area.
 *					x	= x-location in window for scrollbar.
 *					y1, y2= top and bottom locations of scrollbar.
 *					range = unsigned int value giving virtual position of ptr
 *							when ptr=y1, virtual position is 0.
 *							when ptr=y2, virtual value is 'range'
 *					start = unsigned int = starting virtual value of scrollbar.
 *			RETURNS: WBUTTON *Bptr = ptr to the scrollbar data, used below.
 *
 *		wscrollbar_draw() - draw, or redraw scrollbar
 *			PARAMS:  WBUTTON *Bptr = pts to the scrollbar data.
 *			RETURNS: void.
 *
 * 		wscrollbar_scroll() - allow mouse to move scrollbar ptr onscreen.
 *			USE: when wgetc() returns the key that identifies the scrollbar,
 *				the mouse state is: LEFT BUTTON newly pressed, on scrollbar.
 *				Call wscrollbar_scroll() to track the mouse mvt and get new ptr
 *			PARAMS: WBUTTON *Bptr = ptr to scrollbar data area
 *									(obtained from wscrollbar_add() )
 *			RETURNS: unsigned int newval = new virtual position 
 *						0 <= newval <= range
 *							
 *		wscrollbar_reset() - provide new virtual position for the scrollbar 
 *							 and redraw the ptr onscreen.
 *			PARAMS: WBUTTON *Bptr = ptr to scrollbar data.
 *					unsigned int newval = new virtual value.
 *
 *	NOTES: no error checking is performed. Be careful of following:
 *			The scrollbar must be AT LEAST 4 bytes long (y2 > y1+3)
 *			The range must be > 2.
 *			The key assigned should not be producable on a PC/AT keyboard
 *				(prog. will lock if a keyboard press activates the scrollbar)
 *				(negative key values are guaranteed to work)
 *			
 *
 */
#include "wsys.h"

	/* ASCI values for drawing scrollbar */
#define  IMAGE_UP	'\x1E'
#define  IMAGE_DN	'\x1F'
#define  IMAGE_BAR	'\xB0'
#define  IMAGE_PTR	'\xB2'


static void W_NEAR draw_bar ( WBUTTON *Bptr );
static void W_NEAR draw_ptr ( WBUTTON *Bptr, char ptr_char );

static unsigned char W_NEAR get_y(unsigned char y1, unsigned char y2, 
								unsigned int val, unsigned int range );


WBUTTON *wscrollbar_add ( int key, 
			unsigned char ux, unsigned char uy1, unsigned char uy2,
			unsigned int range, unsigned int start )
	{
	WBUTTON *Bptr;
	
	/* allocate storage and place new button in linked list
	 */
	Bptr = wmalloc ( sizeof (WBUTTON), "wbutton_add" );
	Bptr->Bchain = w0->winbutton;
	w0->winbutton = Bptr;

	Bptr-> Bval  = key;
	Bptr-> Bstyle= WBTN_SCROLL + WBTN_ACTIVE;

	Bptr-> Bx    = ux;
	Bptr-> Bxend = ux+1;
	
	
	Bptr-> By    = uy1;
	Bptr-> Byend = uy2;
	
	Bptr-> Brange 	  = range;
	Bptr-> Bscrollval = start;

	draw_bar ( Bptr );

	return (Bptr);	/* wscrollbar_add()  */			
	}
			
void 			wscrollbar_draw   ( WBUTTON *Bptr )
	{
	draw_bar ( Bptr );
	return;
	}
	
void 			wscrollbar_reset  ( WBUTTON *Bptr, unsigned int newval )
	{
	
	draw_ptr ( Bptr, IMAGE_BAR );
	
	Bptr-> Bscrollval = newval;
	draw_ptr ( Bptr, IMAGE_PTR );
	
	return;		/* wscrollbar_reset () */
	}


/* convert virtual scroll value to onscreen position of ptr.
 * Note that the scrollbar image is 2 bytes short to allow UP/DN icons.
 */
static unsigned char W_NEAR get_y(unsigned char y1, unsigned char y2, 
								unsigned int val, unsigned int range )
	{
	register unsigned char y;
	
	y = y1+1 + ( ( val * (y2-y1-2) ) / range );
	
	return (y);		/* get_y */
	}
	
	
	/* draw the entire scrollbar
	 */
static void W_NEAR draw_bar ( WBUTTON *Bptr )
	{
	int n;
	int x, y1, y2;
	
	
	x  = Bptr-> Bx;
	y1 = Bptr-> By;
	y2 = Bptr-> Byend;	
	
	
	wgoto ( x, y1 );
	wputc ( IMAGE_UP ); 
	
	for ( n = y1+1; n<y2; ++n )
		{
		wgoto ( x,n );
		wputc ( IMAGE_BAR );
		}
	
	wgoto ( x, y2 );
	wputc ( IMAGE_DN ); 
	
	draw_ptr ( Bptr, IMAGE_PTR );
	
	return;		/* draw_bar */
	}



static void W_NEAR draw_ptr ( WBUTTON *Bptr, char ptr_char )
	{
	
	wgoto ( Bptr-> Bx, 
			get_y ( Bptr->By, Bptr->Byend, Bptr->Bscrollval,Bptr->Brange ) );

	wputc ( ptr_char );
	
	return;		/* draw_ptr */
	}

	
	/* wscroll_bar_scroll() -
	 * 	On entry to this routine, the mouse is assumed to be in a state of:
	 *		LEFT BUTTON down.
	 */
	
unsigned int 	wscrollbar_scroll ( WBUTTON *Bptr )
	{
	unsigned int val, range;
	unsigned char x, xbar, y1, y2, y, oldy;
	int redraw =0;
	unsigned char ylen;
	
		
	xbar 	= Bptr->Bx;
	y1   	= Bptr->By;		
	y2   	= Bptr->Byend;
	val 	= Bptr->Bscrollval; 
	range   = Bptr->Brange;
	ylen    = y2-y1 -2;		/* length of actual scrollbar */
	
	oldy    = get_y ( y1, y2, val, range );
	
	/* erase the current ptr 
	 */
	wgoto ( xbar, oldy );
	wputc ( IMAGE_BAR );
	
	range 	= Bptr->Brange;
	wmouse_turn (ON);
	do
		{


		x= wmouse.wms_x;
		y= wmouse.wms_y;
		
		if ( (x==xbar)  && (y > y1) && (y < y2) )
			{
			/* Inside actual bar (not counting the up/dn icons) 
			 */
			
			if ( oldy != y )
				{
				/* avoid garbage on EGA/VGA monitors by turning of mouse
				 * while drawing (if  in graphics mode) 
				 */
				wmouse_turn (OFF);
				
				val =  ( ( y-y1-1 ) * ( range ) ) / (ylen);

				wgoto ( xbar, oldy );
				wputc ( IMAGE_BAR );
				
				oldy = y;
				
				wgoto ( xbar, y );
				wputc ( IMAGE_PTR );
				
				wmouse_turn (ON);
				
				redraw =1;	/* see below. */
				}
			}
		wmouse_location ();
		}
		/* continue loop until left is released.
		 */
	while  ( (wmouse.wms_used & WMS_LEFT_RLS) == 0 ) ;

	wmouse.wms_internal = 0;

	wmouse_turn (OFF);

	/* redraw.
	 * The routine used to convert screen-->virtual 
	 * is not exactly the inverse of the routine that converts virtual->screen
	 *	
	 * Therefore if the mouse was used to drag the ptr, 
	 * the screen image of the ptr may be off by +-1 byte from 
	 *    where it's virtual value predicts.
	 * If needed, Remove it using screen co-ords,
	 *			 redraw based on virtual co-ords.
	 */
	if ( redraw )
		{
		wgoto ( xbar, oldy );
		wputc ( IMAGE_BAR );
		} 

	
	/* At this pt, the onscreen ptr is invisible.
	 */
	 
	 
	 
	/* Now see if either the up or the down icon was selected (release LEFT) 
	 */
	if ( (y == y1) && (val > 0    ) )
		{
		/* released mouse on the UP_IMAGE icon
		 */
		--val;
		}

	if ( (y == y2) && (val < range) )
		{
		/* released mouse on the DN_IMAGE icon
		 */
		++val;
		}


	Bptr->Bscrollval = val;

	/* Now draw the final image of the ptr icon.
	 */
	draw_ptr ( Bptr, IMAGE_PTR );


	return (val);		/* wscrollbar_scroll() */
	}



/*------------------------  END OF WSCRLLBR.C ---------------------------*/