Description of Submitted Materials

Four  video  routines  which  are  dependant  on  IBM  VIDEO  ROM  BIOS  calls.
One of which is cursoroff a routine that turns the cursor off by placing it off
the screen at column 0 and row  25.   Extended  write  many(ewriteM)  writes  a
specific number of characters to the screen at  column  parameter  y1  and  row
parameter x1.  The string cannot be larger then MAXLENGTH which is  defined  in
VIDEO.H. The function fill,  fills the screen with any background or textcolor.
It  starts at screen position y1,x1 and ends at column length+y1  and  row  x2.
The routine xy locates the cursor  at  the  appropriate  cursor  position.  All
routines do not check screen coordinates for correctness.



										Marc Bosser
