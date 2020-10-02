/* [SIERP.C of JUGPDS Vol.19]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*	            Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited by Y. Monma (JUG-C/M Disk Editor)                * 
*								*
*****************************************************************
*/

#define MAXLINE 64
#define MAXLEN MAXLINE*8
#define ESC 0x1b
#define H0 512

int	sx, sy, h, x, y;
char	pattarn[MAXLINE][MAXLEN];

main()
{
	int	level;
	char	s;

	printf( "\nLEVEL : " );
	s = getchar();
	level = s -'0';
	initpat();
	writepat(makepat(level));
	finish();
}	


makepat(mlevel)
int	mlevel;
{
	int	n, i, x0, y0;

	i = 0;	h = H0 / 4;	x0 = h + h;	y0 = h + h + h;
	for(i = 1; i <= mlevel; i++) {
		x0 -= h;
		h /= 2;
		y0 += h;
		sx = x = x0; sy = y = y0;
		a(i);	x += h;  y -= h; plot();
		b(i);	x -= h;  y -= h; plot();
		c(i);	x -= h;  y += h; plot();
		d(i);	x += h;  y += h; plot();
	}
	return(MAXLINE);
}


a(i)
int	i;
{
	if (i > 0) {
		a(i-1);	x += h;	y -= h;	plot();
		b(i-1);	x += h + h;	plot();
		d(i-1);	x += h;	y += h;	plot();
		a(i-1);
	}
}


b(i)
int	i;
{
	if (i > 0) {
		b(i-1);	x -= h;	y -= h;	plot();
		c(i-1);	y -= h + h;	plot();
		a(i-1);	x += h;	y -= h;	plot();
		b(i-1);
	}
}


c(i)
int	i;
{
	if (i > 0) {
		c(i-1);	x -= h;	y += h;	plot();
		d(i-1);	x -= h + h;	plot();
		b(i-1);	x -= h;	y -= h;	plot();
		c(i-1);
	}
}


d(i)
int	i;
{
	if (i > 0) {
		d(i-1);	x += h;	y += h;	plot();
		a(i-1);	y += h + h;	plot();
		c(i-1);	x -= h;	y += h;	plot();
		d(i-1);
	}
}


plot()
{
	line(sx,sy,x,y);
	sx = x;
	sy = y;
}



boxplot(x1, x2, y1, y2)
int	x1, x2, y1, y2;
{
	line(x1,y1,x1,y2);
	line(x1,y2,x2,y2);
	line(x2,y2,x2,y1);
	line(x2,y1,x1,y1);
}


line(x1, y1, x2, y2)
int	x1, y1, x2, y2;
{
	unsigned	dxx, dyy, dd;
	int		xx, dx, dy, x, y;

	dy = y2 - y1;
	dx = x2 - x1;
	dyy = abs(dy);
	dxx = abs(dx);
	if (dyy > dxx) {
		if (y1 > y2) {
			y  = y1;	y1 = y2;	y2 = y;
			x  = x1;	x1 = x2;	x2 = x;
		}
		for (y = y1; y < y2; y++) {
			dd = (y - y1);
			xx = (dd * dxx / dyy);
			xx = x2 > x1 ? xx : -xx;
			plotdot(x1 + xx, y);
		}
	}
	else {
		if (x1 > x2) {
			x  = x1;	x1 = x2;	x2 = x;
			y  = y1;	y1 = y2;	y2 = y;
		}
		for (x = x1; x < x2; x++) {
			dd = (x - x1);
			xx = (dd * dyy / dxx);
			xx = y2 > y1 ? xx : -xx;
			plotdot(x, y1 + xx);
		}
	}
}


plotdot(x, y)
int	x, y;
{
	pattarn[ y >> 3 ][ x ] |= ( 128 >> (y & 7) );
}




initpat()
{
	int	i, j;

	outc( ESC );
	outc( 'A' );
	outc(  8  );
	for(i = 0; i < MAXLINE; i++)
		for (j = 0; j < MAXLEN; j++)
			pattarn[i][j] = 0;
}


finish()
{
	outc( ESC );
	outc( '2' );
}



writepat(nlines)
int	nlines;
{
	int	i;

	for (i = 0; i < nlines; i++)
		imageout(pattarn[i]);
}


imageout(line)
char	*line;
{
	int	i, maxlen;

	outc( ESC );
	outc( 'L' );
	outc( MAXLEN & 0x0ff );
	outc( MAXLEN >> 8 );
	maxlen = MAXLEN;
	while (maxlen--)
		outc(*line++);
	outc( '\n' );
}


outc( ch )
char	ch;
{
	bdos(5, ch);
}
