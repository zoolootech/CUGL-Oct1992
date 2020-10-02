scroll_lf(int trow, int tcol, int brow, int bcol, int attr, int no_col)
/* This will scroll the window defined by trow, tcol, brow, bcol to the
   left the number of columns indicated by no_col.  The columns exposed
   after scrolling are set to spaces and are given the attribute specified	
   by attr.
*/
{
	register int x, row, col;
	char hold[160];
	if(bcol - tcol + 1 - no_col < 0) return(1);
	if(!no_col) return(0);
	for(row=trow; row <= brow; row++) {
		for(x=0,col=tcol;col <= bcol;col++) {
			sread_ca(row,col,&hold[x]);
			x += 2;
		}
		for(x=(no_col * 2),col=tcol;col <= (bcol - no_col); col++) {
			swrite_ca(row,col,&hold[x]);
			x += 2;
		}
	}
	scroll_up(trow,bcol-no_col+1,brow,bcol,attr,0);
	return(0);
}
