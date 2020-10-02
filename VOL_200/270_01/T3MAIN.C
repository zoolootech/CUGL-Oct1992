
/*
     HEADER:     CUG270.02;
     TITLE:      TTT3D main program;
     DATE:       06/08/1988;
     VERSION:    1.0;
     FILENAME:   T3MAIN.C;
     SEE-ALSO:   T3.DOC;
     AUTHORS:    Scott Holland;
*/

/* COPYRIGHT 1988  SCOTT HOLLAND */

#include <stdio.h>
#include "t3global.h"

main()
  {
    init();
    intro();
    while (!game_over)
      {
	legal = 0 ;
	while(!legal)
	  {
	    display_board();
            player_move();
	  }
        computer_move();
      }
    display_board();
  }
