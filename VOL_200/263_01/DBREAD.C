#include <stdio.h>
#include <c_wndw.h>
#include <c_ndx.h>
/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
*	This program opens a dBASE file and its associated index file and
*   reads sequentially through it displaying the contents of the field
*	LASTNAME from each record.
*/

void main ()
{
     int fh, ndx, ret, x;
/* Initialize and setup */
     clr_scrn ("Indexed read");
	 fh = fileopen ("MARIETTA\\FRIENDS.DBF", dbase3, readonly);
	 if (fh <= 0) goodbye (10); /* file open error */
	 ndx = ndxopen (fh, "MARIETTA\\FRIENDS.NDX");
	 if (ndx <= 0) goodbye (11);
/* read file sequentially through index */
     for (ret = ndxread (ndx, firstrec), x = 1 ; ret > 0 ;
               ret = ndxread (ndx, nextrec), x++ )
          dispdbf (fh, dbffield (fh, "LASTNAME"), x, 1, high);
/* end of program */
	 fileclos (fh); /* also automatically closes the index file */
     display ("Press any key to terminate", x, 40, blink);
     read_kb ();
     goodbye (0);
}
