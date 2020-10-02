/*DEMOHKEY.C
 * 	this program demonstrates how to install 'hotkey handlers'
 *
 *	a hotkey handler is a function that is executed when a specific key 
 *	...is pressed. The function takes no arguments, returns no values.
 *
 *		
 */


#include <stdlib.h>
#include <ctype.h>

#include "wtwg.h"


/* dummy function to echo any key pressed in whicever window is open
 */
void echo_key (void);


/* Functions which will be set up as hotkeys called by F2 and F3
 * NOTE what happens if you press F2 or F3 when the hotkey function is active.
 *	...reentrant calls to the hotkey handler are disallowed,
 *  ...and the 'hotkey' (F2 or F3) is passed thru
 *  ... to the func that called wgetc().
 */
void F2func (void);
void F2func (void)
	{
	char *save_help;

	save_help = whelp_ptr;
	whelp_ptr = "F2 key";

	wopen ( 10, 14, 45, 5, YELLOW, DOUBLE_BORDER, YELLOW, WSAVE2RAM);
	wtitle ("F2 hotkey");
	wgoto ( 0, 3 );
	wputs ( " Now in the F2 HOTKEY FUNCTION...\n"
			" KEYS will now be echoed in this window\n"
			" until you press ESCAPE to return to main."  );
	
	echo_key();

	wclose ();
	whelp_ptr = save_help;


	return;		/* F2func */
	}
	
void F3func (void);
void F3func (void)
	{
	char *save_help;

	save_help = whelp_ptr;
	whelp_ptr = "F3 key";

	wopen ( 20, 16, 45, 6, LIGHTGRAY<<4,SPECKLE_BORDER,LIGHTGRAY<<4,WSAVE2RAM);
	wtitle ("F3 hotkey");
	wgoto ( 0, 3 );
	wputs ( " Now in the F3 HOTKEY FUNCTION...\n"
			" KEYS will now be echoed in this window\n"
			" until you press ESCAPE to return to main."  );
	
	echo_key();

	wclose ();
	whelp_ptr = save_help;
	return;		/* F3func */
	}
	
	
void echo_key (void)
	{
	int key;
	int count = 0;

	while ( ESCAPE != ( key= wgetc() ) )
		{
		wgoto ( 0, w0-> winymax );		/* bottom line */
		wscroll ();
		if ( isascii (key) && isprint (key) )
			{
			wputc (key);
			wprintf ("    %i", ++count );
			}
		else
			{
			if ( key == FKEY(2) )
				{
				wputs ("F2 HOTKEY is already active\nIGNORED");
				}
			else
			if ( key == FKEY(2) )
				{
				wputs ("F3 HOTKEY is already active\nIGNORED");
				}
			else
				{
				wputs ("Please only enter printable keys...");
				}
			}
		}
	
	return;		/* echo_key */		

	}	
	
	
main()
	{
	winit ( 'T' );
	
	whelp_install ( "demohkey" );		/* F1 */
	whelp_ptr = "demohkey";
	
	whotkey_install ( FKEY(2), F2func );	/* associate FKEY(2) with F2func */
	whotkey_install ( FKEY(3), F3func );



	
	wclear ();
	
	wputs ( "This is a simple echo program. Keys will be echoed in a window.\n"
			"press ESCAPE to quit, press F1 for help, press F2/F3 as hotkeys\n"
		  );
	#ifdef __TURBOC__
		/*  demonstrate use of the SysRq key
		 *		TurboC only. Hold ALT and press the SysRq (PrintScreen) key
		 *
		 */
		wSysRq_install ( wSysRq_ask );
		wputs ( "Or press ALT-SysRq for keyboard interrupt demo.\n" 
				"If keyboard locks, press ALT-SysRq again" );
	#endif /* __TURBOC__ */
	
	wopen ( 4,5, 45, 11,  
		LIGHTGRAY+(RED<<4),  SINGLE_BORDER,  LIGHTGRAY+(RED<<4), 0 );
	wtitle ( "MAIN PROGRAM WINDOW" );
	
	echo_key ();
	
	wclose ();
	wclear ();		/* clears fullscreen */
	
	wputs ("That\'s all folks");
	
	return (0);		/* main */
	
	}