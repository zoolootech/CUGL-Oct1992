/*-----------------------------------------------------------------------------
OTHDBM.C

Revision History
----------------
Jon Ward	17 Oct 1988	Initial Revision.
Jon Ward	30 Oct 1988	Mods for move type.
Jon Ward	30 Oct 1988	Optimized and combined the allocate functions.
				  Now we use the macros ALLOCATE_LIMB and
				  ALLOCATE_BOARD.
Jon Ward	04 Dec 1988	Debugging commencing.
Jon Ward	03 Jan 1989	Added relinking ability to db_delete_subtree
				  changed free_limb from static to global.
-----------------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "othello.h"


/*-----------------------------------------------------------------------------
How This Thing Works:
There will be two data spaces. One for limbs, and one for boards. Each data
space will be maintained as an array. Each array will be bit-mapped so that
we can determine if an element in the array is unused.
-----------------------------------------------------------------------------*/

/*------------------------------------------------
The following structure bd_row_st is used as a
trick to get the MSC compiler to perform a struct
assignment for the board row. The reason for this
is that the database boards are stored in a far
area of ram. Segmentation...Argh!!!!

The dbm_board_st struct is a compressed form of
the board_st structure used every else in the
program. The boards, when compressed, are much
smaller, about 52% smaller or 66% as big as they
would have been.
------------------------------------------------*/

struct bd_row_st
  {
  unsigned char row [8];
  };

struct dbm_board_st
  {
  struct bd_row_st board [8];
  unsigned char fa_bits [FA_BITS_SIZE];
  };

typedef struct dbm_board_st dbm_board_type;


#define MAX_DBM_LIMBS  65536 / sizeof (limb_type)
#define MAX_DBM_BOARDS 32768

STATIC limb_type far limb_array [MAX_DBM_LIMBS];

STATIC key_type num_limbs_left;		/* number of limbs available */
STATIC key_type num_boards_left;	/* number of boards available */

#define MAX_BD_PTRS		50
#define STORED_BOARD_SIZE	sizeof (dbm_board_type)

STATIC key_type bd_per_blk = 16384 / STORED_BOARD_SIZE;
STATIC dbm_board_type far *board_ptrs [MAX_BD_PTRS];
STATIC int bd_blks;

typedef unsigned int am_type;		/* availability map type */
#define AM_SIZE (8 * sizeof (am_type))	/* bits in availability map type */

STATIC am_type lam [(MAX_DBM_LIMBS + 7) / 8];	/* limb availability map */
STATIC am_type bam [(MAX_DBM_BOARDS + 7) / 8];	/* board availability map */

#define AM_USED ((am_type) (-1))

#define LAM_SIZE (sizeof (lam) / sizeof (lam [0]))
#define BAM_SIZE (sizeof (bam) / sizeof (bam [0]))


/*-----------------------------------------------
The following macros are used to GET, SET, and
CLR bits in the limb and board availability maps.
-----------------------------------------------*/

#define GET_BIT(x,y) (((x) [(y) / AM_SIZE]) &   (1 << ((y) % AM_SIZE)))
#define SET_BIT(x,y) (((x) [(y) / AM_SIZE]) |=  (1 << ((y) % AM_SIZE)))
#define CLR_BIT(x,y) (((x) [(y) / AM_SIZE]) &= ~(1 << ((y) % AM_SIZE)))


key_type max_limb_key = MAX_DBM_LIMBS;		/* maximum key number for limbs */
key_type max_board_key = MAX_DBM_BOARDS;	/* maximum key number for boards */

key_type last_parent_key;	/* last parent child was added to */
key_type last_child_key;	/* last child added */


/*-----------------------------------------------------------------------------
                         Local Function Prototypes
-----------------------------------------------------------------------------*/
STATIC key_type allocate_limb (int am_sel);
STATIC void free_board (key_type board);
STATIC void free_subtree (key_type key);
STATIC void get_board (
  key_type key,
  board_type *board);
STATIC void put_board (
  key_type key,
  board_type *board);


/*-----------------------------------------------------------------------------
The following data is used by the allocate_am function when searching for and
allocating a limb or board element in the database.
-----------------------------------------------------------------------------*/

struct am_struct
  {
  am_type *am;		/* pointer to availability map */
  key_type size;	/* number of elements in availability map */
  key_type *max;	/* maximum key for am */
  int *num_avail;	/* number of available elements */
  };


STATIC struct am_struct am_data [] =
  {
    { lam, LAM_SIZE, &max_limb_key,  &num_limbs_left },
    { bam, BAM_SIZE, &max_board_key, &num_boards_left },
  };


#define ALLOCATE_LIMB()  (allocate_am (0))
#define ALLOCATE_BOARD() (allocate_am (1))


/*-----------------------------------------------------------------------------
STATIC key_type allocate_am (int am_sel);

This function will locate the next available element in either the limb or
board array. The am_sel argument determines whether the limb (0) or board (1)
map will be used. If a free element if found, it is marked as used, and the
key for referencing that element is returned. If no free space is available,
the NO_KEY manifest constant is returned to indicate an allocation failure.
-----------------------------------------------------------------------------*/

STATIC key_type allocate_am (int am_sel)	/* availability map selector */
{
struct am_struct *amp;	/* pointer into availability map structure */
register am_type *pt;	/* pointer into the limb availability map */
register key_type key;	/* key var */


amp = &am_data [am_sel];	/* init pointer to am struct */


for (pt = amp->am; pt < (amp->am + amp->size); pt++)
  {
  if (*pt != AM_USED)				/* if a limb is free */
    {
    am_type am_entry;				/* var copy of am entry */

    key = AM_SIZE * (pt - amp->am);
    am_entry = *pt;

    for (; am_entry & 1; am_entry >>= 1, key++);	/* generate the key */

    if (key >= *(amp->max))	/* range check the key */
      break;			/* break if it's too big */

    SET_BIT (amp->am, key);	/* set alloc bit if range ok */
    *(amp->num_avail) -= 1;	/* decrement number available */

    return (key);		/* return the key */
    }
  }

return (NO_KEY);				/* out of limb space */
}


/*-----------------------------------------------------------------------------
void free_limb (key_type limb);

This function will free the array element associated with a previously
allocated limb. The limb argument id the index for the array element to free.
The limb is freed merely by clearing the corresponding bit in the limb
availability map.
-----------------------------------------------------------------------------*/

void free_limb (key_type limb)
{
if (limb >= 0 && limb < max_limb_key && GET_BIT (lam, limb))
  {
  num_limbs_left++;
  CLR_BIT (lam, limb);
  }
}


/*-----------------------------------------------------------------------------
STATIC void free_board (key_type board)

This function will free the array element associated with a previously
allocated board. The board argument id the index for the array element to free.
The board is freed merely by clearing the corresponding bit in the board
availability map.
-----------------------------------------------------------------------------*/

STATIC void free_board (key_type board)
{
if (board >= 0 && board < max_board_key && GET_BIT (bam, board))
  {
  num_boards_left++;
  CLR_BIT (bam, board);
  }
}



/*-----------------------------------------------------------------------------
void db_init (void);

This function will clear the othello database manager variables and prepare
some data space for the othello program to use. It should only be called once
to initialize the database.

Return Value
None.
-----------------------------------------------------------------------------*/

/*-----------------------------------------------
The following pragma declares the memset function
to generate inline code
-----------------------------------------------*/
#pragma intrinsic (memset)

void db_init ()
{
key_type blk_size;			/* block size for allocation */
key_type max_boards;

memset (lam, 0, LAM_SIZE);	/* clear limb availability map */
memset (bam, 0, BAM_SIZE);	/* clear board availability map */

last_parent_key = NO_KEY;
last_child_key = NO_KEY;

blk_size = bd_per_blk * STORED_BOARD_SIZE;

for (bd_blks = 0; bd_blks < MAX_BD_PTRS; bd_blks++)
  {
  if ((board_ptrs [bd_blks] = _fmalloc (blk_size)) == NULL)
    break;
  }

max_boards = bd_blks * bd_per_blk;
if (max_boards < max_board_key)
  max_board_key = max_boards;

num_limbs_left = max_limb_key;		/* set maximum limb var */
num_boards_left = max_board_key;	/* set maximum board var */


#if 0
printf ("Maximum boards = %d\n\n", max_board_key);	/***/
#endif
}


/*-----------------------------------------------------------------------------
void db_kill (void);

This function frees the memory allocated for the database.
-----------------------------------------------------------------------------*/

void db_kill (void)
{
register int i;

for (i = 0; i < bd_blks; i++)
  _ffree (board_ptrs [i]);
}


/*-----------------------------------------------------------------------------
key_type db_add_child (key_type parent_key,
			unsigned char move,
			board_type *board,
			int score)

This function will add a new child board to the look-ahead move tree. The move
argument contains the move required to get to the board. The score argument
contains the numerical value of the board. The parent_key argument indexes the
parent of the new child.

Return Value
The key for the new child leaf, or -1 if there is no more space in the database
for additions.
-----------------------------------------------------------------------------*/

key_type db_add_child (key_type parent_key,
			move_type move,
			board_type *board,
			int score)
{
key_type child_key;
limb_type far *limb;


if ((child_key = ALLOCATE_LIMB ()) == NO_KEY)
  {
  /*** Handle no more limb space condition ***/
  }


/*-----------------------------------------------
Check to see if the parent's child_key is really
a board key.  If it is, free the board and point
the parent to its first born child.
-----------------------------------------------*/

if (parent_key != NO_KEY)
  {
  limb = &limb_array [parent_key];

  if (limb->move & BOARD_MASK)
    {
    limb->move &= ~BOARD_MASK;
    free_board (limb->bc.child_key);
    limb->bc.child_key = child_key;
    }
  }


/*-----------------------------------------------
Save the board, move, score, and sibling for this
limb.
-----------------------------------------------*/

limb = &limb_array [child_key];

if ((limb->bc.child_key = ALLOCATE_BOARD ()) == NO_KEY)
  {
  free_limb (child_key);
  /*** handle condition of no board space available ***/
  }


limb->move = move | BOARD_MASK;
limb->score = score;
limb->sibling_key = NO_KEY;

put_board (limb->bc.child_key, board);


/*-----------------------------------------------
If we are adding children other than the first
to a parent, add these children to the end of the
sibling list.

Update the new last parent and last child vars.
-----------------------------------------------*/

if (parent_key != NO_KEY)
  {
  if (last_parent_key == parent_key)
    {
    limb_array [last_child_key].sibling_key = child_key;
    }

  else if (limb_array [parent_key].bc.child_key != child_key)
    {
    key_type temp;

    temp = limb_array [parent_key].bc.child_key;

    while (limb_array [temp].sibling_key != NO_KEY)	/* find last child */
      temp = limb_array [temp].sibling_key;

    limb_array [temp].sibling_key = child_key;
    }

  last_parent_key = parent_key;			/* save current parent */
  last_child_key = child_key;			/* save current child */
  }

return (child_key);
}


/*-----------------------------------------------------------------------------
int db_retrieve_board (key_type key,
			board_type *board)

This function will retrieve a board from the board database. The key argument
specifies the index for the board to retrieve. The board argument will be
initialized to reflect the board stored in the database.

Return Value
	 0	board successfully retrieved
	-1	board not allocated, not retrieved
-----------------------------------------------------------------------------*/

int db_retrieve_board
  (key_type key,		/* index into board database */
  board_type *board)		/* board to init from database */
{
if (!GET_BIT (bam, key))	/* if the board is not allocated */
  return (-1);			/* return error code */

get_board (key, board);		/* get the board */

return (0);			/* return success code */
}


/*-----------------------------------------------------------------------------
limb_type far *db_retrieve_limb (key_type key)

This function will retrieve the limb referenced by the key argument. If the key
is out of range, or if the limb is not marked as allocated a NULL pointer
will be returned. If the limb does exist, a far pointer to the limb structure
will be returned.
-----------------------------------------------------------------------------*/

limb_type far *db_retrieve_limb (key_type key)
{
if (key >= 0 && key < max_limb_key)
  if (GET_BIT (lam, key))
    return (&limb_array [key]);

return (NULL);
}


/*-----------------------------------------------------------------------------
STATIC void free_subtree (key_type key);

This function is called by the db_delete_subtree function and recursively
calls itself to remove all siblings and children starting with the child
referenced by the key argument.
-----------------------------------------------------------------------------*/

STATIC void free_subtree (register key_type key)
{
limb_type far *pt;		/* pointer to the limb */
static key_type sibling;	/* copy of the sibling index */

do
  {
  pt = &limb_array [key];
  ((pt->move & BOARD_MASK) ? free_board : free_subtree) (pt->bc.child_key);
  sibling = pt->sibling_key;
  free_limb (key);
  key = sibling;
  }
while (key != NO_KEY);
}


/*-----------------------------------------------------------------------------
int db_delete_subtree (
   key_type parent,
   key_type child)

This function will delete the sub-tree referenced by the index stored in the
child argument. If no limb exists at child, nothing will be deleted.
-----------------------------------------------------------------------------*/

int db_delete_subtree (
   key_type parent,
   register key_type child)
{
limb_type far *ch_pt;		/* pointer to the child limb */
limb_type far *l;		/* pointer to child in chain */
key_type far *left_link;	/* link of previous sibling or parent */
key_type chk;			/* current child of parent */


last_parent_key = NO_KEY;
last_child_key = NO_KEY;


/*------------------------------------------------
Attempt to get a pointer to the child and parent
limbs.  If either is NULL, return indicating an
error.
------------------------------------------------*/

if ((ch_pt = db_retrieve_limb (child)) == NULL)
  return (-1);

if ((l = db_retrieve_limb (parent)) == NULL)
  return (-1);


/*------------------------------------------------
Loop thru the children of the parent searching for
the specified child.  If found, link either the
parent or the previous sibling to the following
sibling.  If no mathing child was found, return
an error.
------------------------------------------------*/

chk = *(left_link = &(l->bc.child_key));
for (; chk != NO_KEY; chk = l->sibling_key)
  {
  l = db_retrieve_limb (chk);

  if (chk == child)
    {
    *left_link = l->sibling_key;
    break;
    }

  left_link = &(l->sibling_key);
  }

if (chk != child)
  return (-1);


/*------------------------------------------------
Now, delete the child and his subtree.
------------------------------------------------*/

((ch_pt->move & BOARD_MASK) ? free_board : free_subtree) (ch_pt->bc.child_key);
free_limb (child);

return (0);
}


/*-----------------------------------------------------------------------------
int db_almost_full (void);

This function returns a value of 1 if there are less than 40 boards or limbs
remainaing unmapped in the database. A value of 0 is returned if there are at
least 40 limbs AND boards unmapped.
-----------------------------------------------------------------------------*/

int db_almost_full (void)
{
return (num_limbs_left < 40 || num_boards_left < 40) ? 1 : 0;
}


/*-----------------------------------------------------------------------------
void init_borders (board_type *bd);

Fill the board boarders with the proper values.
-----------------------------------------------------------------------------*/

void init_borders (board_type *bd)
{
register unsigned char *p;
register unsigned char *end;

memset (bd->board [0], OFF_BOARD | BD_AX | FD_AX | H_AX | V_AX, 10);
memset (bd->board [9], OFF_BOARD | BD_AX | FD_AX | H_AX | V_AX, 10);

end = bd->board [9];
for (p = bd->board [1]; p < end; p+=10)
  {
  p [0] = OFF_BOARD | BD_AX | FD_AX | H_AX | V_AX;
  p [9] = OFF_BOARD | BD_AX | FD_AX | H_AX | V_AX;
  }
}


/*-----------------------------------------------------------------------------
STATIC void get_board (
  key_type key,
  board_type *board);

This function gets the board whose key is key from the database and stores the
board data in the structure pointed to by the board argument.
-----------------------------------------------------------------------------*/

STATIC void get_board (
  key_type key,
  board_type *board)
{
div_t ndx;
dbm_board_type far *dbm_bd;
register int i;

ndx = div (key, bd_per_blk);
dbm_bd = &(board_ptrs [ndx.quot]) [ndx.rem];

for (i=0; i<8; i++)
  *((struct bd_row_st *) &(board->board [i+1][1])) = dbm_bd->board [i];

for (i=0; i<FA_BITS_SIZE; i++)
  board->fa_bits [i] = dbm_bd->fa_bits [i];

init_borders (board);
}

/*-----------------------------------------------------------------------------
STATIC void put_board (
  key_type key,
  board_type *board);

This function stores the board pointed to by the board argument into the
database at the location indicated by the key argument.
-----------------------------------------------------------------------------*/

STATIC void put_board (
  key_type key,
  board_type *board)
{
div_t ndx;
dbm_board_type far *dbm_bd;
register int i;

ndx = div (key, bd_per_blk);
dbm_bd = &(board_ptrs [ndx.quot]) [ndx.rem];

for (i=0; i<8; i++)
  dbm_bd->board [i] = *((struct bd_row_st *) &(board->board [i+1][1]));

for (i=0; i<FA_BITS_SIZE; i++)
  dbm_bd->fa_bits [i] = board->fa_bits [i];
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

