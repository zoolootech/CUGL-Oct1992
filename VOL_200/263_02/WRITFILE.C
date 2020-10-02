#include <stdio.h>
#include <c_wndw.h>

/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
*   This sample program creates a binary file with 61 byte size records.
*	The operator is then prompted for entry of a 60 byte string to enter
*	into the file.
*/

void main()
{
	 long recno = 1L;
     int fh, ret = 1, x = 1;
     clr_scrn ("Create a file");
	 fh = fileopen ("MARIETTA\\TEST.DAT", binary, recreate);
     if (fh <= 0) goodbye (10); /* error end to the program */
     fileinit (fh, 0, 61, 0L); /* record length at 61 bytes */
	 for (;;)
	 {
          display ("Enter record: ", x++, 1, high);
          ret = accept (FN[fh].record, left, alt_high, 60, 0);
          if (ret == QUIT || ret == ESC) break;
          if (filewrit (fh, &recno)) break; /* write out the record */
	 }
     fileclos (fh);
	 goodbye (0);
}
