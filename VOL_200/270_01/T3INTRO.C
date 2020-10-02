
/*
     HEADER:    CUG270.04 ;
     TITLE:     TTT3D Introduction subroutine;
     DATE:      06/08/1988 ;
     VERSION:   1.0 ;
     FILENAME:  T3INTRO.C ;
     SEE-ALSO:  T3.DOC;
     AUTHORS:   Scott Holland;
*/

/* COPYRIGHT 1988  SCOTT HOLLAND */

#include <stdio.h>
#include "t3global.h"

intro()
  {
    printf("\n");
    printf("This is Three Dimensional Tic-Tac-Toe.\n\n");
    printf("The purpose of the game is to get four squares in a row\n");
    printf("using columns, rows, perpendiculars and diagonals.\n\n");
    printf("To enter your move, enter a three digit number\n");
    printf("representing the level followed by the row followed\n");
    printf("by the column.\n");
    printf("If you wish to skip your move on the first move to allow\n");
    printf("the computer to move first, enter a zero.\n");
    printf("\nIf you wish to quit at any time, type a Q.\n");
    printf("\n\nType ENTER to begin.\n\n");

    while(getc(stdin) != '\n') ; /* Wait for enter */

  }
