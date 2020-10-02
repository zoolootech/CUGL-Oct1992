/*-----------------------------------------------------------------------------
Calculate the score for a board.

Revision History
----------------
Gary Culp   15 Dec 1988    Initial version.
Gary Culp   29 Jan 1989    Added code to discourage giving the opponent
                           a bridge to the corner.
-----------------------------------------------------------------------------*/

#include "othello.h"

struct special_score_struct {
   int offset;
   unsigned char flags;
};

/*
Calculate the score for a board.
The board must have already been updated.
*/
int
bd_eval(struct board_struct *board_ptr,
        unsigned char which_color)
{
   register int score = 0;

   {
      register unsigned char *cell_ptr;
      unsigned char *stop;

      stop = &board_ptr->board[8][9];
      for (cell_ptr = &board_ptr->board[1][1]; cell_ptr < stop; cell_ptr++) {
         switch (*cell_ptr & (unsigned char)(US_PIECE | THEM_PIECE)) {

         case US_PIECE :
            score++;
            if (IS_PERM(*cell_ptr)) {
               score += 15;
            }
            break;

         case THEM_PIECE :
            score--;
            if (IS_PERM(*cell_ptr)) {
               score -= 15;
            }
            break;
         }
      }
   }

   {
      register struct special_score_struct *ssptr;
      static struct special_score_struct avoid_adj_corner[12] = {
         {12, H_AX  | US_PIECE | THEM_PIECE},
         {21, V_AX  | US_PIECE | THEM_PIECE},
         {22, BD_AX | US_PIECE | THEM_PIECE},

         {17, H_AX  | US_PIECE | THEM_PIECE},
         {27, FD_AX | US_PIECE | THEM_PIECE},
         {28, V_AX  | US_PIECE | THEM_PIECE},

         {71, V_AX  | US_PIECE | THEM_PIECE},
         {72, FD_AX | US_PIECE | THEM_PIECE},
         {82, H_AX  | US_PIECE | THEM_PIECE},

         {77, BD_AX | US_PIECE | THEM_PIECE},
         {78, V_AX  | US_PIECE | THEM_PIECE},
         {87, H_AX  | US_PIECE | THEM_PIECE}
      };

      for (ssptr = &avoid_adj_corner[0];
           ssptr < &avoid_adj_corner[
              sizeof(avoid_adj_corner)/sizeof(struct special_score_struct)];
           ssptr++)
      {
         switch (board_ptr->board[0][ssptr->offset] & ssptr->flags) {
         case US_PIECE :
            /* We have a piece adjacent to a corner, which is not protected
               along the axis it shares with the corner. This is a risky
               situation, so we deduct some points for it.
            */
            score -= 5;
            break;
         case THEM_PIECE :
            score += 5;
            break;
         }
      }
   }

   return (which_color == US_PIECE ? score : -score);
}

