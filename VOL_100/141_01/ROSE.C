/* rose.c -- multi-petal drawing */
/* 1984 apr 28 pmkrasno */
#include "bdscio.h"
#include "float.h"
#define XSCALE "450.0" /* screen points, near full scale */
#define YSCALE "450.0"
#define WH 1	/* colors */
#define BL 15
#define NPTS 180
#define wait pause () ; getchar () 

float pets[1], r[1] ;	/* extern */

main ()	{	/* calc x, y & plot */
	float theta[2+NPTS], y[4+2*NPTS], delta ;
	float *xcoord(), *ycoord() ;
	float xscale[1], yscale[1], *maxx, *maxy ;
	unsigned n, nfcns, npts, secs1, secs2 ;
	int petals ;
	byte time1[13], time2[13], time3[13] ;
	byte dif12[13+5], dif23[13+5] ;

	nfcns = 2 ; /* # functions (x, y) */
	maxx = TEN ;	maxy = TEN ;	/* OOPS */
	atof (yscale, YSCALE) ;	atof (xscale, XSCALE) ;
	fpdiv (delta, TWOPI, itof (delta, NPTS) ) ;
	fpdiv (xscale, xscale, maxx) ;
	fpdiv (yscale, yscale, maxy) ;
	mode2 () ; cls (BL, WH) ; 	npts = NPTS + 2 ;

	while (TRUE) {
		do {	printf ("Petals (3-31) ? ") ;
			scanf ("%d", &petals) ;
			} while ( (petals < 3) 
				|| (31 < petals) ) ;
		itof (pets, petals) ;
		syst_d (time1) ; 
		printf ("Evaluate %3d points: %s\N", 
			npts, time1) ;
		comp_pts(npts, nfcns, theta, y, MHALFPI, 
			delta, xcoord, ycoord) ;

		syst_d (time2) ;
		printf ("Plot %3d points:     %s\N", 
			npts, time2) ;
		cls (WH, BL) ;
		plot_lins (npts, nfcns-1, y, y+npts, 
			xscale, yscale) ;

		syst_d (time3) ;
		printf ("Done :               %s\N", time3) ;
		secs1 = tim_dif (dif12, time2, time1) ;
		secs2 = tim_dif (dif23, time2, time3) ;
		/* convert differences to seconds */
		printf ("Evaluate: %d secs; Plot: %d secs\N", 
			secs1, secs2) ;
	} /* forever */
} /* main */

float *log (res, xin) float *res, *xin ;	{
	int sign ;	float *log10 () ;
	log10 (res, &sign, xin) ;
	if ( sign < 0 ) fpchs (res, res) ;
	return (res) ;
} /* log */

float *xcoord (x, theta) float *x, *theta ;	{
	float *sine(), *cosine(), t ; /* save r for y */
	/* r = 1 + cos (n * theta) ;	*/
	/* x = r * sin (theta) ;	*/
	cosine (r, fpmult (t, pets, theta) ) ;
	fpadd (r, ONE, r) ;
	fpmult (x, sine (t, theta), r) ;
	return x ;
} /* xcoord */

float *ycoord (y, theta) float *y, *theta ;	{
	float *sine(), *cosine(), t ; /* r saved by x */
	/* r = 1 + cos (n * theta) ; 	*/
	/* y = r * cos (theta) ;	*/
	fpmult (y, cosine (t, theta), r) ;
	return y ;
} /* ycoord */

ts, 
			xscale, yscale) ;

		syst_d (time3) ;
	