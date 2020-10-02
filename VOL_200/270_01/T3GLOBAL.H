/*
     HEADER:     CUG270.09 ;
     TITLE:      TTT3D Include File ;
     DATE:       06/08/1988 ;
     VERSION:    1.0 ;
     FILENAME:   T3GLOBAL.H ;
     AUTHORS:    Scott Holland ;
     SEE-ALSO:   T3.DOC ;
*/

/* COPYRIGHT 1988  SCOTT HOLLAND */

int game_over ;
int board[64] ; /* ttt3d game board */
int win_square[76][4] ; /* All the rows that make up wins */
int sum_count[2][4] ;
int move,move1,move2 ; /* Computers move and probable next move */
int strategy ; /* This indicates the strategy level of the
                  computers move
		  -2 Illegal move
		  -1 Player wishes to quit
		   0 No moves taken - first move for player
		   1 Player wins
		   2 Computer wins
		   3 Computer blocks 3 in a row for player
		   4 Computer gets 3 in a row in two intersecting rows
		   5 Computer blocks two rows with 2 in a row for player
		     with an intersecting blank square
		   6 Computer finds 2 intersecting 1 intersecting 2
		     for computer
		     which will result in 2 intersecting 2 after
		     forcing player to block a win
		   7 Computer finds 2 intersecting 1 intersecting 2
		     for player - computer will block with
		     intersecting square
		   8 Computer finds 2 intersecting 0 intersecting 2
		     for computer
		     which will result in 2 intersecting 1 intersecting
                     2 
		  15 Move taken from move list
		  16 No move found
		*/
int legal ; /* Indicates if players move is legal */
int diag ; /* used to print diagnostics */
int iq ;
