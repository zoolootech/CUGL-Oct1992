/*-----------------------------------------------------------------------------
Build the tree of moves down to a given level.

Revision History
----------------
NAME            DATE            MODS
Gary Culp       19 Dec 1988 -
                 2 Jan 1988     Initial version
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include "othello.h"

STATIC int after_opponents_move(move_type humans_move);

STATIC void build_children_of_a_board(
   key_type parent_limb_key,
   key_type parent_board_key,
   unsigned char movers_color);

STATIC int build_a_level(void);

/* return values from build_a_level() */
#define BT_COMPLETED  0
#define BT_DBFULL     1

/*
   This level:

   board/limb  has children?  is bottom?
   -------------------------------------
  -limb----------n--------------n------------ERROR
  -limb----------n------------y--------------ERROR
   limb        y                n         descend along each child
   limb        y              y           return
   board         n              n         build children
   board         n            y           return
  -board-------y----------------n------------ERROR
   board       y              y           return
*/


/*
Keep track of the depth to which the tree has been fully built.
*/
STATIC int tree_depth = 0;

/*
The build_tree function is iterative rather than recursive because building the
tree is asynchronous to the human's move; and we want to take advantage of
information about the human's move immediately (by pruning moves he didn't
make from the tree).

Information for manually-recursive tree build:
   'stack' of limb keys
   desired depth (actually use value of stack index at desired depth; which
      turns out to be the desired depth - 2)
   stack pointer for stack of limb keys (actually an index rather than
      a pointer)
   mover's color at current depth
*/
STATIC key_type limb_key_stack[BT_MAX_DEPTH];
STATIC int bt_desired_nest;
STATIC int bt_current_nest; /* "stack pointer" (index) for limb_key_stack */
STATIC unsigned char bt_movers_color;

STATIC int human_state;
/* defines for human_state */
#define HS_NOT_HUMANS_TURN    0
#define HS_WAITING_FOR_HUMAN  1
#define HS_HUMAN_MOVED        2

STATIC humans_move_limb_key;


/*
Build the tree to the specified level

The function builds the tree until it reaches the specified depth, or
the database fills up.

If movers_color == THEM_PIECE:
   The function will also check for input from the human player while it is
   building the tree.  If it has to stop building the tree, it will continue
   to check for the human's input.  It will not return until the human player
   has input his move.
If movers_color == US_PIECE:
   The function will not check for input from the human player.

This function assumes that the root of the tree corresponds to the current
state of the board.

Returns:
   depth to which tree was completely built
*/
int
build_tree(int depth_to_build, unsigned char movers_color)
{
   struct limb_struct far *limb_ptr;
   int temp;

   if (movers_color == THEM_PIECE) {
      human_state = HS_WAITING_FOR_HUMAN;
      depth_to_build++; /* We'll delete 1 level after the human moves; so we
                           build an extra level now. */
   }
   else {
      human_state = HS_NOT_HUMANS_TURN;
   }

   limb_ptr = db_retrieve_limb(curnt_top_limb);
   if (limb_ptr->move & BOARD_MASK) {
      /* The root is a board.  There is a lower-level function
         (after_opponents_move()) which assumes that the root is not a board,
         so we have to build the first level of the tree before continuing.
      */
      build_children_of_a_board(curnt_top_limb,
                                limb_ptr->bc.board_key,
                                movers_color);
      tree_depth = 1;
   }

   for (bt_desired_nest = tree_depth - 1;
        bt_desired_nest <= depth_to_build - 2;
        bt_desired_nest++)
   {
      limb_key_stack[0] = limb_ptr->bc.child_key;
      bt_current_nest = 0;
      bt_movers_color = movers_color ^ (US_PIECE | THEM_PIECE);

      /* Try to build a level. If the database fills up but we're waiting for
         input from the human, keep trying to build until we get his input.
      */
      do {
         temp = build_a_level();
      } while (temp != BT_COMPLETED && human_state == HS_WAITING_FOR_HUMAN);

      if (temp == BT_COMPLETED) {
         tree_depth = bt_desired_nest + 2;
      }
      else {
         break;
      }
   }

   /* We may finish building the tree to the depth we desire before the
      human opponent enters his move.  If this happens, this loop will wait
      for him to enter his move.
   */
   while (human_state == HS_WAITING_FOR_HUMAN) {
      move_type humans_move;

      if ((humans_move = check_for_input()) != HASNT_MOVED) {
         human_state = HS_HUMAN_MOVED;
         after_opponents_move(humans_move);
      }
   }

   if (movers_color == THEM_PIECE) {

      free_limb(curnt_top_limb);   /* delete root limb */

      /* Make limb for human's move the new root limb */
      curnt_top_limb = humans_move_limb_key;

      tree_depth--;  /* Since we deleted the old root, the tree is shallower */
   }
   return (tree_depth);
}


/*
Build another level onto the tree
This function communicates through static variables.

Returns:
   BT_COMPLETED: finished building the level
   BT_DBFULL:    had to stop building because database was almost full
*/
STATIC int
build_a_level()
{
   limb_type far *limb_ptr;
   move_type humans_move;

   while (1) {       /* exit from loop by 'return's */
      if (human_state == HS_WAITING_FOR_HUMAN  &&
          (humans_move = check_for_input()) != HASNT_MOVED)
      {
         human_state = HS_HUMAN_MOVED;
         if (after_opponents_move(humans_move)) {
            return (BT_COMPLETED);
         }
      }

      /*
      If the database is almost full, stop building.
      Resume build at this point later on, after the tree has been trimmed.
      */
      if (db_almost_full()) {
         return (BT_DBFULL);
      }

      /* Limbs have either a board or children.
         If this limb has a board, build its children now.
      */

      limb_ptr = db_retrieve_limb(limb_key_stack[bt_current_nest]);

      if (limb_ptr->move & BOARD_MASK) {  /* it's a board */ 
         build_children_of_a_board(limb_key_stack[bt_current_nest],
                                 limb_ptr->bc.board_key,
                                 bt_movers_color);
      }

      /*
         Figure out next limb key to examine (leave on top of limb_key_stack)
      */
      if (bt_current_nest != bt_desired_nest) {
         /* Follow child key to nest down one level */
         limb_key_stack[bt_current_nest + 1] =
            db_retrieve_limb(limb_key_stack[bt_current_nest])->bc.child_key;
         ++bt_current_nest;
         bt_movers_color ^= (US_PIECE | THEM_PIECE);
      }
      else {
         while (bt_current_nest >= 0) {   /* also contains a 'break' */
            /* 
               Replace limb key on top of stack with its next sibling;
               if it has no next sibling, pop it off the stack.
            */
            if (
                (limb_key_stack[bt_current_nest] =
                 db_retrieve_limb(limb_key_stack[bt_current_nest])->sibling_key
                )
                == NO_KEY
               )
            {
               --bt_current_nest;  /* pop stack */
               bt_movers_color ^= (US_PIECE | THEM_PIECE);
            }
            else {
               break; /* exit from loop */
            }
         }

         if (bt_current_nest < 0) {
            /* a complete level of the tree has been built */
            return (BT_COMPLETED);
         }
      }
   }
}


/*
Build all the child boards for a given parent_limb.
The parent_limb must have a board (not child limbs) associated with it.
*/
STATIC void
build_children_of_a_board(
   key_type parent_limb_key,
   key_type parent_board_key,
   unsigned char movers_color)
{
   struct board_struct parent_board;
   struct board_struct child_board;
   int aff_list[MAX_AFFECTED_PIECES];  /* list of pieces affected by move */
   int aff_ct;        /* number of pieces affected by move (incl. new piece) */
   int look_at;       /* begin search for move at this offset within .board */
   move_type db_move; /* move, encoded the way database functions want it */

   if (db_retrieve_board(parent_board_key, &parent_board)) {
      /* This should NEVER happen. If it does, there's a bug in the program. */
      printf("\nFATAL PROGRAM BUG:\n\
couldn't retrieve parent board in build_children_of_a_board\n");
      exit(5);
   }

   /* init vars */
   look_at = 0;   /* start at beggining of board */

   while (aff_ct = find_move(&parent_board, look_at, movers_color, aff_list)) {
      /* Let child inherit info from parent, then make changes. */

      child_board = parent_board;

      update_protection_for_board(&child_board, aff_list, aff_ct,
         movers_color);

      db_move = 0;
      SET_ROW_COL(db_move, aff_list[0] / 10 - 1, aff_list[0] % 10 - 1);

      if (db_add_child(parent_limb_key, db_move, &child_board,
                       bd_eval(&child_board, US_PIECE)     ) == -1)
      {
         /* This shouldn't be possible -- we check for room in the database 
            earlier
         */
         printf("OTHELLO FATAL: no room to add board to database\n");
         exit(5);
      }

      look_at = aff_list[0] + 1; /* resume search for moves at next cell */
   }

   /* If there are NO moves at all for this player, we still have to add
      a child board (with a special move code) to the database.
   */
   if (look_at == 0) {

      /* look_at == 0 iff there are NO moves were found */

      /* With the board scoring scheme in effect 3Jan1989, the score for
         this board will the same as the parent board's score.
         In most cases, there will be moves available.  Therefore, this
         code will rarely be executed.  Therefore, the performance gain
         from short-cutting the evaluation of these boards is slight.
         For this reason, I chose to fully evaluate the board.
      */

      if (db_add_child(parent_limb_key, NO_MOVE_MASK, &parent_board,
                       bd_eval(&parent_board, US_PIECE)     ) == -1)
      {
         /* This shouldn't be possible -- we check for room in the database 
            earlier
         */
         printf("OTHELLO FATAL: no room to add board to database\n");
         exit(5);
      }
   }
}


/*
This function should be called after the opponent has entered his move.
It clears out unneeded parts of the database and, if necessary, adjusts
the variables for the tree build.

This function assumes that there are limbs for the children of the root.
(In other words, the tree has been built to at least one level below the root.)

Returns:
   A flag indicating whether the current build phase for the subtree 
   corresponding to the human's move has been completed.
      0: not completed
      1: completed
*/
STATIC int
after_opponents_move(move_type humans_move)
{
   key_type key;
   struct limb_struct far *limb_ptr;
   int already_built;
   int ret_val;

   already_built = 1;

   /* for each limb below root of tree */
   for (key = db_retrieve_limb(curnt_top_limb)->bc.child_key;
        key != NO_KEY;
        key = limb_ptr->sibling_key)
   {

      /* Retrieve the limb */
      limb_ptr = db_retrieve_limb(key);

      /* if this is the limb at the top of the subtree currently being built */
      if (key == limb_key_stack[0]) {
         /* This and subsequent parts of the tree haven't been fully built to
            the desired depth.
         */
         already_built = 0;
      }

      /* if this limb corresponds to the human's move */
      if ((limb_ptr->move & ~BOARD_MASK) == humans_move) {
         /* Remember whether this subtree has been completed. */
         ret_val = already_built;

         /* Remember the limb key for the human's move. */
         humans_move_limb_key = key;

         /* If this is not the limb at the top of the subtree currently
            being built, reset the limb 'stack' to start over at this limb.
         */
         if (key != limb_key_stack[0]) {
            bt_current_nest = 0;
            limb_key_stack[0] = key;
            bt_movers_color = US_PIECE;
         }
      }
      else {
         /* delete this limb & its subtree */
         db_delete_subtree(curnt_top_limb, key);
      }
   }

   return (ret_val);
}


/*
This function is to be called after we (the computer) move.
It deletes all our possible moves except the one we made, deletes the old
root of the tree, and sets the new root to be the move we just made.

The tree_depth variable is also updated to reflect these changes.
*/
void
update_tree_after_our_move(
   key_type our_move)
{
   register key_type ch;
   limb_type far *l;

   ch = db_retrieve_limb(curnt_top_limb)->bc.child_key;
   for ( ; ch != NO_KEY; ch = l->sibling_key) {
      l = db_retrieve_limb(ch);

      if (ch != our_move) {
         db_delete_subtree(curnt_top_limb, ch);
      }
   }

   free_limb(curnt_top_limb);

   curnt_top_limb = our_move;

   tree_depth--;
}
