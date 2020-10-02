#include <stdio.h>
#include <c_wndw.h>
/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
*	This program displays a window containing the message "hello world"
*/

void main ()
{
	 clr_scrn ("Hello world");
	 err_wndw ("          hello world", 0, 0);
     goodbye (0);
}
