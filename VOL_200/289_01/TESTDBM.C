#include "OTHELLO.H"


void main (void)
{
key_type k01, k11, k12, k21, k22, k23, k24, k25;
board_type board;


db_init ();		/* init the DBM */


k01 = db_add_child (NO_KEY, 0x01, &board, 0);


k11 = db_add_child (k01, 0x11, &board, 0);
k12 = db_add_child (k01, 0x12, &board, 0);


k21 = db_add_child (k11, 0x21, &board, 0);
k22 = db_add_child (k11, 0x22, &board, 0);
k23 = db_add_child (k11, 0x23, &board, 0);

k24 = db_add_child (k12, 0x24, &board, 0);
k25 = db_add_child (k12, 0x25, &board, 0);


db_add_child (k21, 0x31, &board, 10);
db_add_child (k21, 0x32, &board, 20);

db_add_child (k22, 0x33, &board, 11);
db_add_child (k22, 0x34, &board, 17);

db_add_child (k23, 0x35, &board, 25);
db_add_child (k23, 0x36, &board, 50);
db_add_child (k23, 0x37, &board, 10);

db_add_child (k24, 0x38, &board, 30);
db_add_child (k24, 0x39, &board, 27);

db_add_child (k25, 0x3A, &board, 15);
db_add_child (k25, 0x3B, &board, 20);
db_add_child (k25, 0x3C, &board, 29);


find_best_move (k01, 3);
}

