/*-----------------------------------------------------------------------------
Move generator

Revision History
----------------
Gary Culp   14 Dec 1988    Initial version.
-----------------------------------------------------------------------------*/
#include "othello.h"

/*
Given a board, a starting cell in that board, the color of the player whose
turn it is, and an array of ints for listing affected pieces,
find a move & build the list of pieces affected by that move.
The first entry in the list of affected pieces is the new piece.

Pieces are listed in the affected_list by the linear offset
of the piece within the .board array.

Returns:
   number of affected pieces (0 if no move was found)

   affected_list will contain the list of affected pieces.  This function
   also uses affected_list as a workspace, so even "unused" elements of
   affected_list will be changed upon return.
*/
int
find_move(
   struct board_struct *board_ptr,
   int start_displacement,
   unsigned char movers_color,
   int *affected_list)
{
   unsigned char *cell_ptr;
   register unsigned char *p;
   register int *aff_ptr;
   int *aff_hold;
   unsigned char hostile_color;
   unsigned char *stop;
   int axis_num;
   int delta;
   int pass;

   hostile_color = (US_PIECE | THEM_PIECE) ^ movers_color;
   stop = &board_ptr->board[8][9];
   aff_ptr = affected_list + 1;

   /* piece loop */
   for (cell_ptr = &board_ptr->board[0][0] + start_displacement; ; cell_ptr++)
   {
      /* find an empty cell */
      while (!BELONGS(*cell_ptr, NO_PIECE)) {
         if (++cell_ptr >= stop) {
            return (0);    /* no move was found */
         }
      }

      /* for each direction (i.e., + and - along each axis) */
      for (axis_num = BD_NDX; axis_num <= V_NDX; axis_num++) {
         delta = delta_array[axis_num];
         for (pass = 0; pass < 2; pass++, delta = -delta) {
            aff_hold = aff_ptr;  /* remember where we are in list */
            p = cell_ptr;
            while (BELONGS(*(p += delta), hostile_color)) {
               *aff_ptr++ = p - &board_ptr->board[0][0];
            }
            if (!BELONGS(*p, movers_color)) {
               /* Rip out the pieces we just put in the list;
                  they wouldn't be captured by this move.
               */
               aff_ptr = aff_hold;
            }
         }
      }

      if (aff_ptr - affected_list > 1) {
         *affected_list = cell_ptr - &board_ptr->board[0][0];
         return (aff_ptr - affected_list);
      }
   }
}
