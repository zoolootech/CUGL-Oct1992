/*
 *    uc_glob.h
 *
 *    header file for Unicorn Software Libraries
 *
 *    Copyright (c) 1988 by David A. Miller --> Unicorn software
 *
 */

unsigned _v_start = 0x0b800;  /* Video buffer segment address	  */
			      /* video memory pointer	       */
char far *vid_ptr = ( char far * ) 0x0b8000000;


int	 G_ROWS   = 0;
int	 G_COLS   = 0;
int	 NUM_COLS = 80;       /* Number of text screen columns	  */
int	 NUM_ROWS = 25;       /* Number of text screen rows	  */

int	 CUR_FORE = 7;	      /* Current screen foreground color  */
int	 CUR_BACK = 0;	      /* Current screen background color  */
int	 CUR_ATT  = 0x07;     /* Curent screen attribute	  */


CURSOR uc_cursor;

int mouse_exist = 0;
