
/*
     HEADER:     CUG270.07 ;
     TITLE:      TTT3D display subroutine ;
     DATE:       06/24/1988 ;
     VERSION:    1.0 ;
     FILENAME:   T3DISP.C ;
     AUTHORS:    Scott Holland ;
     SEE-ALSO:   T3.DOC ;
*/

/* COPYRIGHT 1988  SCOTT HOLLAND */

#include <stdio.h>
#include "t3global.h"

char brd_sqr[6] = {'-','X','W',' ',' ','O'} ;
display_board()
{
  int i1,i2,i3 ;
  int j ;
  move = conv_to3(move);
  move2 = conv_to3(move);
  /* clear the screen */
  printf("\n");
  printf("      1XX   2XX   3XX   4XX\n\n");
  j = 0 ;
  for (i1 = 1; i1<5 ; i1++)
  {
    printf("X%1dX   ",i1);
    for (i2 = 1; i2<5 ; i2++)
    {
      for (i3 = 1 ; i3 < 5 ; i3++ )
        putchar(brd_sqr[ board[j++] ] );
      printf("  ");
    }
    lin_dis(i1);
    printf("\n");
  } 
  printf("\n      1234  1234  1234  1234\n");

}

lin_dis(line)
int line ;
{

      switch (strategy)
      {
        case -3 :
        case -2 :
          if (line == 1)
            printf("      Illegal move.");
          if (line == 2)
            printf("      Try again.");
          break ;
        case -1 :
          if (line == 4)
            printf("      Player quits.");
          break ;
        case 0 :
          /* Strategy set to zero at beginning of player.c */
          if (line == 4)
            printf("      New game.");
          break ;
        case 1 :
          if (line == 4)
            printf("      You win !!!");
          break ;
        case 2 :
          if (line == 4)
            printf("     Computer wins !!!");
          break ;
        case 3 :
          if (line == 4)
      	    printf("My move is %3d ",move);
          if (line == 3)
            printf("Strategy : Block 3 in a row.");
          break ;
        case 4 :
          if (line == 4)
      	    printf("My move is %3d ",move);
          if (line == 3)
            printf("Strategy : 3 in row - 2 rows");
          break ;
        case 5 :
          if (line == 4)
      	    printf("My move is %3d ",move);
          if (line == 3)
            printf("Strategy : Block intersecting square 2 x 2");
          break ;
        case 6 :
          if (line == 4)
      	    printf("My move is %3d ",move);
          if (line == 3)
            printf("Strategy %3d %2d C: 212",
                    move2,strategy);
          break ;
        case 7 :
          if (line == 4)
      	    printf("My move is %3d ",move);
          if (line == 3)
            printf("Strategy %3d %2d P: 212",
                    move2,strategy);
          break ;
        case 8 :
          if (line == 4)
      	    printf("My move is %3d ",move);
          if (line == 3)
            printf("Strategy %3d %2d C: 202",
                    move2,strategy);
          break ;
        case 15 :
          if (line == 4)
      	    printf("My move is %3d ",move);
          if (line == 3)
            printf("Strategy : Random square");
          break ;
        default :
          if (line == 4)
      	    printf("My move is %3d ",move);
          if (line == 3)
            printf("Strategy %3d %2d",move2,strategy);
          break ;
      }  
  }
