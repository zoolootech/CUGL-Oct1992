void box(int trow, int tcol, int lrow, int lcol, int wattr, int battr)
/* This will draw a box using upper left row,col and lower right row,col
   wattr is attribute character for center of box, battr is the border attr.	
*/
{
	int i, j, len;

	writef(trow,tcol,battr,"Ú");
	writef(trow,lcol,battr,"¿");
	writef(lrow,tcol,battr,"À");
	writef(lrow,lcol,battr,"Ù");
	for(i=tcol +1;i<lcol;i++) {
		writef(trow,i,battr,"Ä");
		writef(lrow,i,battr,"Ä");
	}

	for(i=trow + 1;i<lrow;i++) {
		writef(i,tcol,battr,"³");
		writef(i,lcol,battr,"³");
	}
	clrbox(trow,tcol,lrow,lcol,wattr);


}
