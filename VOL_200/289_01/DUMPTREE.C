/*
Dump entire move tree (for debugging)
*/

#include <stdio.h>
#include "othello.h"

static key_type dumptree_sub(key_type key, int indent);

void
dumptree(key_type key)
{
   dumptree_sub(key, 0);
   fprintf(stdprn, "\r\n\r\n");
   fflush(stdprn);
}

static key_type
dumptree_sub(key_type key, int indent)
{
   struct limb_struct far *limb_ptr;
   move_type move;
   struct board_struct dummy_board;

   limb_ptr = db_retrieve_limb(key);

   /*
      Output information about the limb
      key    move  score sibling child
      00000 . . A1 -32768 00000 00000
      65535 b x H8  32767 65535 65535
   */
   if (limb_ptr == NULL) {
      fprintf(stdprn, "%*s %5d ***** ERROR: limb *****\r\n",
         indent * 3, "",
         key);
      return (NO_KEY);
   }
   else {
      move = limb_ptr->move;

      fprintf(stdprn, "%*s %5d %c %c %c%c %6d %5d %5d\r\n",
         indent * 3, "",
         key,
         move & BOARD_MASK ? 'b' : '.',
         move & NO_MOVE_MASK ? 'x' : '.',
         GET_ROW(move) + 'A',
         GET_COL(move) + '1',
         limb_ptr->score,
         limb_ptr->sibling_key,
         limb_ptr->bc.child_key);

      if (move & BOARD_MASK) {
         if (db_retrieve_board(limb_ptr->bc.board_key, &dummy_board)) {
            fprintf(stdprn, "%*s ^^^^^ ***** ERROR: board *****\r\n",
               indent * 3, "");
         }
      }
      else {
         for (key = limb_ptr->bc.child_key; key != NO_KEY;
            key = dumptree_sub(key, indent + 1))  ;
      }
      return (limb_ptr->sibling_key);
   }
}
