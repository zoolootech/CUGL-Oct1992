

#include "OTHELLO.H"

#define U US_PIECE
#define T THEM_PIECE

board_type board =
  {
    {
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, U, T, U, T, U, T, U, T, 0 },
      { 0, T, U, T, U, T, U, T, U, 0 },
      { 0, U, T, U, T, U, T, U, T, 0 },
      { 0, T, U, T, U, T, U, T, U, 0 },
      { 0, U, T, U, T, U, T, U, T, 0 },
      { 0, T, U, T, U, T, U, T, U, 0 },
      { 0, U, T, U, T, U, T, U, T, 0 },
      { 0, T, U, T, U, T, U, T, U, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },

    { 0, }
  };


void main (void)
{
disp_board (&board);
}


