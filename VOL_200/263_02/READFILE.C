#include <stdio.h>
#include <c_wndw.h>

/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
*	This program reads the file "TEST.DAT" by record number.
*   If the record exists, it is displayed.
*	The program WRITFILE.C can be used to create the file.
*/

void main ()
{
     long recno = 1L;
     int fh, ret = 1, in;
     clr_scrn ("Read the file");
	 fh = fileopen ("MARIETTA\\TEST.DAT", binary, readonly);
	 if (!fh) goodbye (err_wndw (" TEST.DAT file not found ", 0, 0));
	 if (fh < 0) goodbye (10); /* error end to the program */
     fileinit (fh, 0, 61, 0L); /* set record length at 61 bytes */
	 for (;;)
	 {
          display ("Which record number to read? ", 1, 1, high);
          ret = acptlong (&recno, code, alt_high, 3);
          if (ret == QUIT) break;
          in = fileread (fh, relative, &recno); /* read the record */
          if (in == 1) display (FN[fh].record, 2, 5, reverse);
          else err_wndw (" Record not found ", 0, 9);     
     }
     fileclos (fh);     
     goodbye (0);
}
