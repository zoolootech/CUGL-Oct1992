
/*
     HEADER:    CUG270.03 ;
     TITLE:     TTT3D initialization subroutine;
     DATE:      06/08/1988;
     VERSION:   1.0;
     FILENAME:  T3INIT.C;
     AUTHORS:   Scott Holland;
     SEE-ALSO:  T3.DOC;
*/

/* COPYRIGHT 1988  SCOTT HOLLAND */

#include <stdio.h>
#include <stdlib.h>
#include "t3global.h"

init()
  {
    int i,j,k,win ;
    srand(2);  /* randomize seed */
    diag = 0 ; /* turn on diagnostics */
    strategy = 0 ; /* Indicates beginning of game */
    game_over = 0 ; /* Indicates game in progress */
    for ( i = 0 ; i<64 ; i++ ) board[i] = 0 ; /* clear board */
    win = 0;
    /* Winning combinations for major diagonals */
    for (i=1;i<5;i++)
      {
		win_square[win][i-1]=conv_move(i,i,i);
		win_square[win+1][i-1]=conv_move(i,i,5-i);
		win_square[win+2][i-1]=conv_move(i,5-i,i);
		win_square[win+3][i-1]=conv_move(i,5-i,5-i);
      }
      win = win + 4 ;
    /* All winning combinations on each level */
    for (i=1;i<5;i++)
      {
	for (j=1;j<5;j++)
	  {
	    for (k=1;k<5;k++) /* four rows, four columns */
	      {
		win_square[win][k-1]=conv_move(i,j,k);
		win_square[win+1][k-1]=conv_move(i,k,j);
	      }
	      win = win + 2 ;
	  }
	/* diagonals */
	for (j=1;j<5;j++)
	  {
		win_square[win][j-1]=conv_move(i,j,j);
		win_square[win+1][j-1]=conv_move(i,j,5-j);
	  }
	  win = win + 2 ;
      }
      /* Perpendicular Four level wins */
      for (i=1;i<5;i++) /* i is row */
	{
	  for (j=1;j<5;j++) /* j is column */
	    {
	      for (k=1;k<5;k++) /* k is level */
		{
		  win_square[win][k-1]=conv_move(k,i,j);
		}
		win = win + 1 ;
	    }
	}
	/* Diagonal Four level wins */
	for (i=1;i<5;i++) /* i is row,column */
	  {
		for (k=1;k<5;k++) /* k is level */
		  {
		    win_square[win][k-1]=conv_move(k,i,k);
		    win_square[win+1][k-1]=conv_move(k,i,5-k);
		    win_square[win+2][k-1]=conv_move(k,k,i);
		    win_square[win+3][k-1]=conv_move(k,5-k,i);
		  }
		  win = win + 4 ;
	  }
  }

