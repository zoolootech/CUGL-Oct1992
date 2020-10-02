/*-----------------------------------------------------------------------------
MOVEIT.C

This file contains a function that verifies a move, updates the current board
to reflect the changes, and displays that board.

Revision History
----------------
Jon Ward	 3 Jan 1989	Initial Revision
Jon Ward	 7 Jan 1989	Modification to handle NO_MOVE_MASK condition.
-----------------------------------------------------------------------------*/

#include "othello.h"


/*-----------------------------------------------------------------------------
int verify_move_and_update_board (
  move_type move,
  unsigned char player);

This function verifies the validity of the move specifiec by the move argument
and, if valid, updates the current board and displays it.

Return Value
------------
      0		move verified and board was updated and displayed.
     -1		move in error - board NOT updated or displayed.
-----------------------------------------------------------------------------*/

int verify_move_and_update_board (
  move_type move,			/* move we wish to make */
  unsigned char player)			/* player making the move */
{
int num_affected;			/* # of pieces that changed */
int aff_list [MAX_AFFECTED_PIECES];	/* array for changed pieces */
int disp;				/* displacement of the move */

/*------------------------------------------------
Calculate the displacement into the board for the
move being made and get the list of pieces that
will be affected for the first move at or
following that particular move.
------------------------------------------------*/

if (move & NO_MOVE_MASK)	/* if they pass NO_MOVE */
  SET_ROW_COL (move,0,0);	/* start scan at top left cell */

disp = &curnt_bd.board [1+GET_ROW(move)][1+GET_COL(move)] -
	&curnt_bd.board [0][0];

num_affected = find_move (&curnt_bd, disp, player, aff_list);


/*---------------------------------------
If no pieces were affected, see if this
is a no move condition.
---------------------------------------*/

if (num_affected == 0)
  {
  if (!(move & NO_MOVE_MASK))
    return (-1);
  }


/*---------------------------------------
If there was a valid moves, check to see
if it was the one they entered.
---------------------------------------*/

else
  {
  if (aff_list[0] != disp || move & NO_MOVE_MASK)
    return (-1);

  handle_changed_pieces (&curnt_bd, aff_list, num_affected, player, 0);
  }


disp_board (&curnt_bd);

if (!(move & NO_MOVE_MASK))
  {
  disp_player_moved_to (player);
  disp_row (GET_ROW (move));
  disp_col (GET_COL (move));
  }

return (0);
}

