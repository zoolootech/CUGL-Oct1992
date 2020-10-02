/*
*****************************************************************************

   IBM Graphics Printer Setup Program - Requires ANSI.SYS to operate


   Future enhancements: 
      Add a locate() function to operate just like color().  It will run
         slower and require two printf's, but will be more modular,
         and will convert more easily to using BIOS external functions.
      Change paint_page_1 to return two things: the option chosen (so that
         it can be passed to paint_page_2), and an indicator to tell whether
         or not to call paint_page_2.

*****************************************************************************
*/

/*
*****************************************************************************

   C header files
   These identify library routines.  LINT_ARGS must be defined first.

*****************************************************************************
*/

#define LINT_ARGS = 1      /* value doesn't matter, only definition does   */

#include <conio.h>         /* for console i/o                              */
#include <dos.h>           /* for DOS interrupt calls                      */
#include <stdio.h>         /* for standard i/o                             */
#include <string.h>        /* for string functions                         */

/*
*****************************************************************************

   Macros

*****************************************************************************
*/

/* This macro returns the number of elements of its array argument         */

#define SIZE(x)         ( sizeof(x) / sizeof( x[0] ) )

/* These macros validate the parameters to the color function              */

#define VALID_FG(X)  ( ( X <= 7 && X > 0 ) ? (X) : (7) )
#define VALID_BG(X)  ( ( X <= 7 && X > 0 ) ? (X) : (0) )

/*
*****************************************************************************

   Constants and global variables

*****************************************************************************
*/

#define CHAR_H         "H"           /* ANSI command for cursor placement  */
#define CHAR_m         "m"           /* ANSI command for setting color     */
#define ESCAPE_BRACKET "\x1b["       /* First part of ANSI screen command  */
#define SEMICOLON      ";"
#define CLEAR_SCREEN   "\x1b[2J"     /* ANSI clear screen                  */
#define BLACK          0             /* ANSI black                         */
#define RED            1             /* ANSI red                           */
#define GREEN          2             /* ANSI green                         */
#define YELLOW         3             /* ANSI yellow                        */
#define BLUE           4             /* ANSI blue                          */
#define MAGENTA        5             /* ANSI magenta (purple)              */
#define CYAN           6             /* ANSI cyan (lt blue)                */
#define WHITE          7             /* ANSI white                         */
#define MAX_HISTORY   18             /* The max nbr characters history kept*/
                                     /*    so that option_history_prt      */
                                     /*    string is not busted            */
#define MAX_HISTORY_PRT  25          /* The max size of the history string */

char *option_hist_loc[] = { "24", "43" };
char option_history[25] = "";
char two_spaces[3]      = "  ";
char option_history_prt[26] = "";

/*
*****************************************************************************

   main() - Beginning of the code 

*****************************************************************************
*/

main()

{

   int option;
   int repaint_page_1 = 1;

   color( WHITE, BLACK );
   option = paint_page_1( repaint_page_1 );
   repaint_page_1 = 0;

   while ( option ) {
      if ( option == 14 || option == 15 ) {
         paint_page_2( option );
         repaint_page_1 = 1;
      }
      option = paint_page_1( repaint_page_1 );
      repaint_page_1 = 0;
   }

   return;
}

/*
******************************************************************************

 paint_page_1()   Displays the text for option page

******************************************************************************
*/

int paint_page_1(repaint_page_1)

int repaint_page_1;

{
   register int i;
   register int j;

   static char option_resp[5];       /* holds keyboard response             */
   static int  nbr_cur_options = 20; /* number options currently            */
   static int  option;               /* option chosen                       */
   static char *cmp_ptr;             /* pointer to command chosen           */
   static char *temp_ptr;            /* use when shrinking history          */

   static char *option_resp_loc[] = { "24", "18" };
   static char *input_resp;

   static char *pg1_row_loc[] = {
       "2",  "4",  "4",
       "6",  "6",  "7",  "7",  "8",  "8",  "9",  "9",
      "10", "10", "11", "11", "12", "12", "13", "13", "14", "14", "15", "15",
      "20", "22", "24", "24", ""
   };

   static char *pg1_col_loc[] = {
      "20",  "8", "39",
      "10", "40", "10", "40", "10", "40", "10", "40",
      "10", "40", "10", "40", "10", "40", "10", "40", "10", "40",  "9", "40",
       "8", "10", "10", "25"
   };

   static char *pg1_opt_msg[] = {
      "IBM Graphics Printer Setup Program",
      "Opt   Description",
      "Opt   Description",
      "1 = 1/8\x22 Spacing",
      "11 = Start Double Strike",
      "2 = 1/6\x22 Spacing",
      "12 = End Double Strike",
      "3 = 6 Spaces Over Perforation",
      "13 = FF (Hold Cmds till done)",
      "4 = Cancel Skip Perforation",
      "14 = Variable Line Feeding",
      "5 = Start Compressed",
      "15 = Variable Perf Skipping",
      "6 = End Compressed",
      "16 = 1/8\x22, Skip 6, Compressed",
      "7 = Start Emphasized",
      "17 = 1/6\x22, No Skip, No Compressed",
      "8 = End Emphasized",
      "18 = Reset (Sets Top of Form",
      "9 = Start Unidirectional",
      "19 = Start Correspondence Quality",
      "10 = End Unidirectional",
      "20 = Stop Correspondence Quality",
      "Please Enter Options One At A Time, Each Followed By An Enter <\xc4\xd9",
      "(To Terminate Program, Press Enter By Itself)",
      "Option:",
      "Previous Choices:"
   };

   static char *pg1_prn_cmds[] = {
      "0",
      "1\x1b0",
      "1\x1b3\x24",
      "1\x1bN\x06",
      "1\x1bO",
      "1\x0f",
      "1\x12",
      "1\x1bE",
      "1\x1bF",
      "1\x1bU\x01",
      "1\x1bU\x00",
      "1\x1bG",
      "1\x1bH",
      "1\x0c",
      "0",
      "0",
      "1\x1b0\x0f\x1bN\x06",
      "1\x1b3\x24\x12\x1bO",
      "0",
      "1\x1bX1",
      "1\x1bXO"
   };

   if ( repaint_page_1 ) {
      printf ("%s",CLEAR_SCREEN);
      color( GREEN, BLACK );
      for ( i = 0 ; i <= 2 ; i++ )
         printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, pg1_row_loc[i], SEMICOLON,
                                  pg1_col_loc[i], CHAR_H, pg1_opt_msg[i] );
      color( YELLOW, BLACK );
      for ( j = 0 ; j <= 19 ; j++ ) {
         printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, pg1_row_loc[i], SEMICOLON,
                                  pg1_col_loc[i], CHAR_H, pg1_opt_msg[i] );
         i++;
      }
      color( CYAN, BLACK );
      while ( *pg1_row_loc[i] ) {
         printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, pg1_row_loc[i], SEMICOLON,
                                    pg1_col_loc[i], CHAR_H, pg1_opt_msg [i] );
         i++;
      }
   }

   /* print the option history                                             */
   printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, option_hist_loc[0], 
                            SEMICOLON, option_hist_loc[1], 
                            CHAR_H, option_history_prt );

   /* clear out the option input field                                     */
   printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, option_resp_loc[0], 
                            SEMICOLON, option_resp_loc[1], CHAR_H, two_spaces );

   /* position the cursor for option input                                 */
   printf ( "%s%s%s%s%s", ESCAPE_BRACKET, option_resp_loc[0], 
                          SEMICOLON, option_resp_loc[1], CHAR_H );

   /* get up to 2 characters, and convert to integer,                      */
   option_resp[0] = 3;              /* max nbr input chars, including <CR> */
   input_resp = cgets( option_resp );
   option = atoi( input_resp );
   /* process the option if it is valid                                    */
   if ( option > 0 && option <= nbr_cur_options ) {
      /* add a comma unless the string is null to start with               */
      if ( *option_history != '\0' ) {
         strcat ( option_history, "," );
      }
      /* add this input to the history                                     */
      manage_history( input_resp );
      /* if first byte of command is one, then execute the command         */
      cmp_ptr = pg1_prn_cmds[option];
      if ( strncmp( cmp_ptr, "1", 1 ) == 0 ) {
         cmp_ptr++;
         fputs( cmp_ptr, stdprn );
      }
   }
   return( option );
}

/*
******************************************************************************

 paint_page_2()   Displays the text for auxiliary option page

******************************************************************************
*/

int paint_page_2( option )

int option;

{
   register int i;
   register int j;

   static char amount_resp[6];       /* holds keyboard response             */
   static char option_string[5];     /* holds converted option              */
   static int  amount;               /* integer amount entered              */

   static char *amount_resp_loc[] = { "24", "18" };
   static char *option_cur_loc[]  = { "18", "51" };
   static char *amount_resp_ptr;
   static char *temp_ptr;            /* use when shrinking history          */
   static char *cmp_ptr;             /* pointer to command chosen           */

   static char *pg2_row_loc[] = {
       "2",  "4",  "5",  "6",  "7",  "9", "10", "11", "12", "18",
      "20", "22", "24", "24", ""
   };

   static char *pg2_col_loc[] = {
      "20",  "5", "10", "10", "10",  "5", "10", "10", "10", "10", "5",
      "10", "10", "25"
   };

   static char *pg2_opt_msg[] = {
      "Auxiliary Input Screen",
      "For Variable Line Feeding: (Option 14)",
      "Enter a number between 1 and 255.  The line feed will be adjusted",
      "to become the entered number / 216 of an inch.  For example, for",
      "1/4\x22 spacing enter 54 (54/216 = 1/4).",
      "For Variable Skipping Over Perforation: (Option 15)",
      "Enter a number between 1 and 127.  The number of lines skipped",
      "is subtracted from the number of lines on the page to determine",
      "the number of lines to print before the skip is performed.",
      "The Option Currently Being Processed is:",
      "Please Enter The Number Desired, Following It By An Enter <\xc4\xd9",
      "(To Return To The Option Screen, Press Enter By Itself)",
      "Amount:",
      "Previous Choices:"
   };

   static char *pg2_prn_cmds[] = {
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "0",
      "1\x1b3\xff",
      "1\x1bN\x7f",
      "0",
      "0",
      "0",
      "0",
      "0"
   };

   itoa( option, option_string, 10 );
   printf ( "%s", CLEAR_SCREEN );
   color( GREEN, BLACK );
   for ( i = 0 ; i <= 0 ; i++ ) {
      printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, pg2_row_loc[i], SEMICOLON,
                               pg2_col_loc[i], CHAR_H, pg2_opt_msg[i] );
   }
   color( YELLOW, BLACK );
   for ( j = 0 ; j <= 8 ; j++ ) {
      printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, pg2_row_loc[i], SEMICOLON,
                               pg2_col_loc[i], CHAR_H, pg2_opt_msg[i] );
      i++;
   }
   printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, option_cur_loc[0], SEMICOLON,
                            option_cur_loc[1], CHAR_H, option_string );
   color( CYAN, BLACK );
   while ( *pg2_row_loc[i] ) {
      printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, pg2_row_loc[i], SEMICOLON,
                               pg2_col_loc[i], CHAR_H, pg2_opt_msg [i] );
      i++;
   }

   /* print the option history                                             */
   printf ( "%s%s%s%s%s%s", ESCAPE_BRACKET, option_hist_loc[0], 
                            SEMICOLON, option_hist_loc[1], 
                            CHAR_H, option_history_prt );

   printf ( "%s%s%s%s%s", ESCAPE_BRACKET, amount_resp_loc[0], 
                          SEMICOLON, amount_resp_loc[1], CHAR_H );

   amount_resp[0] = 4;              /* max nbr input chars, including <CR> */
   amount_resp_ptr = cgets( amount_resp );
   amount = atoi( amount_resp_ptr );
   /* process the amount if it is valid                                    */
   if ( amount > 0 && amount < 256 ) {
      /* add a comma since string should not be null from option logic     */
      strcat ( option_history, "," );
      /* if too much history, save most recent choices                     */
      manage_history( amount_resp_ptr );
      /* if first byte of command is zero, then execute the command        */
      *( pg2_prn_cmds[option] + 3 ) = (char)amount;
      cmp_ptr = pg2_prn_cmds[option];
      if ( strncmp( cmp_ptr, "1", 1 ) == 0 ) {
         cmp_ptr++;
         fputs( cmp_ptr, stdprn );
      }
   }
   return;
}

/*
******************************************************************************

 color()   Sets the screen color with ANSI.SYS

******************************************************************************
*/

color( fg, bg )
int fg, bg;

{
   static char *fg_ansi[] = {
      "30", "31", "32", "33", "34", "35", "36", "37"
   };

   static char *bg_ansi[] = {
      "40", "41", "42", "43", "44", "45", "46", "47"
   };

   fg = VALID_FG( fg );
   bg = VALID_BG( bg );

   printf ( "%s%s%s%s%s", ESCAPE_BRACKET, fg_ansi[fg], SEMICOLON,
                          bg_ansi[bg], CHAR_m );
   return;
}

/*
******************************************************************************

manage_history()  Keeps the most recent options and auxiliary input

   The 'cutting' off of the first option must occur as many times as is
      necessary to get the length down under the stated amount.  This is
      because we could be removing only two characters and adding three.
      Done over and over, the result is that we bust the string.

******************************************************************************
*/

manage_history(input_ptr)
char *input_ptr;

{
   register int j;
   char *temp_ptr;

   /* Cut oldest items off the history list till it is short enough        */
   while ( strlen( option_history ) > MAX_HISTORY ) {
      temp_ptr = strchr( option_history, ',' );
      temp_ptr++;
      for ( j = 0 ; ( option_history[j] = *temp_ptr ) != '\0' ; temp_ptr++, j++ )
         ;                             
   }
   /* add most recent response to history string                           */
   strcat ( option_history, input_ptr );
   strcpy( option_history_prt, option_history );
   /* fill out the print string to cover up anthing left on the string     */
   while( strlen( option_history_prt ) < MAX_HISTORY_PRT ) {
      strcat( option_history_prt, " " );
   }
   return;
}
