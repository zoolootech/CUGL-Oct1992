/*! wgetc.c        window-oriented  get keyboard/mouse, process as needed.
 *
 *	get a key from the keyboard or from the mouse.
 *	if (timeout), black out the screen (blackout)
 *
 *	If a user-defined keytrap routine is in place, call it.
 *		repeat getting keys until the user_defined routine is done.
 *
 *
 *
 *					@ David Blum, 1986, 1989
 */
#include "wsys.h"




/* timer function for 3-minute wait.
 */
static clock_t 		W_NEAR get_limit(void);

/* screen blackout after 3 minute wait.
 */
static void 		W_NEAR blackout ( void);

/* check_button() tests mouse use to see if it matches an active button.
 * 		PARAMETERS: whether to match BUTTONS or SCROLLBARS.
 */
static int W_NEAR check_button ( unsigned char button_test );

/* wait_flush() -
 * small routine to wait for a keypress and then flush that key. 
 */
static void W_NEAR wait_flush (void);





/*! wgetc - get a single character
 *
 * caution: turning mouse on and off in graphics mode
 *		 prevents garbage on screen,
 *		 but you have to make sure that the calls stay balanced
 *			( or the mouse driver will go bonkers)
 *		 so I adopted these conventions for wgetc() routines:
 *			1) turn mouse ON  at entry to wgetc()
 *			2) turn mouse OFF afetr key is read.
 *			3) no 'hooks' to external routines in between
 *
 */
int wgetc(void)
	{

	int key;

	clock_t time_limit, now_time;



	do
		{

		time_limit = get_limit();


		wmouse_turn (ON);



		/* is a key ready to get? */
		while ( ! wready() )
			{
			now_time = clock();
			if  ( now_time > time_limit )
				{
				blackout ( );
				time_limit = get_limit();
				}
			}  /*end while !kbhit */


		key = wreadc ();


		wmouse_turn (OFF);



		if ( wkeytrap != NULL )
			{
			key = (*wkeytrap)(key);
			}


		} /* end of do... */

	while ( key == 0 );

	return (key);	/* wgetc()  */
	}



/*! wready ()
 *	tests to see if a key has been hit or if mouse has been used.
 *
 *
 *
 *
 */





int wready(void)
	{


	int retcode;

	PSEUDOREGS;



	if ( wunget_buffer )
		{
		retcode = 1;
		}

	else
	if ( wpipein )
		{
		retcode = 1;
		}
	else
		{
		wmouse_location ();
		if ( wmouse.wms_internal )
			{
			/* mouse was used */
			retcode = 1;
			}

		else
			{
			/* mouse not pressed --- check keyboard
			 */

			retcode = wready_kbd();

			}
		}


	return ( retcode ); 	/* wready */

	}












/*! wreadc()
 *
 *	read unget buffer and/or redirection sources and/or mouse or kbd.
 *
 *      this function is like wgetc() but it does not do:
 *		1) screen blackout if waiting for keypress.
 *		2) mouse on/off (prevent garbage appearing on grpahics screen
 *              3) call keyboard trap routine
 *		4) repeat operation until an acceptable keypress is made.
 *
 *	NOTE 'pipes' are processed here:
 *		unget buffer takes precedence over pipes.
 *		keys which were ungot don't get passed on to any output pipes
 *
 *
 */
int wreadc(void)
	{
	int key=0;
	register int temp;

	if ( wunget_buffer )
		{
		/* if a key is waiting in the unget buffer */
		key = wunget_buffer;
		wunget_buffer = 0;
		}
	else
		{
		if ( wpipein )
			{
			key = wpipein();
			}
		else
			{
			wmouse_location();
			if ( wmouse.wms_used )
				{
				/* tell the user that the mouse was used,
				 * and reset the interanl use marker
				 * so that next call won't return MOUSE
				 * unless mouse has actually been used.
				 */
				wmouse.wms_internal = 0;

				/* Map mouse clicks on buttons to appropriate key values
				 * Map right button to ESCAPE
				 * all other mouse use is WMOUSE.
				 */
				key = MOUSE;
				if ( wmouse.wms_used & WMS_RIGHT_PRS )
					{
					key = ESCAPE;
					}
				else
				if ( wmouse.wms_used & WMS_LEFT_RLS )
					{
					if ( 0!=( temp= check_button(WBTN_BUTTON+WBTN_ACTIVE) ) )
						{
						key = temp;
						}
					}	/* end if (LEFT_RLS) */
				else
				if ( wmouse.wms_used & WMS_LEFT_PRS )
					{
					if ( 0!=( temp= check_button(WBTN_SCROLL+WBTN_ACTIVE) ) )
						{
						/* look for scrollbars if new button press.
						 */
						key = temp;
						}
					}	/* end if (LEFT_PRS) */
				}	/* end if (mouse used) */
			else
				{
				key = wread_kbd();
				}
			}            /* end of getting MOUSE/kbd combo */

		if ( wpipeout )
			{
			wpipeout (key);
			}

		}

	return (key);      	/* wreadc() */
	}
	

static int W_NEAR check_button ( unsigned char button_test )
	{
	int key = 0;
	WBUTTON *Bptr;

	/* look over list of buttons
	 * if mouse is 'inside' any of them, 
	 * set key value to button value.
	 */
	for (Bptr = w0->winbutton; 	Bptr; Bptr = Bptr->Bchain )
		{
		/*  both the type of button and the ACTIVE flag have to be on.
		 *  requires an exact match to all specified bits.
		 *
		 *  Also note the testing to see if mouse is inside button area. 
		 */
		if (  (  ( (Bptr->Bstyle) & button_test )  == button_test ) 
			     && (wmouse.wms_x >= Bptr->Bx   )
				 &&	(wmouse.wms_x <  Bptr->Bxend)
				 && (wmouse.wms_y >= Bptr->By   )
				 && (wmouse.wms_y <= Bptr->Byend)
				)
				{
				key = Bptr->Bval;	/* set key = button */
				break;				/* quit for() loop  */
				}	/* end mouse inside button border */
		}	/* end looking for Buttons */
		
		return (key);	/* check_button */
		}



static clock_t 	W_NEAR get_limit (void)
	{
	clock_t t;

	/* NOTE: in Microsoft C the CLK_TCK is 1000, 
	 * so if the timeout limit is > 32, you'll get integer arithmetic errors
	 */
	#if WITMEOUT_LIMIT > 32
		#ifndef __TURBOC__
			#error WGETC-- must recode using long arithmetic 
		#endif
	#endif		/* TIMEOUT_LIMIT >32 */
	
	
	t = clock() + WTIMEOUT_LIMIT * CLK_TCK;

	return (t);

	}








/*! blackout  - screen saver routine.
 *  black out the screen, wait for a keypress, restore the screen.
 *
 *  METHOD:
 *	Hercules: hercules control port at 0x3B8 contains:
 *		bit 7 (=hi, 0x80) = page assignment 0=0, 1=1
 *		bit 5 ( 0x20 )    = blink enable (always on).
 *                  3 ( 0x08 )    = video enable/disable
 *		    1 ( 0x02 )    = mode  ( 1= graphics )
 *		    0 ( 0x01 )    = hi-res control (always on)
 *	        unfortunately the port is write-only, so
 * 		need to construct appropriate bit values before setting.
 *	EGA/VGA:
 *		set color palette 
 */
#define  HCONTROL 0x21	/* default value for hercules control por 0x3B8 */



static void W_NEAR blackout (void)
	{
	char herc_byte;
	unsigned char save_palette[17];
	unsigned char black_palette[17];


	PSEUDOREGS

	/* black the screen
	 */
	switch (wmonitor)
		{
	case ('H'):
	case ('M'):
		/* build control byte to turn off video
		 * without affecting graphics mode, blink, or page
		 */
		herc_byte = HCONTROL;
		if ( wmode == 'G' )	  herc_byte |= 0x02;
		if ( w0-> winpage == 1 )  herc_byte |= 0x80;
		outp ( 0x3B8, herc_byte );
		wait_flush ();
		/* video enable
		 */
		herc_byte |= 0x08;	
        outp ( 0x3B8, herc_byte );
		break;

	case ('E'):
	case ('V'):
			wgetpalette ( save_palette );
			memset ( black_palette, 0, sizeof (black_palette) );
			wsetpalette ( black_palette );		
			wait_flush ();
			wsetpalette ( save_palette );

		break;

	default:
		/* do nothing if another type of monitor */
		return;

		}/*end switch*/

	return; /* wblackout */
	}

static void W_NEAR wait_flush (void)
	{
	/* wait for a keypress, then flush that key */
	while ( !wready() )
		{/*empty*/}

	wreadc ();
	return;		/* wait_flush */
	
	}


/*----------------------end of WGETC.C ----------------------------------*/

