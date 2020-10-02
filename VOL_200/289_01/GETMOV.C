/*-----------------------------------------------------------------------------
GETMOV.C

This file contains the functions that read the player's move, verifies the
move and displays the new screen.

Revision History
----------------
Jon Ward	10 Dec 1988	Initial Revision
Gary Culp	 2 Jan 1989	changed return type & value of check_for_input
				  added code to update curnt_bd
				  changed method of validating move
Jon Ward	 7 Jan 1989	Corrected a bug -- forgot to initialize 
				  curnt_move.
-----------------------------------------------------------------------------*/

#include <conio.h>
#include <ctype.h>

#include "othello.h"


/*-----------------------------------------------------------------------------
                        Local Variable Declarations
-----------------------------------------------------------------------------*/

STATIC int move_row;		/* entered row portion of move */
STATIC int move_col;		/* entered col portion of move */
STATIC int input_flag;		/* first pass for move logic flag */


/*-----------------------------------------------------------------------------
move_type check_for_input (void);

This function checks for and processes player input.  When the player presses
a key, it is read by this routine.  If the keystroke represents the termin-
ation of the move, the move is checked against the possible moves.  If it is
a valid move, the board is updated and displayed.

Return Values
-------------
   HASNT_MOVED: complete move not entered, yet.
   valid move:  complete move entered.
-----------------------------------------------------------------------------*/

move_type check_for_input ()
{
register int key;		/* key pressed */
register int stat;		/* status from key */

#define ST_STAT_OK	0	/* ok status -- do nothing */
#define ST_ROW_ERR	1	/* row already entered error */
#define ST_COL_ERR	2	/* col already entered error */
#define ST_BAD_KEY	3	/* bad key error */

/*-----------------------------------------------
First off, check to see if the human can make a
move.  If he can't, return the NO_MOVE_MASK.
-----------------------------------------------*/

if (input_flag)
  {
  input_flag = 0;

  if (!(who_can_move (&curnt_bd) & THEM_PIECE))
    {
    disp_player_cant_move (THEM_PIECE);
    disp_board (&curnt_bd);
    disp_player_move (US_PIECE);
    return (NO_MOVE_MASK);
    }

  disp_player_move (THEM_PIECE);
  }


/*-----------------------------------------------
-----------------------------------------------*/


if (kbhit () == 0)		/* if no key has been pressed, return */
  return (HASNT_MOVED);		/* move not completely entered */

key = getch ();			/* get key pressed */
key = toupper (key);		/* convert key to uppercase */

stat = ST_STAT_OK;		/* init status to OK */


/*-----------------------------------------------
-----------------------------------------------*/

if (key >= '1' && key <= '8')		/* is key a column number? */
  {
  if (move_col == -1)
    disp_col (move_col = key - '1');
  else
    stat = ST_COL_ERR;
  }

else if (key >= 'A' && key <= 'H')	/* is key a row number? */
  {
  if (move_row == -1)
    disp_row (move_row = key - 'A');
  else
    stat = ST_ROW_ERR;
  }

else switch (key)			/* check other keys */
  {
  case '\b':				/* back-space ? */
    move_col = move_row = -1;
    disp_clr_row_col ();
    break;

  default:				/* unrecognized key */
    stat = ST_BAD_KEY;
  }


/*-----------------------------------------------
-----------------------------------------------*/

switch (stat)
  {
  case ST_COL_ERR:
    disp_error_msg ("Column Already Entered");
    goto ERROR_WRAP_UP;

  case ST_BAD_KEY:
    disp_error_msg ("Invalid Key Entered");
    goto ERROR_WRAP_UP;

  case ST_ROW_ERR:
    disp_error_msg ("Row Already Entered");

  ERROR_WRAP_UP:
    disp_board (&curnt_bd);
    disp_player_move (THEM_PIECE);
    if (move_row != -1)
      disp_row (move_row);
    if (move_col != -1)
      disp_col (move_col);

  default:
    break;
  }


/*-----------------------------------------------
-----------------------------------------------*/

if (move_row != -1 && move_col != -1)
  {
  move_type curnt_move;

  curnt_move = 0;

  SET_ROW_COL (curnt_move, move_row, move_col);

  if (verify_move_and_update_board (curnt_move, THEM_PIECE))
    {
    move_row = move_col = -1;
    disp_error_msg ("Invalid Move.  You Can't Move There.");
    goto ERROR_WRAP_UP;
    }

  disp_player_move (US_PIECE);
  move_row = move_col = -1;
  return (curnt_move);			/* move completed and updated */
  }

return (HASNT_MOVED);			/* move not completely entered */
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void init_input_vars ()
{
move_row = -1;
move_col = -1;
input_flag = -1;
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

