/*-----------------------------------------------------------------------------
Update board

Revision History
----------------
Gary Culp   3-14 Dec 1988  Initial version.
Gary Culp   19 Dec 1988    Added prot_check_flag to handle_changed_pieces, so
                           that the display routines can use it, too.
-----------------------------------------------------------------------------*/

/*
Glossary terms:        ***!!!
   permanent  (piece)
   axis
   hostile    (piece or color)
   friendly   (piece or color)
   off-board  (piece or color)

A common piece of advice to Othello players is to take the corners of the
board, because they can never be captured from you.  Not only can you depend
upon their contributing to your final count of pieces, they make a good
solid foundation upon which to build.  Something I haven't heard pointed out
though, is that corners aren't the only pieces that can't be captured.
I call any piece that can't be captured a permanent piece.  My strategy when
playing Othello is to acquire as many permanent pieces as I can; and that is
the strategy implemented in this game.  The most challenging part of this
project, for me, was figuring out an efficient algorithm for determining which
pieces in a given board configuration are permanent.  The answer was obvious
(after several hours of intense thought :) ).
This file contains the code which implements the algorithm.

To capture a piece (or line of pieces), P, the opponent must place 
his pieces, O, on opposite sides of P.
There are 4 axes through which this may be done:

horizontal   vertical   forward    backward
                        diagonal   diagonal
               O             O      O
 O P O         P           P          P
               O         O              O

If a piece is protected from being captured along all 4 axes, it cannot be
captured at all, and is therefore permanent.

A piece is protected along an axis if one of these conditions is true:

1)  The axis is full.
2)  One of the adjacent pieces along the axis is a friendly permanent piece.
3)  Both of the adjacent pieces along the axis are permanent.  (It doesn't
    matter what colors they are.)

For these rules for protection along an axis to work, we have imaginary
squares which lie off the edge of the board (that's why the boards are
manipulated as 10x10 instead of 8x8).  These imaginary squares contain
permanent pieces of a third color: "off-board".  Off-board is considered
to be a friendly color, for both players.  It may seem weird to add all
this imaginary stuff to the game, but it greatly simplifies handling the
edge of the board.  Off-board pieces make the code simpler, smaller,
and faster, at the expense of making the board data structure larger.

When to check protection and permanence:

   When a piece is played or its color is changed, all 4 of its axes must be
   checked for protection (previous protection may be invalidated by color
   change).

   When a protection bit that was clear is set, the piece must be checked
   for permanence.

   When a piece becomes permanent, the pieces adjacent to it must be checked
   for protection along the axis containing the newly permanent piece.
*/


#include "othello.h"


/* These definitions aren't used anywhere else.  They are part of a trick
   for determining whether the protected axis rules are satisfied. (The
   rules which deal with adjacent pieces being permanent, not the rule
   about a full axis.)
   Note that TRICK_PROTECTED == TRICK_ADJ1_PERM | TRICK_ADJ2_PERM.
*/
#define TRICK_ADJ1_PERM    1 
#define TRICK_ADJ2_PERM    2
#define TRICK_PROTECTED    3

/*--------------------------------*/
/*          Prototypes            */
/*--------------------------------*/

void handle_changed_pieces(
   struct board_struct *board_ptr,
   int *affected_list,
   int num_affected,
   unsigned char new_color,
   int prot_check_flag);

void new_piece_axis_fill_check(
   struct board_struct *board_ptr,
   int *affected_list);

void perform_all_protection_checks(struct board_struct *board_ptr);

unsigned char is_filled_axis(unsigned char *new_piece_ptr, int axis_num);

void request_prot_check(
   struct board_struct *board_ptr, /* pointer to board structure */
   unsigned char *cell_ptr,        /* pointer to cell */
   unsigned char requested_axes);  /* requesting prot check for these axes */

int next_check(int *row_num_ptr, int *clm_num_ptr, int *axis_num_ptr);

/*--------------------------------*/
/*             Variables          */
/*--------------------------------*/

/* To convert axis flags to axis numbers, shift the axis flag right 4 bits;
   use the result as an index into this table.
*/
static const unsigned char bit_priority_encode[] = {
   0, /* [0000] */ /* actually, no bit set */
   0, /* [0001] */
   1, /* [0010] */
   1, /* [0011] */
   2, /* [0100] */
   2, /* [0101] */
   2, /* [0110] */
   2, /* [0111] */
   3, /* [1000] */
   3, /* [1001] */
   3, /* [1010] */
   3, /* [1011] */
   3, /* [1100] */
   3, /* [1101] */
   3, /* [1110] */
   3, /* [1111] */
};

/*
   For movement along an axis within a board, add or subtract
   delta_array[axis_number] to a pointer to a cell within the board.
*/
const int delta_array[4] = {11, 9, 1, 10};

static unsigned char schedule_map[10][10];


/*------------------------------*/
/*           Functions          */
/*------------------------------*/


/*
Given a list of affected pieces, starting with the new piece,
update the board, including the protection flags.
*/
void
update_protection_for_board(
   struct board_struct *board_ptr,
   int *affected_list, /* list of affected pieces, beginning with new piece */
   int num_affected,   /* number of pieces in affected list */
   unsigned char new_color)   /* new color for affected pieces */
{
   handle_changed_pieces(board_ptr, affected_list, num_affected, new_color, 1);
   new_piece_axis_fill_check(board_ptr, affected_list);
   perform_all_protection_checks(board_ptr);
}

/*
Update board according to list of pieces changed by this move (including
new piece).
*/
void
handle_changed_pieces(
   struct board_struct *board_ptr,
   register int *affected_list, /* list of affected pieces, beginning with new piece */
   int num_affected,            /* number of pieces in affected list */
   unsigned char new_color,     /* new color for affected pieces */
   int prot_check_flag)         /* flag: request protection checks iff set */
{
   register unsigned char *cell_ptr;

   /* for all pieces changed by the move, including the new piece */
   while (num_affected--) {

      /* Clear all protection bits for this piece & set its new color. */
      *(cell_ptr = &board_ptr->board[0][0] + *affected_list++) = new_color;

      /* Request that this piece be checked for protection along all 4 axes. */
      if (prot_check_flag) {
         request_prot_check(board_ptr, cell_ptr, BD_AX | FD_AX | H_AX | V_AX);
      }
   }
}

/*
Check each axis of new piece to see if we filled the axis.
*/
void
new_piece_axis_fill_check(
   struct board_struct *board_ptr,
   int *affected_list) /* list of affected pieces, beginning with new piece */
{
   unsigned char axis_flag;
   unsigned char *new_piece_ptr;
   register unsigned char *cell_ptr;
   int axis_num;

   new_piece_ptr = &board_ptr->board[0][0] + *affected_list;

   /* for all 4 axes through the new piece */
   for (axis_num = BD_NDX; axis_num <= V_NDX; axis_num++) {
      if (is_filled_axis(new_piece_ptr, axis_num)) {

         /* Set full-axis bit for this axis */
         SET_FA_BIT(board_ptr->fa_bits,
          fa_tabl[*affected_list / 10 - 1][*affected_list % 10 - 1][axis_num]);

         /* Request protection check for each piece along this axis.
            (The pieces are protected along this axis, of course.
            But it's easier to keep the protection checking in one place,
            so we go through the scheduler.)

            The loop which does this (below) never hits the new piece, but
            that's OK because we already requested that it be checked for
            protection (It's in the affected_list).
         */

         {
            register int delta;
            int pass;

            axis_flag = (unsigned char)BD_AX << axis_num;

            delta = delta_array[axis_num];
            for (pass = 0; pass < 2; pass++, delta = -delta) {
               cell_ptr = new_piece_ptr;
               while (!(*(cell_ptr += delta) & OFF_BOARD)) {
                  request_prot_check(board_ptr, cell_ptr, axis_flag);
               }
            }
         }
      }
   }
}


/*
   Perform protection checks until they're all done.

   initialize row and column
   while scheduler returns pieces to check {
      check piece for protection along specified axis
      if piece is protected {
         set protection flag
         if piece is permanent {
            schedule adjacent pieces to be checked for permanence along
            the axis containing this piece
         }
      }
   }
*/
void
perform_all_protection_checks(struct board_struct *board_ptr)
{
   int row;
   int clm;
   int rule_trick;
   register unsigned char *cell_ptr;
   int axis_num;

   row = 1;
   clm = 1;

   while (next_check(&row, &clm, &axis_num)) {

      cell_ptr = &board_ptr->board[row][clm];

      if (CHK_FA_BIT(board_ptr->fa_bits, fa_tabl[row-1][clm-1][axis_num])) {
         /* axis is full, therefore piece is protected along it */
         rule_trick = TRICK_PROTECTED;
      }
      else {
         unsigned char adjac1;
         unsigned char adjac2;
         unsigned char friendly_colors;

         friendly_colors = *cell_ptr & (US_PIECE | THEM_PIECE) | OFF_BOARD;
         adjac1 = *(cell_ptr + delta_array[axis_num]);
         adjac2 = *(cell_ptr - delta_array[axis_num]);

         rule_trick =
            IS_PERM(adjac1) ?
            (BELONGS(adjac1, friendly_colors) ? TRICK_PROTECTED : TRICK_ADJ1_PERM)
            :
            0 ;

         if (IS_PERM(adjac2)) {
            rule_trick |= BELONGS(adjac2, friendly_colors) ?
               TRICK_PROTECTED : TRICK_ADJ2_PERM;
         }
      }

      if (rule_trick == TRICK_PROTECTED) {
         /* the piece is protected along this axis */

         /* set the protection flag & check for permanence */

         if (IS_PERM(*cell_ptr |= BD_AX << axis_num)) {
            int sched_ax_num;
      
            /* Piece is now permanent. */

            /* Schedule protection checks for all adjacent pieces
               along the axis containing this piece.
            */
            for (sched_ax_num = BD_NDX;
               sched_ax_num <= V_NDX;
               sched_ax_num++)
            {
               request_prot_check(
                  board_ptr,
                  cell_ptr + delta_array[sched_ax_num],
                  BD_AX << sched_ax_num
               );
               request_prot_check(
                  board_ptr,
                  cell_ptr - delta_array[sched_ax_num],
                  BD_AX << sched_ax_num
               );
                  cell_ptr - delta_array[sched_ax_num];
            }
         }
      }
   }
}


/*
Set bit(s) in the protection check scheduling map.

If the cell is unoccupied, the entire request will be ignored.
Schedule bits corresponding to axes which are already protected
   will not be set.
*/
void
request_prot_check(
   struct board_struct *board_ptr, /* pointer to board structure */
   unsigned char *cell_ptr,        /* pointer to cell */
   unsigned char requested_axes)   /* requesting prot check for these axes */
{
   if (BELONGS(*cell_ptr, US_PIECE | THEM_PIECE)) {
      /* Cell is occupied.
         Set bits for requested axes, except those axes
         which are already protected.
      */

      *(&schedule_map[0][0] + (cell_ptr - &board_ptr->board[0][0]))
         |= requested_axes & ~*cell_ptr;
   }
   /* else cell is unoccupied, so ignore request */
}

/*
Get next cell for protection check

Returns:
   0: no more cells need to be checked
   1: cell at *row_num_ptr, *clm_num_ptr needs to be checked along axis
      number *axis_num_ptr
*/
int
next_check(int *row_num_ptr, int *clm_num_ptr, int *axis_num_ptr)
{
   register unsigned char *p;
   register unsigned char *stop;
   unsigned char *start;
   int temp;
   int pass;

   p = start = &schedule_map[*row_num_ptr][*clm_num_ptr];
   stop = &schedule_map[8][9];
   for (pass = 0; pass < 2; pass++) {
      while (*p == 0 && ++p < stop) ;
      if (p != stop) {
         /* found one, perform calcs and return */
         temp = p - &schedule_map[0][0];
         *row_num_ptr = temp / 10;
         *clm_num_ptr = temp % 10;
         /* Determine which axis & clear schedule bit for that axis */
         *p ^= BD_AX << (*axis_num_ptr = bit_priority_encode[*p >> 4]);
         return (1); /* found one */
      }
      p = &schedule_map[1][1]; /* now check from 1st cell of the board... */
      stop = start;            /* ...up to where we started checking */
   }
   return (0); /* they're all done */
}


/*
Determine whether a specified axis has been filled by a new piece.

The cell occupied by the new piece is never checked; it is just assumed
to be occupied.

Returns:
   1 if axis is filled, 0 if axis is not filled
*/
unsigned char
is_filled_axis(unsigned char *new_piece_ptr, int axis_num)
{
   register unsigned char *ptr;
   register int delta;
   int pass;

   delta = delta_array[axis_num];
   for (pass = 0; pass < 2; pass++, delta = -delta) {
      ptr = new_piece_ptr;
      while (*(ptr += delta) & (US_PIECE | THEM_PIECE)) ;
      if (*ptr & NO_PIECE) {
         return (0);    /* This axis is still unfilled. */
      }  /* else we ran off the edge of the board */
   }

   return (1);
}
