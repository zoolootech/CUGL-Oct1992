/* demomous.c
 * 		demo file for illustrating mouse programming
 */
 
#include <stdlib.h>
#include <ctype.h>
#include <dos.h>
#include <time.h>


#ifdef __TURBOC__
	#include <graphics.h>
#else
	#include "msc.h"
	#include <graph.h>
	#include <time.h>


/* Microsoft doesn't provide a way to delay program execution
 * This is a limitted version of the TurboC routine.
 */
void delay (long);
void delay (long millisec)
	{
	time_t  start, quit;
	
	start = clock ();
	quit  = start + (time_t) ((millisec *1000L)/CLK_TCK);
	
	while ( clock() < quit )
		/* empty  loop - wait */
		;
			
	return;		/* Microsoft C version of delay */
	} 

#endif		/* microsoft */

#include "wtwg.h"


/* define arrays of 'buttons' which are mouse-selectable boxes onscreen.
 * each button translates mouse selection into a keystroke,
 * so it's transparent whether user clicked the button or pressed equivalent
 * key.
 */
#define NBUTTONS 4
#define BUTTON_LEN  9
static char  *buttons[] =        { "F1 cow","F2 horse","F3 pig","F4 goat"};
static char  button_toggles[NBUTTONS] = {0};


static char *flabels[] = { "ONE", "TWO", "THREE", "FOUR", "FIVE",
							"SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN" };





main ()
	{
	int key, n;


	#ifdef __TURBOC__
		WOBJECT_D_ART *art_object;
	#endif /* Turbo C */

	#define SCROLL_KEY  -1
	#define RANGE 		50
	WBUTTON *Bptr;
	
	
	int color= 0, linestyle = 0, thickness =0;
	
	
	
	winit ('T');
	wclear ();
	
	key = wpromptc ("MOUSE DEMO", "   Run first part of demo\n"
								  "   in Text or Graphics modes ?\n",
								  "Text", "Graphics", NULL );
								  
	/* Only possible returned values from above prompt are 'T', 'G'  & ESCAPE
	 */
	if ( key == 'G' )
		{
		winit ('G');
		}
	else 
	if ( key == ESCAPE )
		{
		exit (0);
		}
	
		
	/* DEMO part I - simple use of mouse relative to a window.
	 */
	wopen ( 5,5, 50,10, (RED<<4)+YELLOW, SINGLE_BORDER,(RED<<4)+YELLOW, 0);
	wputs ( "Move the mouse with a button down, or press a key\n"
			"ESCAPE quits (RIGHT MOUSE BUTTON = ESCAPE)");
			
	while ( ESCAPE != (key = wgetc()) )
		{
		wclear();	
		if ( key == MOUSE )
			{
			wprintf ( "Mouse used. %s\nX (window) =%3.3i, Y (window) =%3.3i\n"
									  "X (screen) =%3.3i, Y (screen) =%3.3i\n",
									  (wmouse.wms_inwindow)? "in window" : 
									  						 "outside window",
									   wmouse.wms_x,   wmouse.wms_y,
 									   wmouse.wms_xabs,wmouse.wms_yabs );
			if ( wmode == 'G' )
				{
				wprintf (             "X pixels   =%3.3i, Y pixels  =%3.3i\n",
										wmouse.wms_px, wmouse.wms_py );
				}									
			if ( wmouse.wms_used & WMS_LEFT_RLS )
				{
				wputs ("LEFT BUTTON RELEASED");
				}
			else
			if ( wmouse.wms_used & WMS_LEFT_PRS )
				{
				wputs ("LEFT BUTTON JUST PRESSED - WAIT...");
				delay (1000);
				}
			else
			if ( wmouse.wms_used & WMS_CENTER_PRS )
				{
				wputs ("CENTER BUTTON JUST PRESSED - WAIT...");
				delay (1000);
				}
			else
			if ( wmouse.wms_used & WMS_CENTER_RLS )
				{
				wputs ("CENTER BUTTON RELEASED");
				}
 			/* The right button is automatically translated into ESCAPE 
			 */
			
			}	/* end (key==MOUSE) */
		else
			{
			if ( isascii (key) && isprint(key) )
				{
				wprintf ("Key pressed,\nvalue = %c",key);
				}
			else
				{
				wprintf ("Non-print key pressed,\nvalue =%i, scancode=%i",
							key, key-128);
				}
			}
		wgoto ( 2,9 );
		wputs ( "ESCAPE or RIGHT BUTTON to quit" );
		}	/* end while() */
	
	wclose ();
	 
	/* DEMO PART II.  demo buttons.
	 */
	 
	wopen ( 3,3,  25,20, GREEN, DOUBLE_BORDER, GREEN<<4, 0 );
	wputs ( "Pick animals with mouse" );
	for ( n= 0; n< NBUTTONS; ++n )
			{
						/* text,     x, y,    len,          key,   style */
			wbutton_add (buttons[n], 2, 2+3*n,BUTTON_LEN, FKEY(1+n),WBTN_BOX );
			wgoto ( 4, 2+3*n );
			}
			
			/* buttons for ESCAPE and ENTER to be mouse-selectable.
			 * Notes: x & y specify co-rds of text for button,
			 *			not the co-ords for the surrounding box.
			 *		  length of the button includes terminal NULL of string.
			 *   
			 */
	wbutton_add ( "ESCAPE", 2, 18, 7, ESCAPE, WBTN_BOX );
	wbutton_add ( "ENTER",  11,18, 6, ENTER, WBTN_BOX );
	
	
	/* allow user to select some buttons
	 */	
	while ( !  (  (ESCAPE == (key = wgetc())) || ( ENTER == key ) )  )
		{
		if ( key >= FKEY(1)  && key <= FKEY(NBUTTONS) )
			{
			n = key - FKEY(1);		/* 0 thru NBUTTONS-1 */
			button_toggles[n] = button_toggles[n] ? 0: 1;	/* toggle */
			
			if ( button_toggles[n] )
				{
				wbutton_mark (key, '>');		/* mark as ON */
				}
			else
				{
				wbutton_mark (key, ' ');		/* remove mark */
				}
			
			}
		
		
		}	/* end while */
	/* show what choices were made
	 */	
	wclear ();
	wputs ("You...");
	for ( n=0; n< NBUTTONS; ++n )
		{
		wgoto ( 4, 2+3*n );
		if ( button_toggles[n] )
			{
			wputs ("\nSelected ");
			}
		else
			{
			wputs ("\nIgnored ");
			}
		wputs ( buttons[n] +3);		/* pts to animal name */
		}
	wgoto ( 0,18 );
	wprintf ( "Final key was %s", (key==ENTER)? "ENTER" : "ESCAPE" );
	
	key=wpromptc (NULL,"This prompt also creates buttons...\n"
					   "Is this easy? (Y=yes, N=no)", 
					   /* NULL-terminated list of buttons for answers 
					    */
				   	   "YES", "NO", NULL );
	wgoto ( 0,14 );
	if ( key== 'Y' )
		{
		wputs ("Yes, it's easy");
		}
	else
	if ( key== 'N' )
		{
		wputs ("It is so easy");
		}
	else
		{
		/* the only values returned after wpromptc(),
		 * are ESCAPE or the first letter of the buttons on the list
		 * so only choice left is ESCAPE
		 */
		wputs ("ESCAPE hit");
		}


	wclose();

	/* DEMO PART III - scroll bar.
	 */
	
	n= RANGE/2;	
	
	wopen (5,2, 12,20, 0x07, SINGLE_BORDER, 0x70, 0);
	wtitle ("SCROLL BAR");
		
	/* create a scrollbar at x=5, onscreen from y=2 to 18,
	 * covering virtual values from 0 to 50 (RANGE = 50)
	 * initially at midpoint. 
	 */
	Bptr =wscrollbar_add ( SCROLL_KEY, 5, 2, 18, RANGE, n );
	wgoto (0,0);
	wprintf ("value =%2.2u", n);

	/* label the limits of the scrollbar
	 */
	wgoto ( 3,  2 );
	wputs ( "00" ); 
	wgoto ( 3, 18 );
	wprintf ("%2.2u", RANGE);
	
	
	while ( ESCAPE != (key=wgetc()) )
		{

		if ( key == SCROLL_KEY )
			{
			n = wscrollbar_scroll ( Bptr );
			}
		else
		if ( key == DN_ARROW  &&  n < RANGE )
			{
			++n;
			wscrollbar_reset (Bptr, n);
			}
		else
		if ( key == UP_ARROW  &&  n >0 )
			{
			--n;
			wscrollbar_reset (Bptr, n);
			}

		wgoto (0,0);
		wprintf ("value =%2.2u", n);
			
			
		}


	/* DEMO part IV. mouse center button.
	 */
	wpopfkey ( flabels );
	
	key = wpromptc ( NULL, "PRESS MIDDLE MOUSE BUTTON TO GET FKEY MENU", NULL );
	if (  ( key >= FKEY(1) && key <= FKEY(10) )
		{
		wpromptc ( NULL, flabels[key - FKEY(1)], NULL );
		}
	else 
	if ( key == FKEY_11 )
		{
		wpromptc ( NULL, flabels[11 -1], NULL );
		}



	
	#ifdef __TURBOC__	
	/* DEMO part V. drawing.
	 */
	winit ('G'); 
	 
	wsetattr (0x70);
	wclear ();
	
	wopen ( 2,2, 75, 15, LIGHTGRAY, SINGLE_BORDER, LIGHTGRAY, 0); 
	wtitle ("CANVAS");
	while ( ESCAPE != 	
				(key = wpromptc ("DRAWING DEMO", 
						"Pick a shape to draw.\n"
						"Click left button to start, right to erase\n"
						"H=horizontal line, V=vertical line, X=cross-hair,\n"
						"R=rectangle,       S=squiggle,      L=line",
						"H", "V", "X", "R", "L", "S", NULL ) ) )
		{
		wclear();
		key = tolower (key);	/* shape indicators are 'r', 'v' etc... */
		if ( wmonitor == 'H' )
			{
			color = 0x07;
			}
		else
			{
			color = 1+ (color++)%7;		/* rotate colors 1-7 */
			}
		thickness = (thickness==NORM_WIDTH) ? THICK_WIDTH : NORM_WIDTH;
		linestyle = (linestyle++)%4;	/* rotate linestyles 0-3 */
		
		art_object =wdraw ( key, color +BRIGHT, linestyle, thickness );
		/* the structure members art_object-> wdr_x1, _x2, _y1, _y2
		 * give the window-relative pixel co-ords for the object 
		 */
		free ( art_object );
		}
	
	#endif /* __TURBOC__ */
	
	
	return (0);		/* main */
	}
