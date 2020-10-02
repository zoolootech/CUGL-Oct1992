/* wmouse.c
 *
 *	simplified mouse interface.
 *		wmouse_init    - detects mouse.
 *		wmouse_turn    - turn mouse ON or OFF. set cursor & scaling.
 *                               call at start of wgets()
 *				 and again at exit from wgets()
 *
 *		wmouse_location- get mouse position & see if buttons hit
 *				 ( in text mode, sets character locations)
 *				 ( in graphics mode, sets pixel & char loc)
 *	
 *		Assumes that any press of RIGHT button will be translated to ESCAPE
 *		Therefore, mouse activity is only reported when:
 *			LEFT- button depress, any mvt with button down, button release
 *			CENTER- ditto.
 *			RIGHT- only reports when button is first pressed. 
 *
 *
 */
#include "wsys.h"







static void calc_coords (void);




static unsigned int prev_buttons =0;





/* wmouse_init
 *	sets wmouse.wms_present
 *
 *	in text mode sets a software mouse cursor.
 */
void wmouse_init (void)
	{
	int hold, nb;





	PSEUDOREGS

	_AX = 0;
	INTERRUPT (0x33);
	nb   = _BX;
	hold = _AX;


	if ( hold )
		{
		/* a mouse is present */
		wmouse.wms_present = nb;
		}


	if ( wmouse.wms_present &&  (wmode == 'T') )
		{
		/* setup software cursor = inverting = 0xffff, 0x7f00
		 *			   char      = 0x0000, 0x07cc
		 */
		if ( wmonitor == 'H' )
			{
			/* setup an underlining (=BLUE) cursor for HERCULES
			 * preserve any bright attributes underlying.
			 */
			wmouse_textmask ( 0x08ff, 0x0100 );
			}
		else
			{
			/* setup a reversing cursor for COLOR monitors
			 */
			wmouse_textmask ( 0xffff, 0x7f00 );
			}

		}

	/* reset current button state (if changing mode )
	 */
	prev_buttons = 0;


	return; /* wmouse_init */

	}



/* mouse text_mode cursor
 *	sets up a text mode cursor.
 *	PARAMETERS:
 *	      screen mask is AND'd with char/attr under mouse.
 *	      cursor mask is XOR'd with char/attr.
 *
 *	RETURNS: void.
 */
void wmouse_textmask ( unsigned int screen_mask, unsigned int cursor_mask )
	{
	PSEUDOREGS

	_DX = cursor_mask;
	_CX = screen_mask;
	_BX = 0; 		/* 0=software cursor, 1=hardware curosr */
	_AX = 0x0A;		/* text mode cursor */
	INTERRUPT ( 0x33 );
	return;			/* wmouse_textmask */
	}




/* wmouse_turn ()
 *	turns mouse ON or OFF
 *	NOTE: must keep calls to this function in order
 *		(alternate ON with OFF, must start with ON)
 * 	mouse driver actually keeps count of calls,
 *		if you call repetitively, mouse will turn OFF
 *		even if you call as ON.
 *
 *	When you turn the mouse ON, this routine sets mouse scaling values.
 */
void wmouse_turn ( int state )
	{
	int hold;

	PSEUDOREGS


	if ( wmouse.wms_present )
		{

		if ( state != OFF )
			{
			hold = 1;		/* 1=turn on 2 = turn off */


			}
		else
			{
			hold = 2;		/* turn mouse off */
			}

		_AX = hold;
		INTERRUPT (0x33);

		}


	return;	/* wmouse_turn */

	}






/* wmouse_location ()
 *
 *	get the curretn location of the mouse,
 *	    and number of times the left and right buttons were pressed.
 *	NOTE: this routine counts key presses on the left
 *		              and key releases on the right.
 *	    this allows the user to be tentative with the left key
 *		but forces the user to be definitive with the right one.
 *	RETURNS:
 *		MOUSE if mouse moved, ENTER for left button, ESCAPE for right
 *
 */
void  wmouse_location (void)
	{

	int 		xtemp,
			ytemp;

	unsigned int  	new_buttons,
			released,
			pressed;

	PSEUDOREGS



	if ( ! wmouse.wms_present )
		{
		return;
		}




	/* Check mouse status -
	 * this function returns mouse location and current button state.
	 * _BX returns buttons down = bit =1, L=0x01, R=0x02 C=0x04
	 */

	_AX =3;
	INTERRUPT ( 0x33 );
	new_buttons = _BX;
	xtemp = _CX;
	ytemp = _DX;

	/*
	 * NOTE mouse driver only guarantees meaning to the low-order bits.
	 * so black out bits we're not inerested in.
	 *
	 *	0x01 =left, 0x02 =right, 0x04 =center.
	 */
	new_buttons &= 0x07;



	/* check for button releases (comparing against prev. button state)
	 *
	 * a bit will be TRUE if button was down (bit ON) and now is up.
	 * also check for new presses of the left button.
	 */
	released = ( (~new_buttons) & ( prev_buttons) );
	pressed  = ( ( new_buttons) & (~prev_buttons) );



	/* setup new button state in prev_buttons,
	 * for next call
	 */
	prev_buttons = new_buttons;



	/* check button release flags
	 * button bits: 0x01 = L,     0x02= R,     0x04= C
	 */
	if ( released & 0x01 )
		{
		/* left button was just released
		 */
		wmouse.wms_internal |= (WMS_LEFT_RLS + WMS_MOVED);
		}
	if ( released & 0x04 )
		{
		wmouse.wms_internal |= WMS_CENTER_RLS;
		}
	if ( pressed & 0x01 )
		{
		wmouse.wms_internal |= (WMS_LEFT_PRS + WMS_MOVED);
		}
	if ( pressed & 0x02 )
		{
		wmouse.wms_internal |= WMS_RIGHT_PRS;
		}
	if ( pressed & 0x04 )
		{
		wmouse.wms_internal |= WMS_CENTER_PRS;
		}
	if ( released & 0x02 )
		{
		wmouse.wms_internal |= WMS_RIGHT_RLS;
		}
		
	/* see if mouse moved with left or center buttons held down.
	 */
	if ( ( new_buttons & 0x01   )		/* LEFT=0x01 or CENTER=0x04 */  
	   &&( ( wmouse.wms_xmickey != xtemp ) || ( wmouse.wms_ymickey != ytemp ) )
	   )
		{
		wmouse.wms_internal |= WMS_MOVED;
		}
		 
	/* check location if L button newly pressed 
	 * or mouse mvt with L button held down. 
	 */
	if ( wmouse.wms_internal )
		{
		wmouse.wms_xmickey = xtemp;
		wmouse.wms_ymickey = ytemp;
		calc_coords ();
		}

	wmouse.wms_used = wmouse.wms_internal;

	return;	/* wmouse_location */

	}






/* calc_coords () -  convert location from 'mickeys' to screen co-ords
 *	mickeys go from 0-639 and 1-199
 * screen coords are measured both in text units (chars)
 * 			 	and   graphics units (pixels)
 */


static void calc_coords (void)
	{
	/* get absolute position on screen, ignoring windows
	 */
	if ( wmode == 'T' )
		{
		wmouse.wms_xabs = wmouse.wms_xmickey/8;
		wmouse.wms_yabs = wmouse.wms_ymickey/8;
		}
	#ifndef TEXTONLY
	else
		{
		/* GRAPHICS MODE get pixel co-ords of mouse */
		wmouse.wms_pxabs = wmouse.wms_xmickey;
		wmouse.wms_pyabs = wmouse.wms_ymickey;

		wmouse.wms_xabs = wmouse.wms_pxabs / wpxchar;
		wmouse.wms_yabs = wmouse.wms_pyabs / wpychar;



		}
	#endif /* !TEXTONLY */




	/* calculate mouse position relative to window
	 */
	#ifndef TEXTONLY
		if ( wmode == 'G' )
			{
			/* calculate pixel co-ords within window */
			wmouse.wms_px = wmouse.wms_pxabs -
					( w0->winleft* wpxchar );
			wmouse.wms_py = wmouse.wms_pyabs -
					( w0->wintop * wpychar );
			}
	#endif	/* ! TEXTONLY */

	/* text mode -co-ords */
	wmouse.wms_x = wmouse.wms_xabs - ( w0->winleft );
	wmouse.wms_y = wmouse.wms_yabs - ( w0->wintop  );

	if (
	     (  /* x-cord inside window */
		wmouse.wms_xabs >= w0-> winleft
	     && wmouse.wms_xabs <= (w0->winleft + w0->winxmax)
	     )
	   &&
	     (	/* y co-ord inside window */
		wmouse.wms_yabs >= w0-> wintop
	     && wmouse.wms_yabs <= (w0->wintop + w0->winymax)
	     )
	   )
		{
		/* mouse is inside window */
		wmouse.wms_inwindow = 1;

		}
	else 	{
		wmouse.wms_inwindow = 0;
		}


	return;	/* calc_coords() */
	}







/*------------------------ end of WMOUSE.C -------------------------*/
