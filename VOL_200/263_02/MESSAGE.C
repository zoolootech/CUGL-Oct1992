#include <stdio.h>
#include <c_wndw.h>
int test_value (int);
/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
*	function to test range of value, returns positive if OK, or negative
*/
int test_value (int i)
{
     if (i > 23 || i < 1)
          return err_wndw (" Number out of range ", 0, -2);
     else return err_wndw (" Number is OK ", 0, 9);
}

/*
*	Program to show the use of message and query windows
*/
void main ()
{
     int i = 10, x = 1, ret;
     clr_scrn ("Message windows");
	 do
	 {
          display ("Enter value: ", x++, 1, high);
		  ret = acptint (&i, decimal, alt_high, 2);
          if (ret == QUIT) break; /* trap F10 key */
          test_value (i);
     } while ( qry_wndw (" Continue with entry of numbers? ") );
/* end the program */
	 goodbye (0);
}
