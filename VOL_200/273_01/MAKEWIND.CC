make_window(int trow, int tcol, int brow, int bcol, int wattr, int battr, int shadow, char *title, char *footer)
/* Make a window at the specified location.
   trow=upper left row of window   range 0-24
   tcol=upper left col of windoe   range 0-79
   brow=lower right row of window
   bcol=lower right col of window
   wattr=attribute of window
   battr=attribute of border of window
   shadow=0=no shadow =1=make a shadow
   title=title for window
   footer=bottom footer for window
   Needs the following golbal data defined:
   int color, mono, cga, ega, scrseg, bios;
*/

{
	int num_cols = bcol - tcol + 1;
	int title_len, adjust;
	if(shadow) make_shadow(trow,tcol,brow,bcol);
	box(trow,tcol,brow,bcol,wattr,battr);
	title_len=strlen(title) + 2;
	if(title_len < 3) return;
	if(title_len >= num_cols) return;
	adjust=((num_cols - title_len) / 2) + tcol;
	writef(trow,adjust,battr,"[");
	writef(trow,adjust+1,battr,title);
	adjust=adjust + title_len - 1;
	writef(trow,adjust,battr,"]");

	title_len=strlen(footer) + 2;
	if(title_len < 3) return;
	if(title_len >= num_cols) return;
	adjust=((num_cols - title_len) / 2) + tcol;
	writef(brow,adjust,battr,"[");
	writef(brow,adjust+1,battr,footer);
	adjust=adjust + title_len - 1;
	writef(brow,adjust,battr,"]");
}

