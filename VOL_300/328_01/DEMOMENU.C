/* demomenu.c
 *
 * Demonstration of pulldown menus
 *
 *
 *
 *
 */

#include <stdlib.h>
#include <string.h>
#include "wtwg.h"


#define  mfunc(aa,nn)  void mf##aa##nn (void);  \
	 void mf##aa##nn (void) {  \
		wsetlocation (WLOC_ATCUR, 2, 1); \
		wpromptc(NULL, "menu func " #aa #nn, NULL);\
		return;  }


/* The following macros generate a series of 'empty' functions
 * that are called by the menu,
 * this is just to demonstrate creation of a menu tree
 * Actual menu functions can do anything, with these restrictions:
 *      1) must return nothing and 2) must take no arguments
 *              ie: be of form: void function(void) { do stuff... return; }
 *      3) must quit eventually after repetitive ESCAPE key presses
 */
mfunc(a,1)
mfunc(a,2)
mfunc(a,3)

mfunc(a1,1)
mfunc(a1,2)
mfunc(a1,3)

mfunc(a2,1)
mfunc(a2,2)
mfunc(a2,3)

mfunc(a3,1)
mfunc(a3,2)
mfunc(a3,3)




mfunc(b,1)
mfunc(b,2)
mfunc(b,3)
mfunc(b,4)


mfunc(c,1)
mfunc(c,2)
mfunc(c,3)
mfunc(c,4)


/* define a 'switch' that could be used to disable a menu option
 *     in this demo, all the menu options are always ON
 *
 * if you had separate switches for each menu function,
 *     you could temporarily inactivate a particular menu function
 *     by moving 0 to that function's switch.
 *
 * the function and the switch are associated with each other
 *    via the menu table.
 */
unsigned char  Mon[1]= {1};



/* Define a 'menu tree'
 * this tree is defined with the most deeply nested menu options on top
 * higher choices then reference their sub-menus by address.
 * The highest level menu (the top menu) is addressed in main()
 *   via the call to wpulldown ( ...top menu table... );
 */


WMENU  A1menu[] =	{
/* mu_entry  mu_func  mu_help mu_menu mu_enable mu_highlight mu_key  */
"A1 1",       mfa11,   "A1", NULL,  Mon,      3,           '1',
"A1 2",       mfa12,   "A1", NULL,  Mon,      3,           '2',
"A1 3",       mfa13,   "A1", NULL,  Mon,      3,           '3',
NULL,	     NULL,    NULL,    NULL,  NULL,     0,           0
			};


WMENU  A2menu[] =	{
/* mu_entry  mu_func  mu_help mu_menu mu_enable mu_highlight mu_key */
"A2 1",       mfa21,   "A2", NULL,  Mon,      3,           '1',
"A2 2",       mfa22,   "A2", NULL,  Mon,      3,           '2',
"A2 3",       mfa23,   "A2", NULL,  Mon,      3,           '3',
NULL,	     NULL,    NULL,    NULL,  NULL,     0,           0
			};


WMENU  A3menu[] =	{
/* mu_entry  mu_func  mu_help mu_menu mu_enable mu_highlight mu_key */
"A3 1",       mfa31,   "A3", NULL,  Mon,      0,           '1',
"A3 2",       mfa32,   "A3", NULL,  Mon,      1,           '2',
"A3 3",       mfa33,   "A3", NULL,  Mon,      3,           '3',
NULL,	     NULL,    NULL,    NULL,  NULL,     0,           0
			};



   /* one level up in the menu tree, references A1menu, A2menu, A3menu
    */
WMENU  Amenu[] =	{
/* mu_entry  mu_func  mu_help mu_menu mu_enable mu_highlight mu_key */
"A 1",       NULL,    "A1", A1menu, Mon,      2,           '1',
"A 2",       NULL,    "A2", A2menu, Mon,      2,           '2',
"A 3",       NULL,    "A3", A3menu, Mon,      2,           '3',
NULL,	     NULL,    NULL,    NULL,  NULL,     0,           0
			};




WMENU  Bmenu[] =	{
/* mu_entry  mu_func  mu_help mu_menu mu_enable mu_highlight mu_key */
"B 1",       mfb1,    "B1", NULL,  Mon,      2,           '1',
"B 2",       mfb2,    "B2", NULL,  Mon,      2,           '2',
"B 3",       mfb3,    "B3", NULL,  Mon,      2,           '3',
"B 4",       mfb4,    "B4", NULL,  Mon,      2,           '4',
NULL,	     NULL,    NULL,    NULL,  NULL,     0,           0
			};




WMENU  Cmenu[] =	{
/* mu_entry  mu_func  mu_help mu_menu mu_enable mu_highlight mu_key */
"C 1",       mfc1,    "C1", NULL,  Mon,      2,           '1',
"C 2",       mfc2,    "C2", NULL,  Mon,      2,           '2',
"C 3",       mfc3,    "C3", NULL,  Mon,      2,           '3',
"C 4",       mfc4,    "C4", NULL,  Mon,      2,           '4',
NULL,	     NULL,    NULL,    NULL,  NULL,     0,           0
			};



WMENU  topmenu[] =	{
/* mu_entry  mu_func  mu_help mu_menu mu_enable mu_highlight mu_key */

"A top",     NULL,    "A top", Amenu, Mon,      0,           ALT_A,
"B top",     NULL,    "B top", Bmenu, Mon,      0,           ALT_B,
"C top",     NULL,    "C top", Cmenu, Mon,      0,           ALT_C,
NULL,	     NULL,    NULL,    NULL,  NULL,     0,           0
			};





main ()
	{
	int key;

	winit('T');

	key = wpromptc ( "MODE", "\tSelect a mode", "Text", "Graphics", NULL );
	
	if ( key == 'G' )
		{
		winit ( 'G' );
		}
	else 
	if ( key == ESCAPE )
		{
		exit (0);
		}

	#ifdef __TURBOC__
		/* install a 'clock' onscreen in lower right corner
		 * NOTE 1) Microsoft C won't run this very well.
		 * 		2) In graphics on EGA/VGA some drawing will occassionally get
		 *			lost or garbled due to clock operation. OK in hercules.
		 */
		if ( (wmode == 'T') || ( wmonitor == 'H' ) )
			{
			wclockinstall ( wxabsmax-5, wyabsmax );
			}
	#endif 	/* TURBOC clock */


	/* install context-sensitive help
	 * NOTE demomenu.hlp and demomenu.hx are the names of the help files
	 *      MUST be in current directory
	 */
	whelp_install ( "demomenu" );

	/* install pulldown menus -
	 * NOTE topmenu is address of the 'root' of the menu tree
	 */
	wpulldown (topmenu);

	wsetattr ( LIGHTGRAY );
	wsetc ( 176 );		/* speckles */

	whelp_ptr = "demohelp";		/* setup help ptr for main menu */

	wsetattr ( (RED<<4) + LIGHTGRAY );
	do 	{
		key =wpromptc (NULL, "main program here\nF1 for help.",
				"continue", NULL );
		}
	while   ( key != ESCAPE );

	return (00);


	}
