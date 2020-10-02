#include <stdio.h>
#include <c_wndw.h>
#include <c_ndx.h>

/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
* 	This program demonstrates the use of the mony functions to enter
*	and display monetary figures with two decimal places.
*	This is a simple speadsheet function with entry of quantity and
*	unit price fields.  The program figures the extended price and
*	keeps a running total.
*/

void main ()
{
     mony ext_price, price = 0, total = 0;
	 int quantity = 0, x, ret;
	 clr_scrn ("Show mony fields");
/* Build column headings */
	 display ("Quantity    Price  Ext Price       Total", 1, 1, high);
/* Accept columns */
	 for (x = 2, ret = ENTER ; ret != QUIT ; x++)
	 {
          set_crsr (x, 3);
		  ret = acptint (&quantity, c_number, alt_high, 4);
          if (ret == QUIT) break; else set_crsr (x, 10);
		  ret = acptmony (&price, c_number, alt_high, 4);
          if (ret == QUIT) break;
		  ext_price = quantity * price; /* extended price */
          dispmony (ext_price, x, 19, reverse, c_number, 6);
		  total += ext_price; /* running total */
          dispmony (total, x, 31, reverse, c_number, 7);
     }
     goodbye (0);
}
