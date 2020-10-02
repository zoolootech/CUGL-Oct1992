#include <stdio.h>
#include <c_wndw.h>

/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
* 	This program shows the use of a simple menu.  There are four options
*	but only three of them are allowed.  When you select an option, a
*	window with the name of the option is displayed.
*/

void main() {
     int ret = 1;
     char *input[21] = {"First choice", "Second choice",  "Third choice"};
	 clr_scrn ("Menu test");
	 display ("Select option from menu:", 2, 2, alt_high);
	 flt_wndw (4, 6, "Menu");
     ret = menu ("First&Second&Third&Fourth|", 6, ret, 1);
     if (ret > 0)   /* Not F10 or ESC */
          err_wndw (input[ret - 1], ret, 0); /* display choice */
     rm_wndw();
     goodbye (0);
}
