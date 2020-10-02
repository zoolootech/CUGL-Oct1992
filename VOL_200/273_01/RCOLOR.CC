void rcolor(int row, int col, int attr, int len)
/* This routine will change the color attribute of a row of character.
   row=row to change  range 0-24
   col=beginning col to change	range 0-79
   attr=attribute to change to
   len=num of characters to change
*/
{
  extern color, mono, cga, ega, scrseg, bios;
  int	x,y,orow,ocol;
  if(bios) get_cur(&orow,&ocol);

	for(y=col, x=0; x<len; x++, y++) {
		if(y > 79) break;
		swrite_a(row,y,attr);
	}
  if(bios) locate(orow,ocol);
}
