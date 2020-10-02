/*-----------------------------------------------------------------------------
MINIMAX.C

This file contains the tree searching minimax algorithms.

Revision History
----------------
Jon Ward	 2 Jan 1989	Initial Revision
Jon Ward	 7 Jan 1989	Added last_max_score var for statistical stuff.
-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <limits.h>

#include "othello.h"


/*-----------------------------------------------------------------------------
			  Global Variable Declarations
-----------------------------------------------------------------------------*/
int last_max_score;

/*-----------------------------------------------------------------------------
			   Local Function Prototypes
-----------------------------------------------------------------------------*/
STATIC int mm_max (
   key_type parent,		/* limb whose children will be maxd */
   int depth,			/* number of levels to minimax */
   key_type *best);		/* pointer to best move */

STATIC int mm_min (
   key_type parent,		/* limb whose children will be mind */
   int depth,			/* number of levels to minimax */
   key_type *worst);		/* pointer to worst move */


/*-----------------------------------------------------------------------------
STATIC int mm_max (
   key_type parent,
   int depth,
   key_type *best);

This function will find the maximal board score depth levels below the parent.
The value returned is the maximal score.
-----------------------------------------------------------------------------*/

STATIC int mm_max (
   key_type parent,		/* limb whose children will be maxd */
   int depth,			/* number of levels to minimax */
   key_type *best)		/* pointer to best move */
{
register int max;		/* value for the maximal limb */
int score;			/* score for the current limb */
limb_type far *l;		/* pointer to the limb for the current limb */
register key_type limb;		/* key for the current limb */

max = INT_MIN;			/* minimum int value */

limb = (db_retrieve_limb (parent))->bc.child_key;

for (; limb != NO_KEY; limb = l->sibling_key)
  {
  l = db_retrieve_limb (limb);

  score = (depth) ? mm_min (limb, depth - 1, NULL) : l->score;

  if (score > max)
    {
    max = score;
    if (best)
      *best = limb;
    }
  }

return (max);
}


/*-----------------------------------------------------------------------------
STATIC int mm_min (
   key_type parent,
   int depth,
   key_type *worst);

This function will find the minimal board score depth levels below the parent.
The value returned is the minimal score.
-----------------------------------------------------------------------------*/

STATIC int mm_min (
   key_type parent,		/* limb whose children will be mind */
   int depth,			/* number of levels to minimax */
   key_type *worst)		/* pointer to worst move */
{
register int min;		/* value for the minimal limb */
int score;			/* score for the current limb */
limb_type far *l;		/* pointer to the limb for the current limb */
register key_type limb;		/* key for the current limb */

min = INT_MAX;			/* maximum int value */

limb = (db_retrieve_limb (parent))->bc.child_key;

for (; limb != NO_KEY; limb = l->sibling_key)
  {
  l = db_retrieve_limb (limb);

  score = (depth) ? mm_max (limb, depth - 1, NULL) : l->score;

  if (score < min)
    {
    min = score;
    if (worst)
      *worst = limb;
    }
  }

return (min);
}


/*-----------------------------------------------------------------------------
key_type find_best_move (
   key_type root,
   int depth);

This function returns the key of the maximal move for the root referenced by
the root argument. Note that the root key is the key for the currently
displayed board and that its children are the moves that will finally be
considered. The depth argument is the number of levels of children that exist
below the root.  For example, depth = 1 means that there is only one level
built below the root.
-----------------------------------------------------------------------------*/

key_type find_best_move (
   key_type root,		/* key to root of tree */
   int depth)			/* levels below root to look */
{
key_type best_move;

last_max_score = mm_max (root, depth - 1, &best_move);	/* do the minimax */
return (best_move);			/* return the limb key of the best move */
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

