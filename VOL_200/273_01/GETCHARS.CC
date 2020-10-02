int get_chars(int row, int col, int leng, char *string)
/* This will read leng characters from the screen at the location specified
   by row,col and will place the characters (not the attr bytes) into the
   string pointed to by *string.
*/
{
	extern color, mono, cga, ega, scrseg, bios;
	int	i, orow, ocol;
	if (col + leng > 80) return(1);

    if(bios) get_cur(&orow,&ocol);
	for(i=1;i<=leng;i++) {
		sread_c(row,col,string);
		string++; col++;
	}
	if(bios) locate(orow,ocol);
	return(0);
}
