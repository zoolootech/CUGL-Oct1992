/* spiral.c -- drawing */
/* 1984 apr 29 pmkrasno */
#include "bdscio.h"
#include "float.h"
#define SCALE "90.0" /* screen points, near full scale */
#define TURNS 19
#define WH 1	/* colors */
#define BL 15
#define NPTS 180
#define wait pause () ; getchar () 

float turns[1], r[1] ;	/* extern */

main ()	{	/* calc x, y & plot */
	float theta[2+NPTS], y[4+2*NPTS], delta, del ;
	float *xcoord(), *ycoord(), scale ;
	unsigned n, nfcns, npts, secs1, secs2 ;
	int nt ;
	byte time1[13], time2[13], time3[13] ;
	byte dif12[13+5], dif23[13+5] ;

	nfcns = 2 ; /* # functions (x, y) */
	fpdiv (del, TWOPI, itof (del, NPTS) ) ;
	mode2 () ; cls (BL, WH) ; 	npts = NPTS + 2 ;

	while (TRUE) {
		do {	printf ("Turns (1-%d) ? ", TURNS) ;
			scanf ("%d", &nt) ;
			} while ( (nt < 1) 
				|| (TURNS < nt) ) ;
		fpmult (delta, del, itof (turns, nt) ) ;
		syst_d (time1) ; 
		printf ("Evaluate %3d points: %s\N", 
			npts, time1) ;
		comp_pts(npts, nfcns, theta, y, ZERO, 
			delta, xcoord, ycoord) ;

		syst_d (time2) ;
		fpdiv (scale, atof (scale, SCALE), r) ;
		printf ("Plot %3d points:     %s\N", 
			npts, time2) ;
		cls (WH, BL) ;
		plot_lins (npts, nfcns-1, y, y+npts, 
			scale, scale) ;

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
	/* r = theta / turns ;	*/
	/* x = r * sin (theta) ;	*/
	fpdiv (r, theta, turns) ;
	fpmult (x, sine (t, theta), r) ;
	return fpchs (x, x) ;
} /* xcoord */

float *ycoord (y, theta) float *y, *theta ;	{
	float *sine(), *cosine(), t ; /* r saved by x */
	/* y = r * cos (theta) ;	*/
	fpmult (y, cosine (t, theta), r) ;
	return fpchs (y, y) ;
} /* ycoord */

;
		cls