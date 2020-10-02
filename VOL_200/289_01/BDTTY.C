/*-----------------------------------------------------------------------------
BDTTY.C

This file contains the board display routines for TTY type terminals.


Revision History
----------------
Jon Ward	24 Nov 1988	Initial Revision
Jon Ward	10 Dec 1988	Added more functions for displaying game info.
Jon Ward	 7 Jan 1989	Added player_moved_to function. Consolidated
				  functions that separately handled us/them.
-----------------------------------------------------------------------------*/

#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "othello.h"


/*-----------------------------------------------------------------------------
		     Global Varaible Declarations
-----------------------------------------------------------------------------*/
char us_char = 'X';	/* character to put on the board when WE move */
char them_char = 'O';	/* Character to put on the board when THEY move */


/*-----------------------------------------------------------------------------
                        Local Variable Declarations
-----------------------------------------------------------------------------*/
char bd_top [] = "     1   2   3   4   5   6   7   8  ";

#if 0		/*** use non-zero for REAL TTY output ***/
char bd_div [] = "   |---+---+---+---+---+---+---+---|";
char bd_row [] = "   |   |   |   |   |   |   |   |   |";

#else		/*** else, output as IBM PC characters ***/
char bd_div [] = "   \xC5\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC5\
\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC5\
\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC5";

char bd_row [] = "   \xB3   \xB3   \xB3   \xB3   \xB3   \
\xB3   \xB3   \xB3   \xB3";
#endif


/*-----------------------------------------------------------------------------
                         Local Constant Definitions
-----------------------------------------------------------------------------*/
#define BD_LET 1	/* position in bd_row where the letter goes */
#define BD_OFS 5	/* position in bd_row where pieces start */
#define BD_LEN 4	/* distance between pieces on board */


/*-----------------------------------------------------------------------------
void disp_board (board_type *board)

This function will display the "us" and "them" pieces for the board pointed
to by the board argument.
-----------------------------------------------------------------------------*/

void disp_board (board_type *board)	/* pointer to board to display */
{
register int r;
register int c;
char buf [81];

memset (buf, '-', sizeof (buf) - 1);	/* gen a line of dashes */
buf [sizeof (buf) - 1] = '\0';		/* terminate it with a NULL */

printf ("\n%s\n\n%s\n%s\n", buf, bd_top, bd_div);

for (r = 1; r <= 8; r++)
  {
  bd_row [BD_LET] = (char) (r + 'A' - 1);
  for (c = 1; c <= 8; c++)
    {
    char piece;

    switch (board->board [r][c] & (US_PIECE | THEM_PIECE))
      {
      case US_PIECE:
	piece = us_char;
	break;

      case THEM_PIECE:
	piece = them_char;
	break;

      default:
	piece = ' ';
	break;
      }

    bd_row [BD_OFS + (c-1) * BD_LEN] = piece;
    }						/* END for (c = 1... */

  printf ("%s\n%s\n", bd_row, bd_div);
  }						/* END for (r = 1... */

printf ("\n");
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void disp_row (int row)
{
printf ("%c", 'A' + row);
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void disp_col (int col)
{
printf ("%c", '1' + col);
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void disp_clr_row_col ()
{
printf ("\r  \r");
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void disp_init ()
{
printf ("\n\n\n");
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void disp_error_msg (char *msg)
{
printf ("\n\aError: %s\nPress any key to continue\n", msg);
getch ();
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void disp_player_move (unsigned char player)
{
printf ((player & US_PIECE) ? "\nOne Moment While I Move...\n" :
	"\nEnter Your Move\a\n");
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void disp_player_cant_move (unsigned char player)
{
printf ("\n%s Can't Move\n", (player & US_PIECE) ? "I" : "You");
printf ("Press any key to continue\n");
getch ();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void disp_player_moved_to (unsigned char player)
{
printf ("%s Moved To: ", (player & US_PIECE) ? "I" : "You");
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void disp_pieces (int us, int them)
{
printf ("\nI have %d pieces.  You have %d pieces.\n", us, them);

if (us < them)
  printf ("You won.  Play again -- you won't be so lucky next time.\n");
else if (us > them)
  printf ("I won. -- Read 'em and weep!\n");
else
  printf ("The game was a tie.\n");
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

