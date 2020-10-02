/*-----------------------------------------------------------------------------
Count the number of pieces of a given type or types.

Revision History
----------------
Gary Culp   7 Jan 1989     Initial version

-----------------------------------------------------------------------------*/
#include "othello.h"

/*
Function to count the number of pieces of a particular type or types.

Returns:
   count of matching pieces

Does not calculate the correct value for OFF_BOARD pieces.
*/
int
piece_count(struct board_struct *board_ptr, unsigned char piece_type)
{
   register unsigned char *p;
   register unsigned char *stop;
   int count;

   count = 0;
   stop =  &board_ptr->board[8][9];
   for (p = &board_ptr->board[1][1]; p < stop; p++) {
      if (*p & piece_type) {
         count++;
      }
   }
   return (count);
}

/*
Function to determine whether each player has a move on a given board.

Returns:
   The US_PIECE bit is set iff the board contains a move for the computer;
   and the THEM_PIECE bit is set iff the board contains a move for the human.

   So a return value of 0 means that the board represents the end of the game.
*/
unsigned char
who_can_move(struct board_struct *board_ptr)
{
   int aff_list[MAX_AFFECTED_PIECES];

   return (
         (find_move(board_ptr, 0, US_PIECE, aff_list) ? US_PIECE : 0)
      |
         (find_move(board_ptr, 0, THEM_PIECE, aff_list) ? THEM_PIECE : 0)
   );
}
