int get_attr(int row, int col)
/* Get the attribute at the specified row and col.
   RETURNED is attr
*/
{
  extern color, mono, cga, ega, scrseg, bios;
  int	attr;
  unsigned int scrofs;
  sread_a(row,col,&attr);
  return(attr);
}
