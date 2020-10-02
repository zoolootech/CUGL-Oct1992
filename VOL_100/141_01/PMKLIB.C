/* pmklib.c -- personal library -- contents:	1984 Apr 29
	floating-point dump
	utility fcns - find punct, fill str, 
		convert filename, hexdump
	time difference
	evaluate & plot functions
	PM Krasno 17813 Kiowa Trail Los Gatos CA 95030
*/

#include "bdscio.h"
#include "float.h"
#define short char

fpdump (x) char *x ; {	/* value & hex dump */
	printf ("%e: ", x) ; hexdump (x, 5) ;
} /* fpdump */

/* some little utility functions */
int punct (p, s) char *p, *s ;	{ /* find punct char */
	/* returns 1st char from p found in s (or '\0') */
	char *ps ; 
	for ( ; *s ; ++s)
		for (ps = p ; *ps ; ++ps)
			if ( *ps == *s ) return *ps ;
	return 0 ;
} /* punct */

fill (s, n, c) char *s, c ; int n ; { /* fill s with n c's */
	while ( n-- ) *s++ = c ; *s = 0 ;
} /* fill */

fix_name (t, f) char *t, *f ; { /* delete ' ', insert '.' */
	int i ;
	for (i=0 ; i < 8 ; ++i)
		if ( (t[i] = *f++) == ' ' ) break ;
	t[i++] = '.' ;
	while ( *f == ' ' ) f++ ;
	while ( ((t[i] = *f++) != ' ') && ++i < 12 )	;
	t[i] = '\0' ;
} /* fix_name */

spread (t, f) byte *t, *f ;  { /* serarate name.ext */
	int i ;	
	for (i = 0 ; i < 8 ; ++i)
		*t++ = ((*f == '.') || (*f == '\0')) ? 
			' ' : *f++ ;
	if (*f == '.')	f++ ;
	for ( ; i < 11; ++i)
		*t++ = (*f == '\0') ? ' ' : *f++ ;
} /* spread */

hexdump (l, n) byte *l ; int n ; { /* debug tool */
	int i ;
	while ( n )	{
		printf ("%04x: ", l) ;  /* loc */
		for (i = 16 ; i && n ; --i, --n)
			printf ("%02x ", *l++) ;
		printf ("\N") ;
	} /* lines */
} /* hexdump */

unsigned tim_dif (d, t1, t2) char *d, *t1, *t2 ; 
/* time difference in seconds, up to 18hrs or so */ {
	char *t ; int n ; unsigned dif ;
	unsigned val[12] ; /* seconds, each digit-position */
	val[6] = 36000 ;	val[7] =  3600 ; /* hrs */
	val[8] =   600 ;	val[9] =    60 ; /* min */
	val[10] =   10 ;	val[11] =    1 ; /* sec */
 	n = 0 ; /* digit counter */
	while ( t1[n] && t1[n] == t2[n] ) /* leading zeroes */
		++n ; 
	if ( t1[n] < t2[n] ) /* if negative, swap pointers */
		{ t = t1 ; t1 = t2 ; t2 = t  ; }
	if ( n < 6 ) return 65535 ; /* error - day changed */
	for (dif = 0 ; n < 12 ; ++n)
		dif += val[n] * (t1[n] - t2[n]) ;	
	return dif ;
} /* tim_dif */

/* evaluate & plot function(s) */

comp_pts (npts, nfcns, x, y, llim, delta, f0, f1, f2, f3)
int npts, nfcns ;	float *x, *y, *llim, *delta ;
float *(*f0)(), *(*f1)(), *(*f2)(), *(*f3)() ;
{ /* compute array of function-values ; fcns is an array of 
pointers to functions returning pointers to float */
	int f, n, fmax ; 
	float tx[1], *fpmult (), *itof (), (*fptr) (), x0[1] ;
	
	fmax = npts * nfcns ;	fpasg (x0, llim) ;
	for (n = 0 ; n < npts ; ++n, ++x, ++y) {
	    fpadd (x, x0, fpmult (tx, delta, itof (tx, n))) ;
		/* x[n] = x[0] + delta * n ; */
	    /* kludge - can't have array of fcn ptrs ! */
	    /* for (f = 0 ; f < nfcns ; f++)
		y[f][n] = fcn[f] (x[n]) ;	*/
	    (*f0) (y,        x) ; if ( nfcns == 1 ) continue ;
	    (*f1) (y+npts,   x) ; if ( nfcns == 2 ) continue ;
	    (*f2) (y+2*npts, x) ; if ( nfcns == 3 ) continue ;
	    (*f3) (y+3*npts, x) ; if ( nfcns == 4 ) continue ;
	} /* point loop */
} /* comp_pts */

#define _x 128	/* center of screen */
#define _y 96

fpline (x1, y1, x2, y2, xs, ys, c) 
float   *x1, *y1, *x2, *y2, *xs, *ys ; int c ; {
	float sx1, sy1, sx2, sy2 ; int ftoir () ;
	byte  bx1, by1, bx2, by2 ;
	bx1 = _x + ftoir (fpmult (sx1, x1, xs)) ;
	by1 = _y - ftoir (fpmult (sy1, y1, ys)) ;
	bx2 = _x + ftoir (fpmult (sx2, x2, xs)) ;
	by2 = _y - ftoir (fpmult (sy2, y2, ys)) ;
	line (bx1, by1, bx2, by2, -1) ;
} /* fpline */

plot_pts (npts, nfcns, x, y, scx, scy) int npts, nfcns ;
float *x, *y, *scx, *scy ;	{ /* plot points */
	int n, f, fmax ; 	byte bx, by, ftoir() ;
	float *fpmult(), sy[1], sx[1] ;

	fmax = npts * nfcns ;
	for (n = 0 ; n < npts ; ++n, ++x, ++y)	{
	    bx = ftoir (fpmult (sx, scx, x)) + _x; 
	    for (f = 0 ; f < fmax ; f += npts)	{
		by = _y - ftoir (fpmult (sy, scy, y+f)) ;
		set (bx, by, -1) ;
	    } /* fcn loop */
	} /* plot loop */
} /* plot_pts */

plot_lins (npts, nfcns, x, y, scx, scy) int npts, nfcns ;
float *x, *y, *scx, *scy ;	{ /* plot lines */
	int n, f, fmax ;
 	byte bx1, bx2, by1, by2, ftoir() ;
	float *fpmult(), t ;

	fmax = npts * nfcns ;
	bx1 = _x + ftoir (fpmult (t, scx, x) ) ;
	for (f = 0 ; f < fmax ; f += npts)
		by1 = _y - ftoir (fpmult (t, scy, y+f) ) ;
	for (n = 1 ; n < npts ; ++n, ++x, ++y)	{
	    bx2 = _x + ftoir (fpmult (t, scx, x) ) ;
	    for (f = 0 ; f < fmax ; f += npts)	{
		by2 = _y - ftoir (fpmult (t, y+f, scy) ) ;
		line (bx1, by1, bx2, by2, -1) ;
		bx1 = bx2 ;	by1 = by2 ;    } /* fcn loop */
	} /* plot loop */
} /* plot_lins */

axes (x, y) { /* plot axis lines */
	/* x,y in screen coords, relative to _x, _y */
	line (x + _x, 0, x + _x, _y + _y -1, -1) ;
	line (0, _y - y, _x + _x -1, _y - y, -1) ;
} /* axes */
#undef _x
#undef _y

 for (f = 0 ; f