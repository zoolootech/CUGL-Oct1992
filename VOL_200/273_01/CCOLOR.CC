ccolor(int row, int col, int attr, int len)
/* This routine will change the color attributes of a column of characters.
   row=row to start changing color
   col=col to start changing color
   attr=attribute to change to
   len=number of rows down the screen to change.
*/
{
  extern color, mono, cga, ega, scrseg, bios;
  int	i, orow, ocol;

	if(bios) get_cur(&orow,&ocol);

  for (i = 0; i < len; i++)
  {
  	if(row>24) break;
    swrite_a(row,col,attr);
    row++;
  }
  if(bios) locate(orow,ocol);
  return;
}
