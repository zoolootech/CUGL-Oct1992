#include <stdio.h>
#include <c_wndw.h>
/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
* 	This program reads the system date and then allows the date
*	to be amended until the operator presses the F10 or ESC keys.
*/


void main ()
{
	 int ret = 0;
     char date1[3]; /* compressed format date */
	 clr_scrn ("Test date functions");
	 set_keys (1, ESC);
/* get the date field */
     dateread (date1); /* get todays date */
	 do
	 {
		 display ("Enter the date field: ", 2, 2, high);
		 ret = accept (date1, date, alt_high, 8, 0);
	 } while (ret != QUIT && ret != ESC);
/* end the program */
     goodbye (0);
}
