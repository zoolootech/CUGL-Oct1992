/*  lib_demo.c
 *
 *  UCLIB 5.00 Demo program
 *
 */

#include <stdio.h>

#include "uc_defs.h"
#include "uc_glob.h"
#include "uc_proto.h"
#include "uc_key.h"

#include "wn_defs.h"
#include "wn_glob.h"	/* used for declaration of sound_on  */


/*
 *  There are not many comments in this program since the comments
 *  for each function are effect located in the reference guide.
 *  Please refer to that file as you go through this code.  It
 *  should provide you with a complete overview of each function
 *  and how it may be called and used.	Sorry but no complete
 *  manual exists for this version.  Registered users will receive
 *  a disk containing the test programs used by the author in
 *  developing the library.  If you feel a full manual would be
 *  of help drop a line to the author.	If enough such requests
 *  are received a complete manual will be considered for future
 *  versions of the library.
 *
 */


char   test_str[] = "     This is a test string     ";

char   tmp_str[ 100 ];


void main( int argc, char argv[]  )
{
   int	i;
   long il;
   char in_str[ 81 ];
   int	cstart;
   int	cstop;
   char *str_mon();
   int	back;
   int	fore;


   uc_init();		/* Inititalize various system variables */
   if ( mouse_exist )
      m_flagdec();

   if ( argc > 2 )
      sound_on = 0;

   cls_clr( mk_att( BLUE, WHITE + BRIGHT ) );


   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "\n\n                          ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n" );
   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "                          ³   Unicorn Software   ³\n" );
   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "                          ³     P.O. Box 3214    ³\n" );
   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "                          ³      Kirkland Wa     ³\n" );
   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "                          ³      98083-3214      ³\n" );
   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "                          ³    (206) 823-4656    ³\n" );
   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "                          ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n" );

   c_printf( mk_att( BLUE, MAGENTA + BRIGHT ),
   "             ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ\n\n" );

   c_printf( mk_att( BLUE, RED ),
   "                Welcome to version 5.00 of the Unicorn Library\n\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
 "\t\t   This demo program will illustrate " );


   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "some " );


   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "of the new library's\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
 "\t\t   capabilities.  It is impossible to show everyone of the " );

   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "over\n\t\t   300 functions " );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
 "contained in the library.  You might also try\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
 "\t\t   the programs `" );

   c_printf( mk_att( BLUE, GREEN ),
   "WINDOW.EXE" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "' and `" );

   c_printf( mk_att( BLUE, GREEN ),
   "TDATA.EXE" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "' which demonstrate\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
 "\t\t   the windowing and data entry portions of the library.      \n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
 "\t\t   At several points during the demo the program will stop and\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
 "\t\t   wait.  Whenever this occurs simply press a key to continue.\n" );

   uc_key();

   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
 "\n\t\t   There now that wasn't so hard!  Just remember to press a\n" );

   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
 "\t\t   key whenever the program has stopped.\n" );

   uc_key();

   cls_clr( mk_att( RED, WHITE + BRIGHT ) );

   c_printf( mk_att( RED, MAGENTA + BRIGHT ),
 "\n\t\tFirst we will look at the three primary screen control functions.\n\n" );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
 "\t1. " );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "A full color version of the standard printf function.  This\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\thas been used to provide all the screen output you have seen up\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\ttill now.  It works the same as the normal printf, accepting all\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\tthe usual printf formatting parameters with two exceptions.  It\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\trequires a first parameter that is the attribute desired and\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\tsecondly will not scroll the screen when the bottom line is\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\treached.  Other than this it works the same including a variable\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\tnumber of arguments.                                            \n" );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
 "\n\t2. " );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "A Clear screen function that allows clearing the screen to a\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\tparticular attribute.  It has been used to clear the screen in this\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\tdemo.                                                           \n" );



   c_printf( mk_att( RED, BROWN + BRIGHT ),
 "\n\t3. " );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "A full color version of gets which allows the use of user input\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\tusing a desired attribute or at least not destroying the current\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
 "\t\tscreen attibute.                                                \n" );

   uc_key();


   cls_clr( mk_att( CYAN, BROWN + BRIGHT ) );

   c_printf( mk_att( CYAN, BROWN + BRIGHT ),
   "\n\n\t\tNow try typing in a string with the c_gets function.\n\n" );

   i = c_gets( mk_att( CYAN, BLACK ), in_str );

   c_printf( mk_att( CYAN, BROWN + BRIGHT ),
   "\n\n\tThe string you typed had " );

   c_printf( mk_att( CYAN, RED + BRIGHT ),
    "%d", i );

   c_printf( mk_att( CYAN, BROWN + BRIGHT ),
    " characters and was\n\t" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
    "<%s>", in_str );

   c_printf( mk_att( CYAN, BROWN + BRIGHT ), "\n\n" );

   c_printf( mk_att( CYAN, BROWN + BRIGHT ),
   "\n\n\tDid you try to edit your string?  If not why not try again.\n\n" );

   i = c_gets( mk_att( CYAN, BLACK ), in_str );


   c_printf( mk_att( CYAN, BROWN + BRIGHT ),
   "\n\n\tThe string you typed had " );

   c_printf( mk_att( CYAN, RED + BRIGHT ),
    "%d", i );

   c_printf( mk_att( CYAN, BROWN + BRIGHT ),
    " characters and was\n\t" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
    "<%s>", in_str );

   c_printf( mk_att( CYAN, BROWN + BRIGHT ), "\n\n" );

   uc_key();
   if ( sound_on )
      uc_error();

   cls_clr( mk_att( GREEN, BLUE + BRIGHT ) );

   c_printf( mk_att( GREEN, BLUE + BRIGHT ),
   "\n\n\n\t\tAs you can see these three functions provide considerable\n" );

   c_printf( mk_att( GREEN, BLUE + BRIGHT ),
   "\t\tcontrol over how the screen appears and this degree of\n" );

   c_printf( mk_att( GREEN, BLUE + BRIGHT ),
   "\t\tcontrol is easy to achieve as you will see when you\n" );

   c_printf( mk_att( GREEN, BLUE + BRIGHT ),
   "\t\tlook at the source code for this demo.\n" );

   uc_key();
   if ( sound_on )
      uc_ok();

   c_printf( mk_att( GREEN, WHITE + BRIGHT ),
   "\n\n\t\tNext we will look at some of the functions designed\n" );

   c_printf( mk_att( GREEN, WHITE + BRIGHT ),
   "\t\tfor general string handling and manipulation.\n" );

   c_printf( mk_att( GREEN, WHITE + BRIGHT ),
   "\t\tThe source code for all of the demo programs is provided\n" );

   c_printf( mk_att( GREEN, WHITE + BRIGHT ),
   "\t\tto aid you in using the library functions.\n" );


   uc_key();
   if ( sound_on )
      uc_tone( 1000, .25 );

   cls_clr( mk_att( MAGENTA, BROWN + BRIGHT ) );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ),
   "\n\tNow we will take a look at some of the string handling functions.\n\n" );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ),
   "\tFor the purpose of this demo we will use the following string\n\t" );

   c_printf( mk_att( RED, BLUE + BRIGHT ),
   "%s", test_str );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ),
   " complete with the leading and\n" );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ),
   "\ttrailing spaces.\n\n" );

   uc_key();
   if ( sound_on )
      siren( 3 );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tFirst you might wish to strip the leading white space which would\n" );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tproduce the string '" );

   strcpy( tmp_str, test_str );
   i = del_lwh( tmp_str );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ), "%s", tmp_str );
   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), "'" );
   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), "  The original\n" );
   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tstring had a length of " );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ), "%d", strlen( test_str ) );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   " while the new string has a length of " );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ),
   "%d", i );
   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), ".\n\n" );

   strcpy( tmp_str, test_str );
   i = del_twh( tmp_str );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tYou might wish to strip the trailing white space instead which\n" );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\twould produce the string '" );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ), "%s", tmp_str );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "' with a length\n\tof " );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ), "%d", i );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), ".\n\n" );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tIt is also posible to remove ALL spaces in the string resulting in\n\t'" );

   strcpy( tmp_str, test_str );
   i = del_blnks( tmp_str );

   c_printf( mk_att( MAGENTA, BROWN + BRIGHT ), "%s", tmp_str );
   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), "'.\n" );



   uc_key();

   cls_clr( mk_att( BLUE, WHITE + BRIGHT ) );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\n\n\tLet's use the same test string for some further operations.\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tFirst we could change the case of every character in\n" );

   strcpy( tmp_str, test_str );
   i = chng_case( tmp_str );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t%s", test_str );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   " Which would produce\n\t" );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "%s", tmp_str );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   ".  We could also convert\n\t" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "the entire string to upper case\n\t" );

   strcpy( tmp_str, test_str );
   strtoup( tmp_str );

   c_printf( mk_att( GREEN, WHITE + BRIGHT ),
   "%s", tmp_str );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   " or we could convert the\n\t" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "entire string to lower case like this\n\t" );

   strcpy( tmp_str, test_str );
   strtolow( tmp_str );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "%s", tmp_str );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   ".\n\n\t" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "We could also shift the string to the right\n\t" );

   strcpy( tmp_str, test_str );
   shr_str( tmp_str );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "%s", tmp_str );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   ".\n\t" );


   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "It is also possible to right justify the string\n\t" );

   strcpy( tmp_str, test_str );
   str_rjst( tmp_str );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "%s", tmp_str );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   ".\n\t" );


   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "Or we could left justify the string like this\n\t" );

   strcpy( tmp_str, test_str );
   str_ljst( tmp_str );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "%s", tmp_str );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   ".\n\n\t" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "We can also reverse the order of a string like this\n\t" );

   strcpy( tmp_str, test_str );
   str_rev( tmp_str );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "%s", tmp_str );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   ".\n\n\t" );


   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "There are a variety of other string handling functions in the\n\t" );

   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "library.  Look at the reference guide for further details.\n\t" );


   uc_key();

   cls_clr( mk_att( RED, WHITE + BRIGHT ) );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\n\n\tNext we will look at some of the date and time formatting\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\troutines.  First we will take the date Saturday 10 Sept. 88 and find\n" );


   il = day2jul( 880910L );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\tthat it is julian day number %ld and ", il );

   i = dayofwk( il );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "it is day number %d in the week.\n", i );

   i = day_of_yr( 88, 9, 10 );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\tThis same day is also day number %d in the year.\n", i );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\tWe can also take the julian date and convert it \n", i );

   il = gregorian( il );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\tback to a gregorian date like --> '%ld'.\n", il );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\n\tThe current day is %s according to the internal clock\n", str_date() );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\tand the current time is %s while the week day is '%s'.\n",
		str_time(), wkday_str()  );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\tIf we use the date 880910 ( Sept. 10, 1988 ) we may easily.\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\tprint the month like this '%s'.\n", str_mon( 880910L ) );


   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\n\tThe dates are stored internally as YYYYMMDD or YYMMDD to\n" );

   c_printf( mk_att( RED, WHITE + BRIGHT ),
   "\tallow for easier sorting.\n" );

   uc_key();

   cls_clr( mk_att( BLUE, WHITE + BRIGHT ) );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\n\n\tWe can also change the cursor, even in response to a user key.\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tTry pressing the insert key.\n" );

   cstart = cur_start();
   cstop  = cur_stop();

   while ( uc_key() != K_INS )
   {
      c_printf( mk_att( RED, BLACK ),
      "\tPlease press the insert key.\n" );
      if ( sound_on )
	 uc_beep();
   }

   setcursh( 0, 6 );

   uc_key();

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tThe cursor shape may be set to any desired shape.\n" );

   setcursh( 0, 3 );
   uc_key();

   setcursh( 3, 6 );
   uc_key();

   setcursh( 3, 4 );
   uc_key();

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tThese shapes may be set by the program or in response to a user\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tor a condition within your progam.\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tThere are functions designed to detect virtually any key on the\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tbeyboard.  Most are defined in the keys header although actual\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tfunctions are used for some of the special keys.\n" );


   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tNow press any key to return the cursor to normal.\n" );

   setcursh( cstart, cstop );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tIn addition the cursor may be moved anywhere on the screen.\n" );

   gotorc( 10, 20, 0 );
   uc_key();

   gotorc( 12, 30, 0 );
   uc_key();

   gotorc( 14, 40, 0 );
   uc_key();

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\n\tThe cursor movement function includes a page parameter so\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tyou may move the cursor on a video page other than the one\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\tcurrently being viewed.\n" );


   uc_key();


   cls_clr( mk_att( CYAN, BLUE + BRIGHT ) );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\n\n\tOther groups of functions include a full set of printer control\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tfunctions for both the epson style and the Diablo style Daisy\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\twheel printers.  These functions allow almost total control\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tof output including character pitch on the dot matrix type.\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tIn addition there is a complete set of mouse control functions\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tnot included in this demo.  A later release will include mouse\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tcontrol for the menu functions to be found in version 2.00.\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\n\tThere are over 300 functions and a long list of macros included\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tin this library.  It is anticipated that the list will grow much\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tlonger by the time version 2.00 is released.\n" );


   uc_key();


   cls_clr( mk_att( RED, BROWN + BRIGHT ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\n\n\tFor the mathematical crowd we find a wide variety of math\n" );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\tfunctions that will grow larger in future versions of the library.\n" );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\tLet's look at the greatest common divisor function first.\n" );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\tThe greatest common divisor of 45 and 18 is %ld.\n", gcd( 45L, 18L ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\tThe greatest common divisor of 237451 and 557187 is %ld.\n",
			    gcd( 237451L, 557187L ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\tThe greatest common divisor of 5840931 and 28510683 is %ld.\n",
			    gcd( 5840931L, 28510683L ) );


   blk_str( "G C D", 15, 15, mk_att( RED, WHITE + BRIGHT ), 0 );

   uc_key();

   cls_clr( mk_att( MAGENTA, WHITE + BRIGHT ) );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\n\n\tThe are also a wide variety or area and volume functions\n\n\n" );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tThe area of a circle of radius 5 is %.14f\n", a_circle( 5.0 ) );


   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tThe area of an ellipse with major and minor radii of 3 and 5\n" );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), "\tis %.14f\n",
					a_ellipse( 3.0, 5.0 ) );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tThe lateral surface area of a cone of radius 5 and height 7\n" );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), "\tis %.14f\n",
						al_cone( 5.0, 7.0 ) );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tThe volume of a sphere of radius 3\n" );


   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), "\tis %.14f\n",
						     v_sphere( 3.0 ) );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ),
   "\tThe volume of a cone of radius 4 and height 9 is\n" );

   c_printf( mk_att( MAGENTA, WHITE + BRIGHT ), "\tis %.14f\n",
						v_cone( 4.0, 9.0 ) );

   uc_key();

   cls_clr( mk_att( CYAN, BLUE + BRIGHT ) );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\n\n\tFor those who need to handle check writing we have $245.67 =\n" );

   nbr2wrds( 245.67, tmp_str );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\t%s dollars.\n\n", tmp_str );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tBy the way the attribute used for this screen is %d or %x Hex.\n",
		    read_att( 0 ), read_att( 0 ) );

   unmk_att( read_att( 0 ), &back, &fore );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tThe current foreground color is %d and the background is %d.\n",
			      fore, back );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\n\tWe can also replace characters in a string; For example using\n" );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tThe string 'This is a test' let's replace 's' with 'S'\n" );

   strcpy( tmp_str, "This is a test" );

   replacech( tmp_str, 's', 'S' );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\twhich gives us '%s'\n", tmp_str );

   c_printf( mk_att( CYAN, BLUE + BRIGHT ),
   "\tThe letter 'S' occurs %d times in the above string.\n",
						countch( tmp_str, 'S' ) );

   uc_key();

   cls_clr( mk_att( RED, BROWN + BRIGHT ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\n\n\tFor the mundane task of unit conversion a variety of predefined\n" );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\tconversions are built into the library.\n\n\n" );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 inches   = %14.10f cm.\n", inch_cm( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 cm       = %14.10f inches.\n", cm_inch( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 degrees  = %14.10f radians.\n", deg_rad( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 radians  = %14.10f degrees.\n", rad_deg( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 ounces   = %14.10f grams.\n", oz_gm( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 grams    = %14.10f ounces. \n", gm_oz( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 gallons  = %14.10f liters. \n", gal_lt( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 liters   = %14.10f gallons.\n", lt_gal( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 lb       = %14.10f kg.     \n", lb_kg( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 kg       = %14.10f lb.     \n", kg_lb( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 mi       = %14.10f km.     \n", mi_km( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 km       = %14.10f mi.     \n", km_mi( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 meters   = %14.10f yards.  \n", m_yd( 5.0 ) );

   c_printf( mk_att( RED, BROWN + BRIGHT ),
   "\t\t\t\t\t5 yards    = %14.10f meters. \n", yd_m( 5.0 ) );


   uc_key();

   cls_clr( mk_att( BLUE, WHITE + BRIGHT ) );

   c_printf( mk_att( BLUE, BROWN + BRIGHT ),
   "\n\n\t\t\t\t\t\t\t\t\tGeneral Information\n\n\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tYou have seen some of the over 300 functions contained in the\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tlibrary in this demonstration program.  There are two other\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tdemo programs which you are encouraged to try 'WINDOW.EXE and\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tTDATA.EXE.  They demonstrate the Window and data entry packages\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tincluded in the library.  This library represents the work of\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\ta single author over a period of more than a year.  It is not\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tfinished and will be updated in the future.  The problem with\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tany software project is deciding when to quit.  In the case of\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tthis library an arbitrary decision was made to release this\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tversion in its present state and continue on to new versions\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tin the comming months.  We hope you find this library useful\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tin your programming efforts and if so you are encouraged to\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tregister your copy.  See the file register.doc for information\n" );

   c_printf( mk_att( BLUE, WHITE + BRIGHT ),
   "\t\tconcerning registration.\n" );

   c_printf( mk_att( BLUE, BLACK + BRIGHT ),
   "\n\t\t  Unicorn Software  P.O. Box 3214 Kirkland, WA 98083-3214.\n" );

   c_printf( mk_att( BLUE, BLACK + BRIGHT ),
   "\t\t                     David A. Miller\n" );

   c_printf( mk_att( BLUE, BLACK + BRIGHT ),
   "\t\t                     (206) 823-4656\n" );

   c_printf( mk_att( BLUE, BLACK + BRIGHT ),
   "\t\t                 No Collect calls Please\n" );


   uc_key();
   if ( sound_on )
      phasor( 2 );

   cls();
}
