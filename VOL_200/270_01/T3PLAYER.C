
/*
     HEADER:     CUG270.05 ;
     TITLE:      TTT3D player move subroutine ;
     DATE:       06/08/1988 ;
     VERSION:    1.0 ;
     FILENAME:   T3PLAYER.C ;
     AUTHORS:    Scott Holland ;
     SEE-ALSO:   T3.DOC;
*/

/* COPYRIGHT 1988 SCOTT HOLLAND */

/* This routine gets the players move and verifies that it is valid */

#include <stdio.h>
#include "t3global.h"

player_move()
  {
    char *gets() ;
    char str[80];
    char *line = &str[0] ;
    int move ;
    int level,column,row ;
    strategy = 0 ;
    legal = 1 ;
    printf("Enter your move : ");
    gets(line);
    /*  printf("%s\n",line); */
    if (*line == 'Q' || *line == 'q')
      {
        strategy = -1 ;
        game_over = 1 ;
        return ;
      };
    if (*line == '0') return ;
    level = cnvtint(line) ;
    *line++;
    row = cnvtint(line) ;
    *line++;
    column = cnvtint(line) ;

    if (diag)
    {
      printf("strat %2d leg %2d level %2d row %2d col %2d\n",
             strategy,legal,level,row,column) ;
    }

    fflush(stdout) ;
    if (legal == 0) return ;
    
    move = conv_move(level,row,column) ;
    if (board[move] != 0)
      { /* if square already taken */
	 strategy = -2 ;
         legal = 0 ;
	 return ;
      }
    board[move] = 1 ;
  }

/* This routine converts a character to an integer */
  
cnvtint(s)
char *s;
{
    if (*s < '1' || *s > '4' ) {
    strategy = -3 ;
    legal = 0 ;
    }
    return( *s -'0');
}

