/* trigplot.c -- point-by-point plot of sine, cosine */
/* 1984 apr 22 pmkrasno */
#include "bdscio.h"
#include "float.h"
#define XSCALE "120.0"
#define YSCALE "90.0"
#define WH 1	/* colors */
#define BL 15
#define wait pause () ; getchar () 

main ()	{	/* read x, y & plot */
	float x[241], y[2*241] ;
	float *sine (), *cosine () ; 
	/* array of pointers to functions returning 
		pointers to float !! */
	float xscale[1], yscale[1] ;
	float ulim[1], llim[1] ;
	float delta[1], sx[1], sy[1], tx[1] ;
	unsigned n, npts, secs1, secs2 ;
	byte time1[13], time2[13], time3[13] ;
	byte dif12[13+5], dif23[13+5] ;

/* cleanup scaling, delta */
	atof (yscale, YSCALE) ;
	atof (xscale, XSCALE) ; fpdiv (xscale, xscale, TWOPI) ;
	atof (delta, XSCALE)  ; fpdiv (delta, TWOPI, delta) ;
	fpasg (llim, MTWOPI)  ; fpasg (ulim, TWOPI) ;
	mode2 () ; cls (WH, BL) ;

	npts = 1 + ftoir (
		fpdiv (tx, fpsub (tx, ulim, llim), delta)) ;
		/* npts = 1 + ((ulim - llim) / delta) ; */
	axes (0, 0) ;

	syst_d (time1) ; 
	printf ("Evaluate %d points: %s\N", 2*npts, time1) ;
	comp_pts (npts, 2, x, y, llim, delta, sine, cosine) ;

	syst_d (time2) ;
	printf ("Plot %d points:     %s\N", 2*npts, time2) ;
	plot_pts (npts, 2, x, y, xscale, yscale) ;

	syst_d (time3) ;
	printf ("Done :               %s\N", time3) ;
	secs1 = tim_dif (dif12, time2, time1) ;
	secs2 = tim_dif (dif23, time2, time3) ;
	/* convert differences to seconds */
	printf ("Evaluate: %d secs; Plot: %d secs\N", 
		secs1, secs2) ;
} /* main */


	unsigned n, npts, secs1, secs2 ;
	byte time1[13], time2[13], time3[13] ;
	byte dif12[13+5], dif23[13+5] ;

/* cleanu