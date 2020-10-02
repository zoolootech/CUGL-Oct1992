/*-----------------------------------------------------------------------------
OTHELLO.C

This file contains the C main function.

Revision History
----------------
Jon Ward	10 Dec 1988	Initial Revision
Jon Ward	 2 Jan 1989	Added the game playing loop
Jon Ward	 7 Jan 1989	Added lookahead to the end of game for the last
				  BT_MAX_DEPTH moves.
Jon Ward	 8 Jan 1989	Changed lookahead level to 3 until the last 30
				  moves of the game where it becomes 5.
Jon Ward	 9 Jan 1989	Added input of who plays first and what color.
Jon Ward	29 Jan 1989	Toupperd Y/N and X/O and added missing ')'.
-----------------------------------------------------------------------------*/

#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include "othello.h"


/*-----------------------------------------------------------------------------
                        Global Variable Declarations
-----------------------------------------------------------------------------*/

board_type curnt_bd;		/* current state of the board */
key_type curnt_top_limb;	/* key for the board in the DB */


/*-----------------------------------------------------------------------------
			   Local Function Prototypes
-----------------------------------------------------------------------------*/

STATIC void play_othello (unsigned char player);


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

void main ()
{
unsigned char first_player;
unsigned char x_player;
unsigned char o_player;


/*-----------------------------------------------
Initialize all sections of the program.
-----------------------------------------------*/

db_init ();			/* init database manager */
disp_init ();			/* init display routines */

printf ("OTHELLO Playing Program\n");
printf ("Written by Jonathan Ward and Gary Culp\n\n");


/*-----------------------------------------------
Find out who plays first.
-----------------------------------------------*/

  {
  register int input;

  printf ("Do you want to play first? (Y/N)\n");
  
  do
    {
    input = getch ();
    input = toupper (input);
    }
  while (input != 'Y' && input != 'N');

  printf ("%c\n", input);
  first_player = (unsigned char) ((input == 'Y') ? THEM_PIECE : US_PIECE);

  printf ("Do you want to be X or O? (X/O)\n");

  do
    {
    input = getch ();
    input = toupper (input);
    }
  while (input != 'X' && input != 'O');

  printf ("%c\n", input);

  if (input == 'X')
    {
    x_player = THEM_PIECE;
    o_player = US_PIECE;
    }
  else
    {
    o_player = THEM_PIECE;
    x_player = US_PIECE;
    }

  us_char = (char) ((input == 'X') ? 'O' : 'X');
  them_char = (char) input;  
  }


/*-----------------------------------------------
Now, setup the initial board configuration and
current top limb var.  Put the initial board in
the database.
-----------------------------------------------*/

init_borders (&curnt_bd);

  {
  register int r;
  register int c;

  for (r = 1; r < 9; r++)
    for (c = 1; c < 9; c++)
      curnt_bd.board [r][c] = NO_PIECE;
  }


/*-----------------------------------------------
Initialize the starting board and put it in the
database.
-----------------------------------------------*/

curnt_bd.board [4][4] = curnt_bd.board [5][5] = x_player;
curnt_bd.board [4][5] = curnt_bd.board [5][4] = o_player;

curnt_top_limb = db_add_child (NO_KEY, NO_MOVE_MASK, &curnt_bd, 0);


/*-----------------------------------------------
Display the current board and begin playing the
game.
-----------------------------------------------*/

disp_board (&curnt_bd);
play_othello (first_player);	/* play the game */

db_kill ();			/* free database allocated ram */
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

STATIC void play_othello (unsigned char player)
{
register int tree_depth;
move_type our_move;
key_type our_move_key;
register int plays_left;

player = (unsigned char) ((player & US_PIECE) ? US_PIECE : THEM_PIECE);

do
  {
  init_input_vars ();		/* clear input vars */

/*------------------------------------------------
Determine how deep we look ahead depending on how
many pieces are left on the board.
------------------------------------------------*/

  plays_left = piece_count (&curnt_bd, NO_PIECE);

  if (plays_left <= BT_MAX_DEPTH)
    tree_depth = BT_MAX_DEPTH;
  else if (plays_left <= 30)
    tree_depth = 5;
  else
    tree_depth = 3;


/*------------------------------------------------
Attempt to build the board to tree_depth levels.
------------------------------------------------*/

  tree_depth = build_tree (tree_depth, player);

#if 0
  dumptree(curnt_top_limb);	/***/
#endif

  our_move_key = find_best_move (curnt_top_limb, tree_depth);
  our_move = (db_retrieve_limb (our_move_key))->move & ~BOARD_MASK;

  if (our_move & NO_MOVE_MASK)
    {
    disp_player_cant_move (US_PIECE);
    disp_board (&curnt_bd);
    }
  else
    {
    verify_move_and_update_board (our_move, US_PIECE);
    }

#if 0		/*** change 0 to print lookahead level ***/
  printf ("\nMinimax maximum = %d  ", last_max_score);
  printf ("Lookahead Level = %d\n", tree_depth);
#endif

  update_tree_after_our_move (our_move_key);

  player = THEM_PIECE;
  }
while (who_can_move (&curnt_bd));

disp_pieces (piece_count (&curnt_bd, US_PIECE),
	     piece_count (&curnt_bd, THEM_PIECE));
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/


