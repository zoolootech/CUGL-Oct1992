#include <tcutil.h>
make_shadow(int trow, int tcol, int brow, int bcol)
/* This will create the illusion of a shadow under the window specified by:
   trow=upper left row
   tcol=upper left col
   brow=lower right row
   bcol=lower right col
*/
{
	int shadow = setatr(WHITE,BLACK,0,0);
	int r_leng, c_leng;
	trow++; tcol++; brow++; bcol++;
	c_leng = brow - trow + 1;
	r_leng = bcol - tcol + 1;
	if(brow>24) return(1);
	if(bcol>79) return(1);
	rcolor(brow,tcol,shadow,r_leng);
	ccolor(trow,bcol,shadow,c_leng);
	bcol++;
	if(bcol>79) return(1);
	ccolor(trow,bcol,shadow,c_leng);
	return(0);
}
