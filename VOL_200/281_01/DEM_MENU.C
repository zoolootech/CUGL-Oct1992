/*  dem_menu.c
 *
 *  Demo program for menu functions
 *    This program will also spawn the window, data entry and general
 *    library demo programs from a menu.
 *
 */

#include <stdio.h>
#include <process.h>


#include "uc_defs.h"
#include "uc_glob.h"
#include "uc_proto.h"

#include "wn_defs.h"
#include "wn_glob.h"
#include "wn_proto.h"

#include  "mn_defs.h"
#include  "mn_glob.h"


#include "uc_key.h"



MNUPTR	    make_mnu();

MNU_ITEMPTR make_item();



void main( void )
{
   MNU_ITEMPTR	mnu_item;
   MNUPTR	mnu_ptr;
   WINDOWPTR	wn_mnu;
   WINDOWPTR	wn_main;
   int		dummy();
   int		bye_bye();
   int		unicorn();
   int		demos();
   int		registration();
   int		future();
   int		notes();
   int		uclib51();
   int		i;

   uc_init();	/* This function checks the current video mode and    */
		/* sets up the window system accordingly.	      */
		/* At present you must define attributes for either a */
		/* color or a monochrome system.  A future release    */
		/* add a universal attribute system used in a past    */
		/* Unicorn release.  This system allows the user to   */
		/* predefine a set of attributes for both systems and */
		/* to use the appropriate ones determined on a run    */
		/* time check of the video mode.  This means a single */
		/* program may be written that will run correctly on  */
		/* both color and monochrome systems.		      */
   if ( mouse_exist )
      m_flagdec();


   cur_save();	/* Save the users current cursor location and shape.  */


		/*  Make a full screen window.	This will be popped up*/
		/*  to save the users screen so that it may be	      */
		/*  restored on exiting this demo program.	      */
   wn_main = wn_make( 0, 0, 24, 80 );
   wn_wnclr( wn_main, mk_att( BLUE, BROWN + BRIGHT ) );
   wn_bclr( wn_main, mk_att( BLUE,  WHITE + BRIGHT ) );
   wn_clear( wn_main );
   wn_margin( wn_main, 1 );


		/* A future release will contain a function to build a*/
		/* title so the user will not have to do this messy   */
		/* stuff.  It will be hidden behind a function call.  */

   wn_main->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_main->title->title = ( char * ) malloc( 100 );
   strcpy( wn_main->title->title,
" Unicorn Software P.O. Box 3214 Kirkland, WA 98034-3214 (206) 823-4656" );
   wn_main->title->position = 4;
					/* Set the title color	      */
   wn_main->clr->title = mk_att( WHITE, BLUE + BRIGHT );


   wn_mkbdr( wn_main, BDR_LNP );

		/* position the cursor in the window and write some   */
		/* text for the user to read.			      */

   wn_csr( wn_main, 4, 21 );
   wn_printf( wn_main, "Unicorn Menu System Version 1.0\n" );

   wn_wnclr( wn_main, mk_att( BLUE, CYAN ) );
   wn_printf( wn_main, "\n                                     " );
   wn_printf( wn_main, "Welcome to the " );
   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "Unicorn Menu System\n" );

   wn_wnclr( wn_main, mk_att( BLUE, CYAN ) );
   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "Version 1.0.   This is included as\n" );

   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "a part of the Unicorn Library.     \n" );

   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "We hope you enjoy the menu Demo and\n" );

   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "the other demo programs it will run.\n\n" );

   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "Not all features are demonstrated but\n" );

   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "with the source code for this program\n" );

   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "and the other demos along with the\n" );

   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "reference guide there should be more\n" );

   wn_printf( wn_main, "                                     " );
   wn_printf( wn_main, "than enough to allow you to write\n" );

   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "    If you have a mouse driver       " );
   wn_wnclr( wn_main, mk_att( BLUE, CYAN ) );
   wn_printf( wn_main, "your own programs.  " );

   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "Registered users \n" );

   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "    installed, you may use the       " );
   wn_printf( wn_main, "will recieve a " );

   wn_wnclr( wn_main, mk_att( BLUE, BROWN + BRIGHT ) );
   wn_printf( wn_main, "6 disk set " );
   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "including\n" );

   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "    mouse to point and shoot         " );
   wn_wnclr( wn_main, mk_att( BLUE, BROWN + BRIGHT ) );
   wn_printf( wn_main, "full source code" );
   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, " for all of the over\n" );

   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "    with the menus and may use       " );
   wn_wnclr( wn_main, mk_att( BLUE, BROWN + BRIGHT ) );
   wn_printf( wn_main, "300 functions " );
   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "contained in this version\n" );

   wn_printf( wn_main, "    the buttons to move through      " );
   wn_printf( wn_main, "of the Unicorn Library.  " );
   wn_wnclr( wn_main, mk_att( BLUE, CYAN ) );
   wn_printf( wn_main, "Thanks for\n" );

   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "    the display instead of           " );
   wn_wnclr( wn_main, mk_att( BLUE, CYAN ) );
   wn_printf( wn_main, "looking at the library and considering\n" );

   wn_wnclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_printf( wn_main, "    pressing a key.                  " );
   wn_wnclr( wn_main, mk_att( BLUE, CYAN ) );
   wn_printf( wn_main, "registration.        " );
   wn_wnclr( wn_main, mk_att( BLUE, BROWN + BRIGHT ) );
   wn_printf( wn_main, " Dave Miller\n" );
   wn_wnclr( wn_main, mk_att( BLUE, CYAN ) );


		     /* Time to make a window for the menu.  It will  */
		     /* be attached to a menu structure.	      */
   wn_mnu = wn_make( 4, 2, 11, 32 );
   wn_wnclr( wn_mnu, mk_att( CYAN, BLACK + BRIGHT ) );
   wn_bclr( wn_mnu, mk_att( CYAN,  BROWN + BRIGHT ) );
   wn_clear( wn_mnu );
   wn_margin( wn_mnu, 2 );
   wn_mkbdr( wn_mnu, BDR_DLNP );

		     /* Make the menu structure now.		      */
   mnu_ptr = make_mnu( wn_mnu );

   wn_printf( mnu_ptr->mnu_window, "        Main Menu\n" );


		     /* Now we will make the actual menu items.  Each */
		     /* item call is passed several parameters.  You  */
		     /* may easily experiment with changing them to   */
		     /* get a better feel for the system.  One very   */
		     /* important one is the pointer to a function to */
		     /* be called if this item is selected.  For the  */
		     /* first item the pointer is 'unicorn'.	      */

   mnu_item = make_item( 3, 4, mk_att( CYAN, BLUE ),
	    mk_att( BLUE, CYAN + BRIGHT ), " Unicorn Software ", unicorn,
			'U', mk_att( CYAN, RED ), mnu_ptr );

   mnu_item = make_item( 4, 4, mk_att( CYAN, BLUE ),
	    mk_att( BLUE, CYAN + BRIGHT ), " Demo Programs    ", demos,
			'D', mk_att( CYAN, RED ), mnu_ptr );

   mnu_item = make_item( 5, 4, mk_att( CYAN, BLUE ),
	    mk_att( BLUE, CYAN + BRIGHT ), " Registration    ", registration,
			 'R', mk_att( CYAN, RED ), mnu_ptr );

   mnu_item = make_item( 6, 4, mk_att( CYAN, BLUE ),
	    mk_att( BLUE, CYAN + BRIGHT ), " Future Updates  ", future,
			  'F', mk_att( CYAN, RED ), mnu_ptr );

   mnu_item = make_item( 7, 4, mk_att( CYAN, BLUE ),
	    mk_att( BLUE, CYAN + BRIGHT ), " Library Notes   ", notes,
			  'L', mk_att( CYAN, RED ), mnu_ptr );

   mnu_item = make_item( 8, 4, mk_att( CYAN, BLUE ),
	    mk_att( BLUE, CYAN + BRIGHT ), " Uclib 5.1 Update", uclib51,
			  'i', mk_att( CYAN, RED ), mnu_ptr );

   mnu_item = make_item( 9, 4, mk_att( CYAN, BLUE ),
	    mk_att( BLUE, CYAN + BRIGHT ), " Exit to DOS     ", bye_bye,
			  'x', mk_att( CYAN, RED ), mnu_ptr );

		     /* Display the full screen window on the screen  */
   wn_dsply( wn_main );

		     /* Go process the menu.			      */
   mnu_proc( mnu_ptr, 1 );

		     /* Now that the menu is processed and the program*/
		     /* is over lets bring down the windows and       */
		     /* restore the users screen and cursor.	      */
   wn_hide( mnu_ptr->mnu_window );
   wn_hide( wn_main );
   cur_restore();
}



/*--------------------------------------------------------------------*/
/*  This function, called from the main menu will spawn the window    */
/*  demo program.						      */
/*  Notice that all menu item functions are passed a pointer to the   */
/*  menu and the choice from the menu.	This allows the called	      */
/*  function access to the menu information and thus to be smart      */
/*  enough to position any windows according to the menu item selected*/
/*  among other things. 					      */
/*--------------------------------------------------------------------*/

int window( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR	 wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;
   MNUPTR	 mn_ptr;
   MNU_ITEMPTR	 mn_item;
   int		 general();
   int		 window();
   int		 data_entry();
   int		 ret;

			     /* This uses the passed information to   */
			     /* reset the menu's border type and      */
			     /* attribute.			      */
   wn_bclr( mnu_ptr->mnu_window, mk_att( RED, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );

   ret = spawnl( P_WAIT, "window.exe", "window", NULL );
   if ( ret == -1 )
   {
      printf( "\n\nSpawn failed\n" );
      exit( -1 );
   }

			     /* This sets the calling menu's border   */
			     /* type and attribute back to the	      */
			     /* original values.		      */
   wn_bclr( mnu_ptr->mnu_window, mk_att( RED, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   return( 1 );   /* A return of 1 tells the menu processor to	      */
		  /* continue processing the menu.  A return of 0     */
		  /* informs the processor an exit is desired and thus*/
		  /* the processor will imediately exit.	      */
}



int general_no( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR	 wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;
   MNUPTR	 mn_ptr;
   MNU_ITEMPTR	 mn_item;
   int		 general();
   int		 window();
   int		 data_entry();
   int		 ret;
   WINDOWPTR	 wn_main;


   wn_bclr( mnu_ptr->mnu_window, mk_att( RED, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );

   cur_on();
		/*  Make a full screen window.	This will be popped up*/
		/*  to save the users screen so that it may be	      */
		/*  restored on exiting this demo program.	      */
   wn_main = wn_make( 0, 0, 24, 80 );
   wn_wnclr( wn_main, mk_att( BLUE, BROWN + BRIGHT ) );
   wn_bclr( wn_main, mk_att( BLUE,  WHITE + BRIGHT ) );
   wn_clear( wn_main );
   wn_margin( wn_main, 1 );

   wn_main->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_main->title->title = ( char * ) malloc( 100 );
   strcpy( wn_main->title->title,
" Unicorn Software P.O. Box 3214 Kirkland, WA 98034-3214 (206) 823-4656" );
   wn_main->title->position = 4;
					/* Set the title color	      */
   wn_main->clr->title = mk_att( WHITE, BLUE + BRIGHT );


   wn_mkbdr( wn_main, BDR_LNP );

   wn_dsply( wn_main );


   ret = spawnl( P_WAIT, "lib_demo.exe", "lib_demo", "1", "2", "3", "4", NULL );
   if ( ret == -1 )
   {
      printf( "\n\nSpawn failed\n" );
      exit( -1 );
   }

   wn_bclr( mnu_ptr->mnu_window, mk_att( RED, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   wn_hide( wn_main );
   cur_off();
   return( 1 );
}




int general( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR	 wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;
   MNUPTR	 mn_ptr;
   MNU_ITEMPTR	 mn_item;
   int		 general();
   int		 window();
   int		 data_entry();
   int		 ret;
   WINDOWPTR	 wn_main;


   wn_bclr( mnu_ptr->mnu_window, mk_att( RED, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );

   cur_on();
		/*  Make a full screen window.	This will be popped up*/
		/*  to save the users screen so that it may be	      */
		/*  restored on exiting this demo program.	      */
   wn_main = wn_make( 0, 0, 24, 80 );
   wn_wnclr( wn_main, mk_att( BLUE, BROWN + BRIGHT ) );
   wn_bclr( wn_main, mk_att( BLUE,  WHITE + BRIGHT ) );
   wn_clear( wn_main );
   wn_margin( wn_main, 1 );

   wn_main->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_main->title->title = ( char * ) malloc( 100 );
   strcpy( wn_main->title->title,
" Unicorn Software P.O. Box 3214 Kirkland, WA 98034-3214 (206) 823-4656" );
   wn_main->title->position = 4;
					/* Set the title color	      */
   wn_main->clr->title = mk_att( WHITE, BLUE + BRIGHT );


   wn_mkbdr( wn_main, BDR_LNP );

   wn_dsply( wn_main );


   ret = spawnl( P_WAIT, "lib_demo.exe", "lib_demo", NULL );
   if ( ret == -1 )
   {
      printf( "\n\nSpawn failed\n" );
      exit( -1 );
   }

   wn_bclr( mnu_ptr->mnu_window, mk_att( RED, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   wn_hide( wn_main );
   cur_off();
   return( 1 );
}



int data_entry( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR	 wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;
   MNUPTR	 mn_ptr;
   MNU_ITEMPTR	 mn_item;
   int		 general();
   int		 window();
   int		 data_entry();
   int		 ret;

   wn_bclr( mnu_ptr->mnu_window, mk_att( RED, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );

   ret = spawnl( P_WAIT, "tdata.exe", "tdata", NULL );
   if ( ret == -1 )
   {
      printf( "\n\nSpawn failed\n" );
      exit( -1 );
   }

   wn_bclr( mnu_ptr->mnu_window, mk_att( RED, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   return( 1 );
}



/*--------------------------------------------------------------------*/
/* This function sets up a sub menu using the passed information to   */
/* place it on the screen in correct relationship to the calling menu.*/
/* This menu will then call fucntions that are capable of spawing the */
/* demo programs for the library.				      */
/* As many sub menus as you need or want may be built in the same     */
/* manner creating as complicated a menu system as you desire or need */
/*--------------------------------------------------------------------*/

int demos( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR	 wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;
   MNUPTR	 mn_ptr;
   MNU_ITEMPTR	 mn_item;
   int		 general();
   int		 gereral_no();
   int		 window();
   int		 data_entry();

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );

   tmp = mnu_ptr->mnu_item;
   for( i = 1; i < choice; i++ )
      tmp = tmp->next;

   wn1 = wn_make( mnu_ptr->mnu_window->row + tmp->row + 1,
		  mnu_ptr->mnu_window->col + tmp->col + 1, 9, 32 );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_bclr( wn1,  mk_att( RED, BROWN + BRIGHT ) );
   wn_clear( wn1 );
   wn_margin( wn1, 4 );
   wn_mkbdr( wn1, BDR_DLNP );
   wn_dsply( wn1 );

   mn_ptr = make_mnu( wn1 );

   wn_printf( mn_ptr->mnu_window, "    Demo Programs\n" );

   mn_item = make_item( 3, 4, mk_att( RED, WHITE ),
		   mk_att( WHITE, RED ),  " General ( w/sound)", general,
			'w', mk_att( RED, BROWN + BRIGHT ), mn_ptr );

   mn_item = make_item( 4, 4, mk_att( RED, WHITE ),
		   mk_att( WHITE, RED ),  " General (no sound)", general_no,
			'n', mk_att( RED, BROWN + BRIGHT ), mn_ptr );

   mn_item = make_item( 5, 4, mk_att( RED, WHITE ),
		   mk_att( WHITE, RED ),  " Window System     ", window,
			'W', mk_att( RED, BROWN + BRIGHT ), mn_ptr );

   mn_item = make_item( 6, 4, mk_att( RED, WHITE ),
		    mk_att( WHITE, RED ), " Data Entry System ", data_entry,
			 'D', mk_att( RED, BROWN + BRIGHT ), mn_ptr );

   mn_item = make_item( 7, 4, mk_att( RED, WHITE ),
		    mk_att( WHITE, RED ), " Exit              ", bye_bye,
			  'x', mk_att( RED, BROWN + BRIGHT ), mn_ptr );


   mnu_proc( mn_ptr, 1 );

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   wn_hide( mn_ptr->mnu_window );
   return( 1 );
}







int uclib51( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR  wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;


   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );

   tmp = mnu_ptr->mnu_item;
   for( i = 1; i < choice; i++ )
      tmp = tmp->next;

   wn1 = wn_make( mnu_ptr->mnu_window->row + tmp->row + 1,
		  mnu_ptr->mnu_window->col + tmp->col + 1, 11, 63 );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_bclr( wn1,  mk_att( RED, WHITE + BRIGHT ) );
   wn_clear( wn1 );
   wn_margin( wn1, 3 );
   wn_mkbdr( wn1, BDR_12LNP );
   wn_dsply( wn1 );
   wn_printf( wn1, "     Unicorn Library Updates for version 5.1" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n\n   This release of the Unicorn Library is    \n" );
   wn_printf( wn1, "version 5.1 occuring in October of 1988.  Version\n" );
   wn_printf( wn1, "5.0 occurred in September of 1988.  This version \n" );
   wn_printf( wn1, "is comming so rapidly since the menu system was  \n" );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "\n              Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );

   wn_printf( wn1, "completed sooner than the author anticipated.    \n" );
   wn_printf( wn1, "   This new version adds the first portion of the\n" );
   wn_printf( wn1, "planned menus to the library along with mouse    \n" );
   wn_printf( wn1, "support.  The inititalization function 'uc_init()'\n" );
   wn_printf( wn1, "will now look for a mouse and if found it will   \n" );
   wn_printf( wn1, "turn on the mouse cursor ( default is a reverse  \n" );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "\n              Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );

   wn_printf( wn1, "video block ).  This cursor may be turned off by \n" );
   wn_printf( wn1, "a call to 'm_flagdec()' following the call to    \n" );
   wn_printf( wn1, "'uc_init().  The demos illustrate this.          \n" );
   wn_printf( wn1, "  The menus may be accessed by the mouse using   \n" );
   wn_printf( wn1, "the 'point and shoot approach.  In addition any   \n" );
   wn_printf( wn1, "time a key stroke is called for in the demos a   \n" );


   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "\n               Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );

   wn_printf( wn1, "mouse button may be pressed instead.             \n" );
   wn_printf( wn1, "  At the present time the next update is planned \n" );
   wn_printf( wn1, "for about the end of 1988.  Look for it on the   \n" );
   wn_printf( wn1, "boards and in particular the Seattle area boards \n" );
   wn_printf( wn1, "mentioned elsewhere in the documentation.         \n" );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "\n               Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );

   wn_hide( wn1 );

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   return( 1 );

}






int future( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR  wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;


   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );

   tmp = mnu_ptr->mnu_item;
   for( i = 1; i < choice; i++ )
      tmp = tmp->next;

   wn1 = wn_make( mnu_ptr->mnu_window->row + tmp->row + 1,
		  mnu_ptr->mnu_window->col + tmp->col + 1, 14, 63 );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_bclr( wn1,  mk_att( RED, WHITE + BRIGHT ) );
   wn_clear( wn1 );
   wn_margin( wn1, 3 );
   wn_mkbdr( wn1, BDR_12LNP );
   wn_dsply( wn1 );
   wn_printf( wn1, "                     The Future" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n\n   In large part the future is up to you, the\n" );
   wn_printf( wn1, "user.  Most of the our releases have been heavily\n" );
   wn_printf( wn1, "influenced by user input while others are the    \n" );
   wn_printf( wn1, "direct result of user requests.  Feel free to    \n" );
   wn_printf( wn1, "provide your input on features or other tools you\n" );
   wn_printf( wn1, "would like to see.                               \n" );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "\n              Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );

   wn_printf( wn1, "   Currently there are many enhancements planned \n" );
   wn_printf( wn1, "for this library.  New features are planned in   \n" );
   wn_printf( wn1, "particular for the window, data entry form and   \n" );
   wn_printf( wn1, "the menu sections of the library.  There are a   \n" );
   wn_printf( wn1, "large number of mouse functions available in the \n" );
   wn_printf( wn1, "library which will be utilized in these areas.   \n" );
   wn_printf( wn1, "Also planned are enhancements to the sound       \n" );
   wn_printf( wn1, "functions in the library.                        \n" );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "\n              Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );

   wn_printf( wn1, "  In addition upgrades of several other releases \n" );
   wn_printf( wn1, "are being planned.  All Unicorn releases except   \n" );
   wn_printf( wn1, "for one may be found by looking for UC????? on a \n" );
   wn_printf( wn1, "board since they all start with the letters 'UC' \n" );
   wn_printf( wn1, "except for the 'C Tool Box' which was released as\n" );
   wn_printf( wn1, "'CTB100.ARC'.                                    \n" );
   wn_printf( wn1, "   Again, let us know what YOU would like to see.\n" );
   wn_printf( wn1, "You may contact us directly or through the Journey\n" );
   wn_printf( wn1, "to Mars Board in the Seattle Area, (206) 486-3136.\n" );
   wn_printf( wn1, "Leave a message for Dave Miller.                 \n" );


   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "\n               Press a key to continue" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );

   wn_hide( wn1 );

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   return( 1 );

}






int notes( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR  wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );


   tmp = mnu_ptr->mnu_item;
   for( i = 1; i < choice; i++ )
      tmp = tmp->next;

   wn1 = wn_make( mnu_ptr->mnu_window->row + tmp->row + 1,
		  mnu_ptr->mnu_window->col + tmp->col + 1, 12, 63 );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_bclr( wn1,  mk_att( RED, WHITE + BRIGHT ) );
   wn_clear( wn1 );
   wn_margin( wn1, 2 );
   wn_mkbdr( wn1, BDR_12LNP );
   wn_dsply( wn1 );
   wn_printf( wn1, "                    Library Notes\n" );

   wn_printf( wn1, "   Documentation is found in the reference guide \n" );
   wn_printf( wn1, "file.  At present there is no full manual in the \n" );
   wn_printf( wn1, "true sense.  The reference guide coupled with the \n" );
   wn_printf( wn1, "source code for the demo programs should be more  \n" );
   wn_printf( wn1, "enough to get you started writing programs using  \n" );
   wn_printf( wn1, "the library.  Not all functions, unfortunately are\n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n                 Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );

   wn_printf( wn1, "\nincluded in the demos due to size considerations. \n" );
   wn_printf( wn1, "In addition all registered users will recieve a \n" );
   wn_printf( wn1, "disk containing all the test programs used in the\n" );
   wn_printf( wn1, "development of the library.  These obviously show\n" );
   wn_printf( wn1, "how the functions are called.                    \n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n                 Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );

   wn_printf( wn1, "\nA function call attached to a menu selection is   \n" );
   wn_printf( wn1, "passed a pointer to the menu structure and the  \n" );
   wn_printf( wn1, "currently selected item number.  This allows great\n" );
   wn_printf( wn1, "versitility in many ways.  In this demo the info \n" );
   wn_printf( wn1, "is used to position the function windows or  \n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n                 Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );


   wn_printf( wn1, "\nsub menus to just below the selected item on the  \n" );
   wn_printf( wn1, "menu.  However you chose to use the information  is\n" );
   wn_printf( wn1, "up to you but it will allow you to do quite a bit \n" );
   wn_printf( wn1, "with a little thought.                           \n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n                 Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );

   wn_printf( wn1, "\n   It is also possible to execute another program \n" );
   wn_printf( wn1, "from a menu.  The demos selection will spawn the two\n" );
   wn_printf( wn1, "other demos found with the menu demo.  This allows\n" );
   wn_printf( wn1, "a great deal of versitility in your programming. \n" );
   wn_printf( wn1, "   A utility ( uc_chk.exe ) is included in this  \n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n                 Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );

   wn_printf( wn1, "package for your use.  If run against the library \n" );
   wn_printf( wn1, "or any executables it produces it will list any and \n" );
   wn_printf( wn1, "all library functions included.  For example type:\n\n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "          uc_chk uclib.lib          \n" );
   wn_printf( wn1, "or        uc_chk dem_menu.exe       \n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n                 Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );

   wn_printf( wn1, "The output of this utility is redirectable to the \n" );
   wn_printf( wn1, "printer or to a file.  For example:  \n\n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "          uc_chk uclib.lib > prn         \n" );
   wn_printf( wn1, "or        uc_chk dem_menu.exe > listing.txt \n\n" );

   wn_wnclr( wn1, mk_att( RED, MAGENTA + BRIGHT ) );
   wn_printf( wn1, "               1 October 1988 Dave Miller        \n\n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "                 Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );

   wn_hide( wn1 );

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   return( 1 );
}





int registration( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR  wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );


   tmp = mnu_ptr->mnu_item;
   for( i = 1; i < choice; i++ )
      tmp = tmp->next;

   wn1 = wn_make( mnu_ptr->mnu_window->row + tmp->row + 1,
		  mnu_ptr->mnu_window->col + tmp->col + 1, 15, 63 );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_bclr( wn1,  mk_att( RED, WHITE + BRIGHT ) );
   wn_clear( wn1 );
   wn_margin( wn1, 2 );
   wn_mkbdr( wn1, BDR_12LNP );
   wn_dsply( wn1 );
   wn_printf( wn1, "               Library Registration\n" );

   wn_printf( wn1, "   Registration of your copy of the library couldn't\n" );
   wn_printf( wn1, "be easier.  Simply look at the file register.doc and\n" );
   wn_printf( wn1, "follow the instructions.                             \n" );
   wn_printf( wn1, "This will cause 6 ( " );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "yes 6!" );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, " ) disks to appear in your\n" );
   wn_printf( wn1, "mailbox shortly thereafter.  Two of these will contain\n" );
   wn_printf( wn1, "the library release files, three will contain ALL\n" );
   wn_printf( wn1, "source code to the library and the last will    \n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n              Press a key to continue\n\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );

   wn_printf( wn1, "contain all the author's test programs used in  \n" );
   wn_printf( wn1, "developing and testing the library.  These programs\n" );
   wn_printf( wn1, "will of course illustrate further the correct call  \n" );
   wn_printf( wn1, "for the functions and the source code should clear up\n" );
   wn_printf( wn1, "any remaining questions you might have.  Remember   \n" );
   wn_printf( wn1, "that's " );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "6 disks " );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "for the registered user with all the\n" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "source code " );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "to " );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "over 300 " );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "( and growing ) " );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "functions. \n" );
   wn_printf( wn1, "\n              Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );


   wn_printf( wn1, "Registration is NOT required for evaluation of  \n" );
   wn_printf( wn1, "the library, you are free to use it for this purpose\n" );
   wn_printf( wn1, "however, " );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "any commercial use or use for profit of this\n" );
   wn_printf( wn1, "library requires registration with the author.       \n" );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, "If you decide you like the library and use it you   \n" );
   wn_printf( wn1, "should register your copy.  You may give the public\n" );
   wn_printf( wn1, "files to a third party however the source code you \n" );
   wn_printf( wn1, "receive upon registration may NOT be transfered to \n" );
   wn_printf( wn1, "any third party.  Executables created with the code\n" );
   wn_printf( wn1, "may be transfered to third parties.                \n" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n              Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );

   wn_printf( wn1, "Please respect the author's work and effort in  \n" );
   wn_printf( wn1, "protecting the source code when you register.       \n" );
   wn_printf( wn1, "   Development of this library took over a year of \n" );
   wn_printf( wn1, "of the author's spare time ( about 25 hrs a week ).\n" );

   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "\n              Press a key to continue\n" );
   uc_key();
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );


   wn_hide( wn1 );

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BROWN +BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   return( 1 );
}




int unicorn( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR  wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BLUE ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_LNP );
   wn_bdr( mnu_ptr->mnu_window );


   tmp = mnu_ptr->mnu_item;
   for( i = 1; i < choice; i++ )
      tmp = tmp->next;

   wn1 = wn_make( mnu_ptr->mnu_window->row + tmp->row + 1,
		  mnu_ptr->mnu_window->col + tmp->col + 1, 15, 63 );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_bclr( wn1,  mk_att( RED, WHITE + BRIGHT ) );
   wn_clear( wn1 );
   wn_margin( wn1, 2 );
   wn_mkbdr( wn1, BDR_12LNP );
   wn_dsply( wn1 );
   wn_printf( wn1, "                Unicorn Software\n\n" );

   wn_printf( wn1, "   Unicorn Software is the spare time project of a  \n" );
   wn_printf( wn1, "single author.  Originally begun as a way to learn C,\n" );
   wn_printf( wn1, "the author soon found the process was at least as    \n" );
   wn_printf( wn1, "enjoyable as the end result.  Thus the process of    \n" );
   wn_printf( wn1, "developing tools for C programmers continues and will  \n" );
   wn_printf( wn1, "for the foreseeable future, consistant with user    \n" );
   wn_printf( wn1, "support and interest.                               \n" );

   wn_printf( wn1, "\n              Press a key to continue\n\n" );
   uc_key();

   wn_printf( wn1, "   Unicorn Software has been releasing shareware for\n" );
   wn_printf( wn1, "over two years.  This library is the latest release\n" );
   wn_printf( wn1, "and plans for expansion and enhancement continue. The\n" );
   wn_printf( wn1, "library was originally released in September 1988 and\n" );
   wn_printf( wn1, "this update, which added menus, was released in October\n" );
   wn_printf( wn1, "of 1988.  Plans continue for this library well into \n" );
   wn_printf( wn1, "1989 with several new features planned.             \n" );

   wn_printf( wn1, "\n              Press a key to continue\n\n" );
   uc_key();

   wn_printf( wn1, "   Watch for our releases on your favorite board or \n" );
   wn_printf( wn1, "contact Journey to Mars ( 206 ) 486-3136 or College \n" );
   wn_printf( wn1, "Corner at ( 206 ) 643-0804.  Both are fine boards   \n" );
   wn_printf( wn1, "run by excellent sysops in the Seattle area.  Unicorn\n" );
   wn_printf( wn1, "Software releases are usually placed on these two   \n" );
   wn_printf( wn1, "boards as soon as released so they should contain the\n" );
   wn_printf( wn1, "latest versions at all times.                       \n" );

   wn_printf( wn1, "\n              Press a key to continue\n\n" );
   uc_key();

   wn_printf( wn1, "   The author may be contacted on one of the above  \n" );
   wn_printf( wn1, "boards, leave a message for Dave Miller, or try     \n" );
   wn_printf( wn1, "directly by phone.  The current number is           \n" );
   wn_printf( wn1, "( 206 ) 823-4656.  Please call after 6PM weeknights \n" );
   wn_printf( wn1, "and sorry but no collect calls please.  Since the   \n" );
   wn_printf( wn1, "the author tends to move a bit, a post office box has\n" );
   wn_printf( wn1, "been established to provide a constant address.     \n" );
   wn_printf( wn1, "Unicorn Software may always be reached at that box. \n" );

   wn_printf( wn1, "\n              Press a key to continue\n" );
   uc_key();

   wn_hide( wn1 );

   wn_bclr( mnu_ptr->mnu_window, mk_att( CYAN, BROWN + BRIGHT ) );
   wn_mkbdr( mnu_ptr->mnu_window, BDR_DLNP );
   wn_bdr( mnu_ptr->mnu_window );

   return( 1 );
}




int bye_bye( MNUPTR mnu_ptr, int choice )
{
   WINDOWPTR  wn1;
   MNU_ITEMPTR	 tmp;
   int		 i;

   return( 0 ); 	    /* A return of Zero ( 0 ) causes the */
			    /* menu processor to exit the menu	 */
}
