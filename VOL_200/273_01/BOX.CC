void box(int trow, int tcol, int lrow, int lcol, int wattr, int battr)
/* This will draw a box using upper left row,col and lower right row,col
   wattr is attribute character for center of box, battr is the border attr.	
*/
{
	int i, j, len;

	writef(trow,tcol,battr,"�");
	writef(trow,lcol,battr,"�");
	writef(lrow,tcol,battr,"�");
	writef(lrow,lcol,battr,"�");
	for(i=tcol +1;i<lcol;i++) {
		writef(trow,i,battr,"�");
		writef(lrow,i,battr,"�");
	}

	for(i=trow + 1;i<lrow;i++) {
		writef(i,tcol,battr,"�");
		writef(i,lcol,battr,"�");
	}
	clrbox(trow,tcol,lrow,lcol,wattr);


}
