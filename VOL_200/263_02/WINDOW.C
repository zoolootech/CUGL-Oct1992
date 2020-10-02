#include <stdio.h>
#include <c_wndw.h>

/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
*	This program shows the creation and removal of overlapping windows.
*	A short delay is inserted between each window made and removed,
*	otherwise the action will be too quick to see.
*/

void main ()
{
     int  x, y; /* (x,y) forms the coordinates of the top left corner */
     clr_scrn ("Test of windows");
/* create overlapping windows until the screen is full */
	 for (x = y = 4 ; x + 6 < 25 ; x += 3, y += 10 )
	 {
          mk_wndw (x, y, x + 6, y + 16, "Test window");
          display (" hello world ", 1, 1, alt_high);
		  idleloop (10); /* 18 clock ticks to the second */
     }
/* Remove the windows one at a time */
	 while (rm_wndw() > 0) idleloop (10);
/* End the program */
     goodbye (0);
}
