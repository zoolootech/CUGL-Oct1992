/*  tdata.c
 *
 *  Test program for data entry functions
 *
 */


/*
 *  This program demonstrates some of the data entry functions contained
 *  in the Unicorn Library.  While the author intends to add many more
 *  functions and a much broader capability to the library this early
 *  version is being released.	Planned for a future release is a form
 *  design utility that will allow easy field definitions.  This utility
 *  should be able to take an ASCII description file and write the C
 *  code for the fields.  This should save considerable time in form
 *  development.  At present this is in the early planning stages so
 *  no final release date has been set.
 *
 */



#include <stdio.h>

#include "uc_defs.h"
#include "uc_glob.h"

#include "wn_defs.h"
#include "wn_glob.h"

WINDOWPTR  fmp;
WINDOWPTR  wn1;
WINDOWPTR  wn_main;
WINDOWPTR  wn_title;
WINDOWPTR  wn_make();
char	   *strblnk();
FIELDPTR   fld_make();
char	   *str_save();
FIELDPTR   fld[ 8 ];


void main( void )
{
   int	  i;

   char   *rec;
   char   *rec_mask;

   char   *fname;
   char   *f_mask;

   char   *lname;
   char   *l_mask;

   char   *addr;
   char   *ad_mask;

   char   *city;
   char   *c_mask;

   char   *state;
   char   *s_mask;

   char   *zip;
   char   *z_mask;

   char   *phone;
   char   *p_mask;

   char   *birthday;
   char   *b_mask;

   char   *amount;
   char   *a_mask;

   int	  before6();
   int	  after6();
   int	  on_key();
   int	  val_state();
   int	  val_date();


   char   *xx;

			    /*	Save the users original cursor status */
   cur_save();


			    /*	This function inititalizes the system */
   uc_init();
   if ( mouse_exist )
      m_flagdec();
			    /*	This window is built as a full screen */
			    /*	window to save the users original     */
			    /*	screen.  Bringing this window down at */
			    /*	the end of the demo the screen will be*/
			    /*	restored to its original state.       */

   wn_main = wn_make( 0, 0, 24, 80 );
   wn_wnclr( wn_main, mk_att( BLACK, CYAN + BRIGHT ) );
   wn_bclr( wn_main, mk_att( BLACK, BROWN + BRIGHT ) );
   wn_clear( wn_main );
   wn_margin( wn_main, 5 );
   wn_mkbdr( wn_main, BDR_SLNP );

   wn_dsply( wn_main );



	       /* Make a window for display of the Unicorn Software   */
	       /* address and phone number.  The phone is the authors */
	       /* home phone.  Call after 6pm Pacific time on	      */
	       /* weeknights please.  The author must moonlight for   */
	       /* about 40 hours a week as a software engineer in     */
	       /* order to support this shareware hobby.	      */

   wn_title = wn_make( 0, 0, 7, 80 );
   wn_wnclr( wn_title, mk_att( BLACK, BROWN + BRIGHT ) );
   wn_bclr( wn_title, mk_att( BLACK, BROWN + BRIGHT ) );
   wn_clear( wn_title );

   wn_mkbdr( wn_title, BDR_SLNP );

   wn_dsply( wn_title );

   wn_printf( wn_title, "\t\t\t\t\t         Unicorn Software\n" );
   wn_printf( wn_title, "\t\t\t\t\tP.O Box 3214 Kirkland, WA 98083-3214\n" );
   wn_printf( wn_title, "\t\t\t\t\t         ( 206 ) 823-4656\n" );
   wn_wnclr( wn_title, mk_att( BLACK, MAGENTA + BRIGHT ) );
   wn_printf( wn_title, "\t\t\t\t\t      No collect calls Please" );


			   /*  This was inititalized in the uc_init   */
			   /*  call but then a window ( wn_main ) was */
			   /*  placed on top of it so now we must move*/
			   /* it to the front.			      */
   wn_front( wnd_message );



			    /* Now we will define the data form and   */
			    /* start attaching fields to the window   */

   fmp = wn_make( 5, 0, 14, 80 );
   wn_wnclr( fmp, mk_att( BLUE, CYAN + BRIGHT ) );
   wn_bclr( fmp, mk_att( BLUE, BROWN + BRIGHT ) );
   wn_clear( fmp );

   wn_mkbdr( fmp, BDR_DLNP );

   wn_printf( fmp,
    "                Unicorn Software Data Entry System 1.00" );
   wn_printf( fmp, "\n\n\n\n\n\n\n\n\n\n\n" );
   wn_printf( fmp,
    "                Press F1 for help  -  ESC to leave form" );

			      /* display the form on the screen       */
   wn_dsply( fmp );

			      /* prepare to add data fields to window */
   fld_init( fmp );



				  /* This is the first field	      */
   rec	    = strblnk( 8 );
   rec_mask = strblnk( 8 );

   strcpy( rec,       "    " );
   strcpy( rec_mask,  "9999" );

   fld[ 0 ] = fld_make( fmp, 2, 5, "Record Number ", 19,
					 rec_mask, rec, FLD_ZERO_FILL );
   if ( fld[ 0 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 0" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the current record number for this person" );
   sf_message( fld[ 0 ], xx );
   xx = str_save( "The record number must be an integer" );
   sf_hlptag( fld[ 0 ], xx );
				/* The helptag parameter is currently */
				/* nothing more than a message.  In a */
				/* later release it will become a true*/
				/* help tag that may be used to find  */
				/* the help message in an ASCII text  */
				/* file.  The user will then be able  */
				/* to scroll this file in the help    */
				/* window.  This will allow the       */
				/* creation of a full capability help */
				/* system that is context sensitive.  */


   fname    = strblnk( 18 );
   f_mask   = strblnk( 18 );

   strcpy( fname,   "               " );
   strcpy( f_mask,  "!AAAAAAAAAAAAAA" );

   fld[ 1 ] = fld_make( fmp, 3, 5, "First Name    ", 19,
				    f_mask, fname, FLD_ALPHA_NUMERIC );
   if ( fld[ 1 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 1" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the first name only ( no middle initial )" );
   sf_message( fld[ 1 ], xx );
   xx = str_save( "Enter the full legal first name" );
   sf_hlptag( fld[ 1 ], xx );





   lname    = strblnk( 18 );
   l_mask   = strblnk( 18 );

   strcpy( lname,   "Miller         " );
   strcpy( l_mask,  "!AAAAAAAAAAAAAA" );

   fld[ 2 ] = fld_make( fmp, 4, 5, "Last Name     ", 19,
					 l_mask, lname, FLD_ALPHA_NUMERIC );
   if ( fld[ 2 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 2" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the last name for this person" );
   sf_message( fld[ 2 ], xx );
   xx = str_save( "Enter the full legal last name" );
   sf_hlptag( fld[ 2 ], xx );






   addr     = strblnk( 28 );
   ad_mask  = strblnk( 28 );

   strcpy( addr,     "                     " );
   strcpy( ad_mask,  "XXXXXXXXXXXXXXXXXXXXX" );

   fld[ 3 ] = fld_make( fmp, 5, 5, "Address       ", 19,
					 ad_mask, addr, FLD_ALPHA_NUMERIC );
   if ( fld[ 3 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 3" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the full street address" );
   sf_message( fld[ 3 ], xx );
   xx = str_save( "Include apartment number if needed" );
   sf_hlptag( fld[ 3 ], xx );







   city     = strblnk( 18 );
   c_mask   = strblnk( 18 );

   strcpy( city,     "                " );
   strcpy( c_mask,   "!AAAAAAAAAAAAAAA" );

   fld[ 4 ] = fld_make( fmp, 6, 5, "City          ", 19,
					 c_mask, city, FLD_ALPHA_NUMERIC );
   if ( fld[ 4 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 4" ) );
      exit( 1 );
   }
   xx = str_save( "Enter this person's city of residence" );
   sf_message( fld[ 4 ], xx );
   xx = str_save( "This must be the city of legal residence" );
   sf_hlptag( fld[ 4 ], xx );




   state    = strblnk( 4 );
   s_mask   = strblnk( 4 );

   strcpy( state,    "  " );
   strcpy( s_mask,   "!!" );

   fld[ 5 ] = fld_make( fmp, 6, 42, "State ", 50,
					 s_mask, state, FLD_ALPHA_NUMERIC );
   if ( fld[ 5 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 5" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the state of residence" );
   sf_message( fld[ 5 ], xx );
   xx = str_save( "Use proper postal abbreviations only" );
   sf_hlptag( fld[ 5 ], xx );
   sf_val( fld[ 5 ], val_state );






   zip	    = strblnk( 8 );
   z_mask   = strblnk( 8 );

   strcpy( zip,      "     " );
   strcpy( z_mask,   "99999" );

   fld[ 6 ] = fld_make( fmp, 6, 55, "Zip Code", 64,
					 z_mask, zip, FLD_ALPHA_NUMERIC );
   if ( fld[ 6 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 6" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the five digit zip code" );
   sf_message( fld[ 6 ], xx );
   xx = str_save( "This field does not include the extended zip code" );
   sf_hlptag( fld[ 6 ], xx );

   sf_begfunc( fld[ 6 ], before6 );
   sf_endfunc( fld[ 6 ], after6  );




   phone    = strblnk( 16 );
   p_mask   = strblnk( 16 );

   strcpy( phone,    "          " );
   strcpy( p_mask,   "(999) 999-9999" );

   fld[ 7 ] = fld_make( fmp, 7, 5, "Phone         ", 19,
					 p_mask, phone, FLD_ALPHA_NUMERIC );
   if ( fld[ 7 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 7" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the home phone for this person" );
   sf_message( fld[ 7 ], xx );
   xx = str_save( "Include the area code for this phone number" );
   sf_hlptag( fld[ 7 ], xx );





   birthday = strblnk( 10 );
   b_mask   = strblnk( 10 );

   strcpy( birthday, "        " );
   strcpy( b_mask,   "99/99/99" );

   fld[ 8 ] = fld_make( fmp, 7, 55, "Birthday", 64,
					 b_mask, birthday, FLD_ALPHA_NUMERIC );
   if ( fld[ 8 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 8" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the birthdate for this person in mm/dd/yy format" );
   sf_message( fld[ 8 ], xx );
   xx = str_save( "Use only two digits for the year" );
   sf_hlptag( fld[ 8 ], xx );
   sf_val( fld[ 8 ], val_date );



   amount   = strblnk( 8 );
   a_mask   = strblnk( 8 );


			  /* This field is justified.  The actual     */
			  /* data string must NOT include space for   */
			  /* the mask characters or the justification */
			  /* routine will shove the data out of view  */
			  /* to the right.  Trust me!		      */
   strcpy( amount,   "      " );
   strcpy( a_mask,   "$9999.99" );

   fld[ 9 ] = fld_make( fmp, 9, 25, "Balance   ", 36,
					 a_mask, amount, FLD_MONEY );
   if ( fld[ 9 ] == ( FIELDPTR ) NULL )
   {
      fld_err( str_save( "Out of memory for field 9" ) );
      exit( 1 );
   }
   xx = str_save( "Enter the this person's current account balance" );
   sf_message( fld[ 9 ], xx );
   xx = str_save( "Deduct any current payments" );
   sf_hlptag( fld[ 9 ], xx );


				 /* This loop will set all the field  */
				 /* attributes, Active, Inactive and  */
				 /* prompt.			      */
   for ( i = 0; i <= 9; i ++ )
   {
      sf_active( fld[ i ], mk_att( RED, BROWN + BRIGHT ) );
      sf_inactive( fld[ i ], mk_att( RED, WHITE ) );
      sf_patt( fld[ i ], mk_att( BLUE, BROWN + BRIGHT ) );
   }


			   /* Clear the form.  It is possible to      */
			   /* preassign data to the fields by placing */
			   /* the data in the string when using the   */
			   /* strblnk function ( see last name field )*/
			   /* above.  Without the fm_clear this would */
			   /* appear in the field inititally.  Later  */
			   /* versions of the library will contain    */
			   /* more sophisticated ways of controlling  */
			   /* default and initial data display.       */
   fm_clear( fmp );

			   /* This function does the entire form      */
			   /* processing.  It actually consists of two*/
			   /* separate parts, a form processor and a  */
			   /* field processor although this is hidden */
			   /* form the user.  Hooks are provided for  */
			   /* user use.  There are ways to perform a  */
			   /* user function call before or after a    */
			   /* field is processed and also each time a */
			   /* key is pressed inside a field.  In      */
			   /* addition the user may build a field     */
			   /* validation function which may be called */
			   /* before a field is exited. 	      */
   fm_proc( fmp );


   cur_off();	     /* Turn off the cursor to avoid screen clutter   */


			 /* Display a window with the form data in it */

   wn1 = wn_make( 0, 0, 24, 80 );
   wn_wnclr( wn1, mk_att( RED, MAGENTA + BRIGHT ) );
   wn_bclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_clear( wn1 );

   wn_mkbdr( wn1, BDR_DLNP );

   wn_dsply( wn1 );


   wn_printf( wn1,
    "                Unicorn Software Data Entry System 1.00" );
   wn_printf( wn1, "\n\n" );

   wn_printf( wn1,
    "                     Readout of Data Entry Fields\n\n" );


   wn_printf( wn1,
    "                      Raw unformatted data only\n\n\n\n" );

   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );


   wn_printf( wn1, "\t\t\tThe record number is - <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", rec );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );

   wn_printf( wn1, "\t\t\tThe first name is ---- <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", fname );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );

   wn_printf( wn1, "\t\t\tThe last name is ----- <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", lname );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );


   wn_printf( wn1, "\t\t\tThe address is ------- <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", addr );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );


   wn_printf( wn1, "\t\t\tThe city is ---------- <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", city );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );

   wn_printf( wn1, "\t\t\tThe state is --------- <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", state );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );

   wn_printf( wn1, "\t\t\tThe zip code --------- <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", zip );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );

   wn_printf( wn1, "\t\t\tThe phone number ----- <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", phone );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );

   wn_printf( wn1, "\t\t\tThe birthday is ------ <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", birthday );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );

   wn_printf( wn1, "\t\t\tThe amount is -------- <" );
   wn_wnclr( wn1, mk_att( RED, WHITE + BRIGHT ) );
   wn_printf( wn1, "%s", amount );
   wn_wnclr( wn1, mk_att( RED, BROWN + BRIGHT ) );
   wn_printf( wn1, ">.\n" );


   wn_wnclr( wn1, mk_att( RED, BLUE + BRIGHT ) );

   wn_printf( wn1, "\n\n\n\t\t\t\t\t\tPress any key to continue" );


   uc_key();


   wn_hide( wn1 );
   wn_hide( wnd_message );
   wn_hide( fmp );

   wn_hide( wn_title );

   wn_wnclr( wn_main, mk_att( BLACK, BLUE + BRIGHT ) );


   wn_printf( wn_main,
    "\n   We hope you have enjoyed working with this demo of the current\n" );

   wn_printf( wn_main,
    "Unicorn Data entry system.  Not all of the capabilities have been\n" );

   wn_printf( wn_main,
    "demonstrated but there is enough to get you started building your\n" );

   wn_printf( wn_main,
    "own data forms.  Many enhancements are planned in future releases\n" );

   wn_printf( wn_main,
    "that will add new capabilities and make the system easier and more\n" );

   wn_printf( wn_main,
    "versitile to use.\n" );

   wn_wnclr( wn_main, mk_att( BLACK, BROWN + BRIGHT ) );

   wn_printf( wn_main, "\n\n\t\t\t\t\tPress any key to return to DOS" );

   uc_key();

   wn_hide( wn_main );

   cur_on();
   cur_restore();
}




int before6( char *data )
{
   WINDOWPTR   wnd_before6;

   cur_off();
   wnd_before6 = wn_make( 0, 0, 8, 80 );
   wn_wnclr( wnd_before6, mk_att( RED, WHITE + BRIGHT ) );
   wn_bclr( wnd_before6, mk_att( RED, BROWN + BRIGHT ) );
   wn_clear( wnd_before6 );
   wn_margin( wnd_before6, 2 );
   wn_mkbdr( wnd_before6, BDR_DLNP );
   wn_printf( wnd_before6,
   "This is an example of a before field function.  These functions may\n" );
   wn_printf( wnd_before6,
   "be used to inititalize data or for other purposes you may find.\n" );
   wn_printf( wnd_before6,
   "Before field functions are passed a pointer to the current field data\n" );
   wn_printf( wnd_before6,
   "which is <%s>.\n", data );
   wn_printf( wnd_before6,
   "\t\t\t\t\t   Press any key to continue." );
   wn_dsply( wnd_before6 );
   uc_key();
   wn_delete( wnd_before6 );
   cur_on();
}





int after6( char *data )
{
   WINDOWPTR   wnd_after6;

   cur_off();
   wnd_after6 = wn_make( 0, 0, 8, 80 );
   wn_wnclr( wnd_after6, mk_att( RED, WHITE + BRIGHT ) );
   wn_bclr( wnd_after6, mk_att( RED, BROWN + BRIGHT ) );
   wn_clear( wnd_after6 );
   wn_margin( wnd_after6, 2 );
   wn_mkbdr( wnd_after6, BDR_DLNP );
   wn_printf( wnd_after6,
   "This is an example of a after field function.  These functions may\n" );
   wn_printf( wnd_after6,
   "be used to manipulate the data or for other purposes you may find.\n" );
   wn_printf( wnd_after6,
   "After field functions are passed a pointer to the current field data\n" );
   wn_printf( wnd_after6,
   "which is <%s>.\n", data );
   wn_printf( wnd_after6,
   "\t\t\t\t\t   Press any key to continue." );
   wn_dsply( wnd_after6 );
   uc_key();
   wn_delete( wnd_after6 );
   cur_on();
}
