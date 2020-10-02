/*-----------------------------------------------------------------------------
OTHELLO.H

This file contains global function prototypes, external variable declarations,
and manifest constant definitions for the othello program.

Revision History
----------------
Jon Ward    17 Oct 1988    Initial Revision.
Jon Ward    30 Oct 1988    Added typedef for move type.
Jon Ward    01 Nov 1988    Added constant defs and macros.
Jon Ward    04 Dec 1988    Changed fa_ndx_struct from a structure to an
                           array.
Jon Ward    12 Dec 1988    Started adding function prototypes and external
                           declarations for global variables.
Gary Culp   15 Dec 1988    Added declaration for delta_array, BELONGS macro.
Gary Culp   19 Dec 1988    Updated prototypes for functions in PROTECT.C
Gary Culp   22 Dec 1988    Added MAX_AFFECTED_PIECES.
Gary Culp    2 Jan 1989    Added STATIC and BT_MAX_DEPTH
Jon Ward     3 Jan 1989    Added prototypes for minimax functions.
			    added prototype for limb removing DB function
Jon Ward     7 Jan 1989    Added last_max_score extern for MINIMAX.C.
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
For debugging purposes, it is sometimes useful to change 'static' functions and
variables to global, so they will appear in MAP files.  This definition makes
that easier to do.  Just define STATIC to be an empty string (For MSC, the
command line option "/DSTATIC=" will do this.) Do not use STATIC to declare
static variables inside functions!  They will turn into automatics, and cease
to work correctly.
-----------------------------------------------------------------------------*/
#ifndef STATIC
#define STATIC static
#endif


/*-----------------------------------------------------------------------------
This constant represents the number of axes on the othello board that can 
contain a capture. There are obviously 8 vertical and 8 horizontal columns and 
rows. There are also 15 diagonal "rows" forward and 15 diagonal "rows" 
backward. However, 4 of each of these 15 are eliminated because a row must 
contain 3 or more cells for a capture to occur. The 2 diagonal "rows" nearest 
each corner have only 1 and 2 cells.
-----------------------------------------------------------------------------*/

#define NUM_CAPTURABLE_AXES (11 + 11 + 8 + 8)


/*-----------------------------------------------------------------------------
The following board structure will contain the definition for a particular
board.

The board element will contain bits (as defined below) that tell whether a cell
is empty or occupied and whether the piece in that cell is protected along an
axis. Once a piece is protected along all 4 axes, it has become permanent and
can never be changed in the remainder of this game.

The fa_bits (fa being full axis) is an array of bits that tell if a particular
axis is completely full.  1 is added for the non-capturable axes and 7 is added
to round up to the nearest byte.
-----------------------------------------------------------------------------*/

#define FA_BITS_SIZE ((NUM_CAPTURABLE_AXES + 1 + 7) / 8)

struct board_struct
  {
  unsigned char board [10][10];
  unsigned char fa_bits [FA_BITS_SIZE];
  };

typedef struct board_struct board_type;


/*-----------------------------------------------------------------------------
Board manifest constants and macros for the board_struct board element.

Note that the first four bits (NO_PIECE thru OFF_BOARD) are mutually exclusive.
Only one of them may be set at a given time.

This bit mapping is NOT arbitrary.  The IS_PERM macro is a good example of why
it is not.  The program exploits the numerical properties of the variables
built with these definitions, as well as extracting bits from them.
-----------------------------------------------------------------------------*/

#define NO_PIECE	0x01	/* Empty cell, No disk there */
#define US_PIECE	0x02	/* Cell occupied by our disk */
#define THEM_PIECE	0x04	/* Cell occupied by their disk */
#define OFF_BOARD	0x08	/* Cell is off the board */
#define BD_AX		0x10	/* Backward diagonal axis protected */
#define FD_AX		0x20	/* Forward diagonal axis protected */
#define H_AX		0x40	/* Horizontal axis protected */
#define V_AX		0x80	/* Vertical axis protected */

#define IS_PERM(cell) ((cell) > (NO_PIECE | BD_AX | FD_AX | H_AX | V_AX))
#define BELONGS(cell,code) ((cell)&(code))

#define TEST_BITS(cell,bits) ((cell) & (bits))
#define SET_BITS(cell,bits) ((cell) |= (bits))
#define CLR_BITS(cell,bits) ((cell) &= ~(bits))


/*-----------------------------------------------------------------------------
Full axis table for FA index table and macros for setting and testing the FA
bits.
-----------------------------------------------------------------------------*/

typedef unsigned char fa_type [4];

#define BD_NDX 0	/* index for backward diagonal FA bit */
#define FD_NDX 1	/* index for forward diagonal FA bit */ 
#define H_NDX  2	/* index for horizontal FA bit */       
#define V_NDX  3	/* index for vertical FA bit */         

#define WHICH_BYTE(ndx) ((unsigned char) (ndx) >> 3)
#define BIT_N_BYTE(ndx) ((unsigned char) 1 << ((ndx) & 0x07))

#define SET_FA_BIT(fa,bit) ((fa) [WHICH_BYTE(bit)] |= BIT_N_BYTE(bit))
#define CHK_FA_BIT(fa,bit) ((fa) [WHICH_BYTE(bit)] & BIT_N_BYTE(bit))


/*-----------------------------------------------------------------------------
The following type is used to represent a number used for indexing into the
database manager when referencing a node (limb) in one of the move trees.
-----------------------------------------------------------------------------*/

typedef unsigned int key_type;

#define NO_KEY ((key_type) 0xFFFF)


/*-----------------------------------------------------------------------------
The following type is used to represent the row and column of an othello move.

Since there are only 8 rows and 8 columns on the othello board, the move can be
reduced to 6 bits, 3 for the row and 3 for the column.  One of the remaining
bits is used to indicate that the player (computer or opponent) has no valid
moves.  The other bit will be used by the DBM to indicate that there is a board
associated with a limb entry.

The GET_ROW/GET_COL macros extract and normalize the row and col stored in a
move_type.

The SET_ROW/SET_COL macros initialize the row and col for a move_type.

The CLR_ROW/CLR_COL macros clear the row and col for a move_type.
-----------------------------------------------------------------------------*/

typedef unsigned char move_type;

#define ROW_MASK	0x07	/* 3 bits for row number */
#define COL_MASK	0x38	/* 3 bits for col number */
#define NO_MOVE_MASK	0x40	/* bit set for no valid moves */
#define BOARD_MASK	0x80	/* bit set in DBM if bottom level of tree */
#define HASNT_MOVED BOARD_MASK /* returned by check_for_input() when
                                  move has not been entered yet */

#define GET_ROW(move) ((move) & ROW_MASK) 
#define GET_COL(move) (((move) & COL_MASK) >> 3)

#define SET_ROW(move,row) ((move) |= ((row) & 0x07))
#define SET_COL(move,col) ((move) |= ((col) & 0x07) << 3)

#define CLR_ROW(move) ((move) &= ~ROW_MASK)
#define CLR_COL(move) ((move) &= ~COL_MASK)

#define SET_ROW_COL(m,r,c) ((m) |= (((c) & 0x07) << 3) | ((r) & 0x07))


/*-----------------------------------------------------------------------------
The limb_type type represents the data that is contained at each branch (limb)
in the move tree.

The bc union is either a key for the board at this limb, or is a key for the
first child of this limb.  The move struct element will have the BOARD_MASK
bit set if the bc element is a board key.  If the BOARD_MASK bit is NOT set,
bc will be the child_key.

A value of NO_KEY indicates that there are no children or siblings.
-----------------------------------------------------------------------------*/

struct limb_struct 
  {
  move_type move;		/* what we did to get here */
  int score;			/* value of this limb */
  key_type sibling_key;		/* key for next sibling of this limb */

  union
    {
    key_type child_key;		/* key for the limb's first child */
    key_type board_key;		/* key for board at this limb */
    } bc;

  };

typedef struct limb_struct limb_type;


/*-----------------------------------------------------------------------------
Maximum depth to build the tree
-----------------------------------------------------------------------------*/
#define BT_MAX_DEPTH 11


/*-----------------------------------------------------------------------------
				   BD_EVAL.C
-----------------------------------------------------------------------------*/
int bd_eval(
   struct board_struct *board_ptr,
   unsigned char which_color);

/*-----------------------------------------------------------------------------
				    BDTTY.C
-----------------------------------------------------------------------------*/
void disp_board (board_type *board);
void disp_row (int row);
void disp_col (int col);
void disp_clr_row_col (void);
void disp_init (void);
void disp_error_msg (char *msg);
void disp_player_move (unsigned char player);
void disp_player_cant_move (unsigned char player);
void disp_player_moved_to (unsigned char player);
void disp_pieces (int us, int them);

extern char us_char;
extern char them_char;


/*-----------------------------------------------------------------------------
				   BUILDLVL.C
-----------------------------------------------------------------------------*/
int build_tree (
   int depth_to_build,
   unsigned char movers_color);

void update_tree_after_our_move (
   key_type our_move);

/*-----------------------------------------------------------------------------
				    FATABL.C
-----------------------------------------------------------------------------*/
extern fa_type fa_tabl [8][8];


/*-----------------------------------------------------------------------------
				    GETMOV.C
-----------------------------------------------------------------------------*/
move_type check_for_input (void);
void init_input_vars (void);


/*-----------------------------------------------------------------------------
				   MINIMAX.C
-----------------------------------------------------------------------------*/
key_type find_best_move (
   key_type root,
   int depth);

key_type find_worst_move (
   key_type root,
   int depth);

extern int last_max_score;	/* score for last tree evaluated */


/*-----------------------------------------------------------------------------
				    MOVEIT.C
-----------------------------------------------------------------------------*/
int verify_move_and_update_board (
  move_type move,
  unsigned char player);

/*-----------------------------------------------------------------------------
				   MOVE_GEN.C
-----------------------------------------------------------------------------*/
int find_move(
   struct board_struct *board_ptr,
   int start_displacement,
   unsigned char movers_color,
   int *affected_list);

/* affected_list needs room for this many entries */
#define MAX_AFFECTED_PIECES 19


/*-----------------------------------------------------------------------------
				    OTHDBM.C
-----------------------------------------------------------------------------*/
void free_limb (key_type limb);
void db_init (void);
void db_kill (void);

key_type db_add_child (
   key_type parent_key,
   unsigned char move,
   board_type *board,
   int score);

int db_retrieve_board (
   key_type key,
   board_type *board);

limb_type far *db_retrieve_limb (
   key_type key);

int db_delete_subtree (
   key_type parent,
   key_type child);

int db_almost_full (void);
void init_borders (board_type *bd);

/*-----------------------------------------------------------------------------
				   OTHELLO.C
-----------------------------------------------------------------------------*/
void main (void);

extern board_type curnt_bd;		/* board -- used for display */
extern key_type curnt_top_limb;		/* key of limb for current board */


/*-----------------------------------------------------------------------------
				   PIECE_CT.C
-----------------------------------------------------------------------------*/
int piece_count(
   struct board_struct *board_ptr,
   unsigned char piece_type);

unsigned char who_can_move(struct board_struct *board_ptr);

/*-----------------------------------------------------------------------------
				   PROTECT.C
-----------------------------------------------------------------------------*/
void update_protection_for_board(
   struct board_struct *board_ptr,
   int *affected_list, /* list of affected pieces, beginning with new piece */
   int num_affected,   /* number of pieces in affected list */
   unsigned char new_color);  /* new color for affected pieces */

void handle_changed_pieces(
   struct board_struct *board_ptr,
   int *affected_list,
   int num_affected,
   unsigned char new_color,
   int prot_check_flag);

/*
   For movement along an axis within a board, add or subtract
   delta_array[axis_number] to a pointer to a cell within the board.
*/
extern const int delta_array[4];


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

