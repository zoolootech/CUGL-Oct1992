/*  window.c
 *
 *  Demo program for uclib.lib Version 5.10
 *
 */

/*--------------------------------------------------------------------*/
/*  This program in no way illustrates each of the over 300 functions */
/*  contained in uclib.lib version 5.10.  It does however make an     */
/*  attempt to illustrate some of what can be done with the library.  */
/*--------------------------------------------------------------------*/

#include <stdio.h>	   /* needed since NULL is used in uc_defs.h  */
#ifdef MSC
#include <malloc.h>
#else
#include <alloc.h>
#endif

#include "uc_defs.h"	   /* Assume the headers are in the current   */
#include "uc_glob.h"	   /* directory.			      */
#include "uc_proto.h"

#include "wn_defs.h"
#include "wn_glob.h"
#include "wn_proto.h"

extern WINDOWPTR  err_wnd; /* Allow access to the system error window */

void main( void )	   /* Unicorn Software programs are written   */
{			   /* using ANSI standard prototyping.	      */
   WINDOWPTR	wn_main;
   WINDOWPTR	wn_sub;    /* sub window for the main screen window   */
   WINDOWPTR	wn_1;
   WINDOWPTR	wn_2;
   WINDOWPTR	wn_3;
   WINDOWPTR	wn_4;
   WINDOWPTR	wn_5;
   WINDOWPTR	wn_6;
   WINDOWPTR	wn_7;
   WINDOWPTR	wn_8;
   WINDOWPTR	wn_9;
   WINDOWPTR	wn_10;

   char 	*str_save();

   char 	err_str[ 256 ];  /* error message string	      */
   int		i;
   int		count;


   uc_init();			 /* Inititalize various system values */
   if ( mouse_exist )
      m_flagdec();

   cur_off();		      /* Turn off the cursor to avoid clutter */

   wn_main = wn_make( 0, 0, 25, 80 );  /* Define a window and size it */
			      /* Set the window normal attribute      */
   wn_wnclr( wn_main, mk_att( BLUE, BROWN + BRIGHT ) );
			      /* Set the window border attribute      */
   wn_bclr( wn_main, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_clear( wn_main );       /* Clear the window area		      */

			      /* Set up a window title - a future     */
			      /* enhancement will allow more than one */
			      /* title to be placed in the window's   */
			      /* border.  Position parameters will    */
			      /* also be allowed such as CENTER etc.  */
			      /* Also functions will be added to set  */
			      /* up the title directly without going  */
			      /* through this indirect process.       */

   wn_main->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );


   wn_main->title->title = ( char * ) malloc( 100 );


   strcpy( wn_main->title->title,
" Unicorn Software P.O. Box 3214 Kirkland, WA 98034-3214 (206) 823-4656" );
   wn_main->title->position = 4;
					/* Set the title color	      */
   wn_main->clr->title = mk_att( WHITE, RED + BRIGHT );

			      /* Set the window's right & left margins*/
   wn_margin( wn_main, 3 );

   wn_mkbdr( wn_main, BDR_DLNP ); /* make a border for the window     */




   wn_sub = wn_make( 4, 5, 19, 72 );  /* Define a window and size it */
			      /* Set the window normal attribute      */
   wn_wnclr( wn_sub, mk_att( BLUE, BROWN + BRIGHT ) );
			      /* Set the window border attribute      */
   wn_bclr( wn_sub, mk_att( BLUE, WHITE + BRIGHT ) );
   wn_clear( wn_sub );	      /* Clear the window area		      */

   wn_margin( wn_sub, 1 );

   wn_mkbdr( wn_sub, BDR_SLNP );  /* make a border for the window     */



/*--------------------------------------------------------------------*/
/* At this point the window has been defined and all parameters for   */
/* the basic window, the title and border are contained in structures.*/
/* Next we will write some text to the window, even though it is not  */
/* yet displayed on the screen, then the window will be displayed and */
/* finally further text will be written to the displayed window.      */
/*--------------------------------------------------------------------*/

			     /* the following function writes a single*/
			     /* character to the window.	      */
   wn_pchar( wn_main, '\n' );

   for ( i = 0; i <= 25; i++ )
      wn_pchar( wn_main, ' ' );

   wn_pchar( wn_main, 'U' );
   wn_pchar( wn_main, 'n' );
   wn_pchar( wn_main, 'i' );
   wn_pchar( wn_main, 'c' );
   wn_pchar( wn_main, 'o' );
   wn_pchar( wn_main, 'r' );
   wn_pchar( wn_main, 'n' );
   wn_pchar( wn_main, ' ' );
   wn_pchar( wn_main, 'L' );
   wn_pchar( wn_main, 'i' );
   wn_pchar( wn_main, 'b' );
   wn_pchar( wn_main, 'r' );
   wn_pchar( wn_main, 'a' );
   wn_pchar( wn_main, 'r' );
   wn_pchar( wn_main, 'y' );
   wn_pchar( wn_main, ' ' );
   wn_pchar( wn_main, '5' );
   wn_pchar( wn_main, '.' );
   wn_pchar( wn_main, '0' );
   wn_pchar( wn_main, '0' );
   wn_pchar( wn_main, '\n' );
   wn_pchar( wn_main, '\n' );


   wn_wnclr( wn_sub, mk_att( BLUE, WHITE ) );

   wn_printf( wn_sub,
   "\nWelcome to version 5.10 of " );

   wn_wnclr( wn_sub, mk_att( BLUE, CYAN + BRIGHT ) );

   wn_printf( wn_sub,
   "UCLIB.LIB" );

   wn_wnclr( wn_sub, mk_att( BLUE, WHITE ) );

   wn_printf( wn_sub,
   ".  This all new version of\n" );

   wn_printf( wn_sub,
   "the library contains many differences from past releases.  First\n" );

   wn_printf( wn_sub,
   "and foremost the library is now entirely in C, gone are the \n" );

   wn_printf( wn_sub,
   "assembler routines of past releases.  This should make it much\n" );

   wn_printf( wn_sub,
   "easier for users to understand and work with the library source\n" );

   wn_printf( wn_sub,
   "code.  An " );

   wn_wnclr( wn_sub, mk_att( BLUE, WHITE + BRIGHT ) );

   wn_printf( wn_sub,
   "all new windowing package " );

   wn_wnclr( wn_sub, mk_att( BLUE, WHITE ) );

   wn_printf( wn_sub,
   " is included as is the " );

   wn_wnclr( wn_sub, mk_att( BLUE, WHITE + BRIGHT ) );

   wn_printf( wn_sub,
   "data\nentry system" );

   wn_wnclr( wn_sub, mk_att( BLUE, WHITE ) );

   wn_printf( wn_sub,
   ".  The new windowing system allows you to write to\n" );

   wn_printf( wn_sub,
   "a window whether or not it is currently displayed on the screen.\n" );

   wn_printf( wn_sub,
   "The data entry system uses this windowing system for maximum user\n" );

   wn_printf( wn_sub,
   "flexibility.  As this demo progresses it will stop at various points.\n" );

   wn_printf( wn_sub,
   "When this occurs simply press a key to continue.                       \n" );

   wn_printf( wn_sub,
   "          \n" );

   wn_dsply( wn_main );

   wn_dsply( wn_sub );	 /* display the window now!!		     */


   gr_bckgnd( BLUE );	  /* Set the border color		      */


   uc_key();   /* get a user key stroke and ignore the key returned   */


   wn_printf( wn_sub,
   "\nSee how simple that was!  Now in this section please note\n" );

   wn_printf( wn_sub,
   "two important items, first the cursor has been turned off to\n" );

   wn_printf( wn_sub,
   "avoid unsightly screen clutter and secondly the main text of\n" );

   wn_printf( wn_sub,
   "this window will scroll as new lines are added.               \n" );

   uc_key();

   wn_printf( wn_sub,
   "\nIn this next section additional windows will be placed on the\n" );

   wn_printf( wn_sub,
   "screen.  They will be moved in all three directions, text will\n" );

   wn_printf( wn_sub,
   "be written to them in addition to other suprises.             \n" );


   uc_key();



   wn_1 = wn_make( 5, 5, 12, 40 );
   wn_wnclr( wn_1, mk_att( RED, BROWN + BRIGHT ) );
   wn_bclr( wn_1, mk_att( RED, WHITE + BRIGHT ) );
   wn_clear( wn_1 );
   wn_1->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_1->title->title = ( char * ) malloc( 100 );
   strcpy( wn_1->title->title, " Sub Window 1 " );
   wn_1->title->position = 4;
   wn_1->clr->title = mk_att( RED, WHITE + BRIGHT );
   wn_margin( wn_1, 1 );
   wn_mkbdr( wn_1, BDR_12LNP );


   wn_dsply( wn_1 );

   wn_printf( wn_1, "This window is currently in the\n" );

   wn_printf( wn_1, "forefront and next another window\n" );

   wn_printf( wn_1, "will be placed on the screen which\n" );

   wn_printf( wn_1, "will partially obscure this one.\n" );

   uc_key();




   wn_2 = wn_make( 7, 15, 12, 40 );
   wn_wnclr( wn_2, mk_att( CYAN, RED ) );
   wn_bclr( wn_2, mk_att( CYAN, RED ) );
   wn_clear( wn_2 );
   wn_2->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_2->title->title = ( char * ) malloc( 100 );
   strcpy( wn_2->title->title, " Sub Window 2 " );
   wn_2->title->position = 4;
   wn_2->clr->title = mk_att( CYAN, BROWN + BRIGHT );
   wn_margin( wn_2, 3 );
   wn_mkbdr( wn_2, BDR_21LNP );


   wn_dsply( wn_2 );

   wn_printf( wn_2, "The full source code for this\n" );

   wn_printf( wn_2, "demo is in provided to help you\n" );

   wn_printf( wn_2, "in using the library.  It is \n" );

   wn_printf( wn_2, "impossible for this demo to \n" );

   wn_printf( wn_2, "illustrate all of the over 300\n" );

   wn_printf( wn_2, "functions contained in the \n" );

   wn_printf( wn_2, "library however it will give\n" );

   wn_printf( wn_2, "you an idea of what is \n" );

   wn_printf( wn_2, "possible.                   \n" );



   uc_key();


   wn_front( wn_1 );

   wn_printf( wn_1, "\nThere is a reference guide\n" );

   wn_printf( wn_1, "provided to further aid you \n" );

   wn_printf( wn_1, "in using the library.  All\n" );

   wn_printf( wn_1, "registered users will be\n" );

   wn_printf( wn_1, "given the source code to all\n" );

   wn_printf( wn_1, "the author's test programs\n" );

   wn_printf( wn_1, "which should provide all the\n" );

   wn_printf( wn_1, "information needed to use\n" );

   wn_printf( wn_1, "the library efficiently. \n" );

   uc_key();

   wn_rposition( wn_1, 3, 15 );

   uc_key();

   wn_front( wn_2 );

   wn_printf( wn_2, "\nAs you may see windows may\n" );

   wn_printf( wn_2, "be moved in all three axis \n" );

   wn_printf( wn_2, "without much difficulty.  \n" );

   wn_printf( wn_2, "In addition it should be\n" );

   wn_printf( wn_2, "noted that most of the demos\n" );

   wn_printf( wn_2, "printing to windows is being\n" );

   wn_printf( wn_2, "done with a window oriented\n" );

   wn_printf( wn_2, "version of printf.\n" );


   uc_key();


   wn_3 = wn_make( 5, 10, 16, 60 );
   wn_wnclr( wn_3, mk_att( MAGENTA, WHITE + BRIGHT ) );
   wn_bclr( wn_3, mk_att( MAGENTA, BROWN + BRIGHT ) );
   wn_clear( wn_3 );

   wn_3->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_3->title->title = ( char * ) malloc( 100 );
   strcpy( wn_3->title->title, " wn_printf " );
   wn_3->clr->title = mk_att( MAGENTA, BLUE + BRIGHT );
  /*
   wn_3->title->title = str_save( " wn_printf " );
*/
   wn_3->title->position = 24;
   wn_margin( wn_3, 2 );
   wn_mkbdr( wn_3, BDR_LNP );
   wn_dsply( wn_3 );

   wn_printf( wn_3, "The window function part of the library contains\n" );
   wn_printf( wn_3, "a window oriented version of printf that uses   \n" );
   wn_printf( wn_3, "the current window attribute.  It is similar to \n" );
   wn_printf( wn_3, "the general library color version of printf with\n" );
   wn_printf( wn_3, "the exception of being able to specify a window.\n\n" );

   wn_printf( wn_3, "     Integers ----- %d, %d, %d\n",
					 5, 67, 4567 );
   wn_printf( wn_3, "     Floats ------- %f, %f, %.4f\n",
			      1.234, 567.4567, 23.45678912 );

   wn_printf( wn_3, "     Strings ------ %s\n",
			      "This is a test string" );

   wn_printf( wn_3, "     Doubles ------ %.15lf\n\n",
			      123.123456789012345678 );

   wn_printf( wn_3, "The window printf function will accept all the\n" );
   wn_printf( wn_3, "formatting parameters of the standard printf.\n" );


   uc_key();

   wn_hide( wn_3 );




   wn_4 = wn_make( 2, 1, 6, 38 );
   wn_wnclr( wn_4, mk_att( GREEN, BROWN + BRIGHT ) );
   wn_bclr( wn_4, mk_att( GREEN, RED ) );
   wn_clear( wn_4 );
   wn_4->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_4->title->title = ( char * ) malloc( 100 );
   strcpy( wn_4->title->title, " Multiple Scroll #1 " );
   wn_4->title->position = 3;
   wn_4->clr->title = mk_att( GREEN, BLACK );
   wn_margin( wn_4, 1 );
   wn_mkbdr( wn_4, BDR_DLNP );


   wn_dsply( wn_4 );






   wn_5 = wn_make( 2, 41, 6, 38 );
   wn_wnclr( wn_5, mk_att( MAGENTA, WHITE + BRIGHT ) );
   wn_bclr( wn_5, mk_att( MAGENTA, RED ) );
   wn_clear( wn_5 );
   wn_5->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_5->title->title = ( char * ) malloc( 100 );
   strcpy( wn_5->title->title, " Multiple Scroll #2 " );
   wn_5->title->position = 3;
   wn_5->clr->title = mk_att( MAGENTA, BLACK );
   wn_margin( wn_5, 1 );
   wn_mkbdr( wn_5, BDR_LNP );


   wn_dsply( wn_5 );




   wn_6 = wn_make( 18, 1, 6, 38 );
   wn_wnclr( wn_6, mk_att( RED, BROWN + BRIGHT ) );
   wn_bclr( wn_6, mk_att( RED, BROWN + BRIGHT ) );
   wn_clear( wn_6 );
   wn_6->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_6->title->title = ( char * ) malloc( 100 );
   strcpy( wn_6->title->title, " Multiple Scroll #3 " );
   wn_6->title->position = 3;
   wn_6->clr->title = mk_att( RED, BLACK );
   wn_margin( wn_6, 1 );
   wn_mkbdr( wn_6, BDR_21LNP );


   wn_dsply( wn_6 );






   wn_7 = wn_make( 18, 41, 6, 38 );
   wn_wnclr( wn_7, mk_att( CYAN, BLUE + BRIGHT ) );
   wn_bclr( wn_7, mk_att( CYAN, RED ) );
   wn_clear( wn_7 );
   wn_7->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_7->title->title = ( char * ) malloc( 100 );
   strcpy( wn_7->title->title, " Multiple Scroll #4 " );
   wn_7->title->position = 3;
   wn_7->clr->title = mk_att( CYAN, BLACK );
   wn_margin( wn_7, 1 );
   wn_mkbdr( wn_7, BDR_12LNP );


   wn_dsply( wn_7 );



   wn_printf( wn_4, "Let's watch several windows\n" );
   wn_printf( wn_5, "Let's watch several windows\n" );
   wn_printf( wn_6, "Let's watch several windows\n" );
   wn_printf( wn_7, "Let's watch several windows\n" );
   wn_printf( wn_4, "at once all scrolling lines\n" );
   wn_printf( wn_5, "at once all scrolling lines\n" );
   wn_printf( wn_6, "at once all scrolling lines\n" );
   wn_printf( wn_7, "at once all scrolling lines\n" );

   uc_key();

   wn_printf( wn_4, "in various parts of the screen.\n" );
   wn_printf( wn_5, "in various parts of the screen.\n" );
   wn_printf( wn_6, "in various parts of the screen.\n" );
   wn_printf( wn_7, "in various parts of the screen.\n" );
   wn_printf( wn_4, "While this doesn't accomplish\n" );
   wn_printf( wn_5, "While this doesn't accomplish\n" );
   wn_printf( wn_6, "While this doesn't accomplish\n" );
   wn_printf( wn_7, "While this doesn't accomplish\n" );
   wn_printf( wn_4, "of real use it does look\n" );
   wn_printf( wn_5, "of real use it does look\n" );
   wn_printf( wn_6, "of real use it does look\n" );
   wn_printf( wn_7, "of real use it does look\n" );

   uc_key();

   wn_printf( wn_4, "pretty!  Perhaps you have a\n" );
   wn_printf( wn_5, "pretty!  Perhaps you have a\n" );
   wn_printf( wn_6, "pretty!  Perhaps you have a\n" );
   wn_printf( wn_7, "pretty!  Perhaps you have a\n" );
   wn_printf( wn_4, "use for this capability in\n" );
   wn_printf( wn_6, "use for this capability in\n" );
   wn_printf( wn_7, "use for this capability in\n" );
   wn_printf( wn_5, "use for this capability in\n" );
   wn_printf( wn_4, "your own application.  If\n" );
   wn_printf( wn_5, "your own application.  If\n" );
   wn_printf( wn_6, "your own application.  If\n" );
   wn_printf( wn_7, "your own application.  If\n" );

   uc_key();

   wn_printf( wn_4, "you do at least it is\n" );
   wn_printf( wn_5, "you do at least it is\n" );
   wn_printf( wn_6, "you do at least it is\n" );
   wn_printf( wn_7, "you do at least it is\n" );
   wn_printf( wn_4, "possible to do. \n" );
   wn_printf( wn_5, "possible to do. \n" );
   wn_printf( wn_6, "possible to do. \n" );
   wn_printf( wn_7, "possible to do. \n" );

   uc_key();


   for( i = 0; i <= 20; i++ )
   {
      wn_printf( wn_4, "    Line #%d\n", i );
      wn_printf( wn_5, "    Line #%d\n", i );
      wn_printf( wn_6, "    Line #%d\n", i );
      wn_printf( wn_7, "    Line #%d\n", i );
   }




   wn_printf( wn_4, "\n     " );
   wn_printf( wn_5, "\n     " );
   wn_printf( wn_6, "\n     " );
   wn_printf( wn_7, "\n     " );


   wn_wnclr( wn_4, mk_att( GREEN, BLUE + BRIGHT ) );
   wn_wnclr( wn_5, mk_att( MAGENTA, BLUE + BRIGHT ) );
   wn_wnclr( wn_6, mk_att( RED, BLUE + BRIGHT ) );
   wn_wnclr( wn_7, mk_att( CYAN, BLUE + BRIGHT ) );

   wn_printf( wn_4, "Wasn't that fun" );
   wn_printf( wn_5, "Wasn't that fun" );
   wn_printf( wn_6, "Wasn't that fun" );
   wn_printf( wn_7, "Wasn't that fun" );

   wn_wnclr( wn_4, mk_att( GREEN, MAGENTA + BRIGHT ) );
   wn_wnclr( wn_5, mk_att( MAGENTA, GREEN + BRIGHT ) );
   wn_wnclr( wn_6, mk_att( RED, WHITE + BRIGHT ) );
   wn_wnclr( wn_7, mk_att( CYAN, BLUE + BRIGHT ) );

   wn_wnclr( wn_4, mk_att( GREEN, BROWN + BRIGHT ) );
   wn_wnclr( wn_5, mk_att( MAGENTA, WHITE + BRIGHT ) );
   wn_wnclr( wn_6, mk_att( RED, BROWN + BRIGHT ) );
   wn_wnclr( wn_7, mk_att( CYAN, BLUE + BRIGHT ) );


   wn_printf( wn_4, "\n  Note the atrribute change" );
   wn_printf( wn_5, "\n  Note the atrribute change" );
   wn_printf( wn_6, "\n  Note the atrribute change" );
   wn_printf( wn_7, "\n  Note the atrribute change" );


   uc_key();

   wn_hide( wn_4 );
   wn_hide( wn_5 );
   wn_hide( wn_6 );
   wn_hide( wn_7 );

   uc_key();




   wn_9 = wn_make( 5, 3, 18, 74 );
   wn_wnclr( wn_9, mk_att( RED, BROWN + BRIGHT ) );
   wn_bclr( wn_9, mk_att( RED, WHITE + BRIGHT ) );
   wn_clear( wn_9 );
   wn_9->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_9->title->title = ( char * ) malloc( 100 );
   strcpy( wn_9->title->title, " Unicorn Software " );
   wn_9->title->position = 28;
   wn_9->clr->title = mk_att( RED, CYAN );
   wn_margin( wn_9, 3 );
   wn_mkbdr( wn_9, BDR_LNP );


   wn_dsply( wn_9 );

   wn_printf( wn_9,
      "\nUnicorn Software has been supplying shareware for over two\n" );

   wn_printf( wn_9,
      "years.  Our location has changed and may change again in the\n" );

   wn_printf( wn_9,
      "future.  To avoid problems in users finding us we now use\n" );

   wn_printf( wn_9,
      "a P.O. Box as a permanant location.  The current phone number\n" );

   wn_printf( wn_9,
      "listed is the author's home phone which may also change in\n" );

   wn_printf( wn_9,
      "the future.  The author also uses several Seattle, WA area\n" );

   wn_printf( wn_9,
      "boards which are listed in the readme file.  While not\n" );

   wn_printf( wn_9,
      "very active messages should reach the author on one of\n" );

   wn_printf( wn_9,
      "these.  Also listed in the readme file is information\n" );

   wn_printf( wn_9,
      "concerning where to find upgrades to this library.\n" );

   wn_printf( wn_9,
      "We hope you find the library useful to you and if\n" );

   wn_printf( wn_9,
      "so you are encouraged to register your copy." );

   wn_wnclr( wn_9, mk_att( RED, WHITE + BRIGHT ) );

   wn_printf( wn_9,
      "  The\nthree disks of source code alone can be very educational.\n" );


   uc_key();

   wn_hide( wn_9 );



   wn_8 = wn_make( 5, 3, 18, 74 );
   wn_wnclr( wn_8, mk_att( WHITE, BLUE + BRIGHT ) );
   wn_bclr( wn_8, mk_att( WHITE, RED ) );
   wn_clear( wn_8 );
   wn_8->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_8->title->title = ( char * ) malloc( 100 );
   strcpy( wn_8->title->title, " REGISTRATION " );
   wn_8->title->position = 30;
   wn_8->clr->title = mk_att( WHITE, BLACK );
   wn_margin( wn_8, 3 );
   wn_mkbdr( wn_8, BDR_21LNP );


   wn_dsply( wn_8 );

   wn_printf( wn_8,
      "Registration of this version of the library may be accomplished\n" );


   wn_printf( wn_8,
      "by filling out the registration form provided and sending it to\n" );

   wn_printf( wn_8,
      "the below address.  This sets in motion a chain of events that\n" );

   wn_printf( wn_8,
      "will result in the source code for the entire library appearing\n" );

   wn_printf( wn_8,
      "mysteriously in your mailbox.  In addition you will receive the\n" );

   wn_printf( wn_8,
      "source for the author's test programs which exercise the library\n" );

   wn_printf( wn_8,
      "functions extensivly.  There are over 300 functions in the\n" );

   wn_printf( wn_8,
      "library currently and there are plans for extensive additions\n" );

   wn_printf( wn_8,
      "in future releases.\n\n" );

   wn_printf( wn_8, "                    Unicorn Software\n" );

   wn_printf( wn_8, "                    P.O. Box 3214   \n" );

   wn_printf( wn_8, "                    Kirkland, WA    \n" );

   wn_printf( wn_8, "                    98083-3214      \n" );

   wn_printf( wn_8, "                    (206) 823-4656  \n" );




   uc_key();




   wn_10 = wn_make( 5, 3, 18, 74 );
   wn_wnclr( wn_10, mk_att( WHITE, BLUE + BRIGHT ) );
   wn_bclr( wn_10, mk_att( WHITE, RED ) );
   wn_clear( wn_10 );
   wn_10->title = ( TITLEPTR ) malloc( sizeof( TITLE ) );
   wn_10->title->title = ( char * ) malloc( 100 );
   strcpy( wn_10->title->title, " The Future " );
   wn_10->title->position = 31;
   wn_10->clr->title = mk_att( WHITE, BLACK );
   wn_margin( wn_10, 3 );
   wn_mkbdr( wn_10, BDR_21LNP );


   wn_dsply( wn_10 );

   wn_printf( wn_10,
      "\n  The future holds many improvements and enhancements for the\n" );

   wn_printf( wn_10,
      "current version of the Unicorn Library.  The window and data\n" );

   wn_printf( wn_10,
      "entry portions in particular will have many additional functions\n" );


   wn_printf( wn_10,
      "added to improve user ability to " );

   wn_wnclr( wn_10, mk_att( WHITE, BROWN + BRIGHT ) );


   wn_printf( wn_10,
      "very easily" );

   wn_wnclr( wn_10, mk_att( WHITE, BLUE + BRIGHT ) );

   wn_printf( wn_10,
      " program the\n" );

   wn_printf( wn_10,
      "desired application.  Some of the enhancements under current\n" );

   wn_printf( wn_10,
      "consideration, in the planning stage or currently being coded\n" );

   wn_printf( wn_10,
      "include:\n\n" );

   wn_wnclr( wn_10, mk_att( WHITE, BROWN + BRIGHT ) );

   wn_printf( wn_10,
      "\t\t1.  Menus, ( these will be added very soon )\n" );


   wn_printf( wn_10,
      "\t\t2.  Additional field data handling in the data entry\n" );

   wn_printf( wn_10,
      "\t\t    portion of the library.  These will be higher level\n" );

   wn_printf( wn_10,
      "\t\t    functions to allow quicker overall definitions \n" );

   wn_printf( wn_10,
      "\t\t    while still leaving the lower level for manipulating\n" );

   wn_printf( wn_10,
      "\t\t    details easily.\n" );

   uc_key();


   wn_printf( wn_10,
      "\t\t3.  The ability to scroll windows down, right and left\n" );

   wn_printf( wn_10,
      "\t\t    in addition to the up scrolling seen in this demo.\n" );

   wn_printf( wn_10,
      "\t\t4.  A much more comprehensive help system that will\n" );

   wn_printf( wn_10,
      "\t\t    allow the use of a standard ASCII help file keyed\n" );

   wn_printf( wn_10,
      "\t\t    from a passed help tag.  This will help in making\n" );

   wn_printf( wn_10,
      "\t\t    a more robust context sensitive application.\n" );

   wn_printf( wn_10,
      "\t\t5.  The possibility of graphics windows is being explored\n" );

   wn_printf( wn_10,
      "\t\t6.  Additional sound functions.\n" );




   uc_key();


   wn_hide( wn_10 );

   uc_key();


   cur_on();		   /* Turn the cursor back on for the user    */



			   /* Set the screen border color to black    */
   gr_bckgnd( BLACK );

			/* Bring the window down & restore the screen */
   wn_hide( wn_1 );
   wn_hide( wn_2 );
   wn_hide( wn_8 );
   wn_hide( wn_sub );
   wn_hide( wn_main );

}
