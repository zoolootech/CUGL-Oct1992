#include <stdio.h>
#include <string.h>

#include "othello.h"



void main (void)
{
fa_type fa_ndx_tbl [8][8];
register int r;
register int c;
unsigned char next_fa_bit;


/*-------------------------------------
Initialize some variables. Clear the
table to all zeroes.
-------------------------------------*/

next_fa_bit = 1;
memset (fa_ndx_tbl, 0, sizeof (fa_ndx_tbl));


/*-------------------------------------
Initialize the horizontal index for all
cells in the fa index table.
-------------------------------------*/

for (r = 0; r < 8; r++, next_fa_bit++)
  for (c = 0; c < 8; c++)
    fa_ndx_tbl [r][c][H_NDX] = next_fa_bit;


/*-------------------------------------
Initialize the vertical index for all
cells in the fa index table.
-------------------------------------*/

for (c = 0; c < 8; c++, next_fa_bit++)
  for (r = 0; r < 8; r++)
    fa_ndx_tbl [r][c][V_NDX] = next_fa_bit;


/*-------------------------------------
Initialize the positive slope diagonals
(forward diagonal) starting from the
left and bottom edges and moving up.
This is more tricky, since we have to
move diagonally.
-------------------------------------*/

for (r = 2; r < 8; r++, next_fa_bit++)
  {
  int r2;	/* temp row */

  r2 = r;
  for (c = 0; (c < 8) && (r2 >= 0); c++, r2--)
    fa_ndx_tbl [r2][c][FD_NDX] = next_fa_bit;
  }

for (c = 0; c < 6; c++, next_fa_bit++)
  {
  int c2;	/* temp column */

  c2 = c;
  for (r = 7; (r >= 0) && (c2 < 8); r--, c2++)
    fa_ndx_tbl [r][c2][FD_NDX] = next_fa_bit;
  }


/*-------------------------------------
Initialize the negative slope diagonals
(backward diagonal) starting from the
top and left edges and moving down.
This uses the same method to move
diagonally as in the positive diags.
-------------------------------------*/

for (c = 0; c < 6; c++, next_fa_bit++)
  {
  int c2;	/* temp column */

  c2 = c;
  for (r = 0; (r < 8) && (c2 < 8); r++, c2++)
    fa_ndx_tbl [r][c2][BD_NDX] = next_fa_bit;
  }

for (r = 1; r < 6; r++, next_fa_bit++)
  {
  int r2;	/* temp row */

  r2 = r;
  for (c = 0; (c < 8) && (r2 < 8); c++, r2++)
    fa_ndx_tbl [r2][c][BD_NDX] = next_fa_bit;
  }


/*-------------------------------------
Now, print it out!
-------------------------------------*/

for (r=0; r<8; r++)
  {
  for (c=0; c<8; c++)
    {
    printf ("    { %2d, %2d, %2d, %2d },\t\t/* %d,%d */\n",
	fa_ndx_tbl [r][c][BD_NDX],
	fa_ndx_tbl [r][c][FD_NDX],
	fa_ndx_tbl [r][c][H_NDX],
	fa_ndx_tbl [r][c][V_NDX],
	r, c);
    }
  printf ("\n");
  }

}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

