/* circle.c -- complete graph on N pts */
/* 1984 apr 21 pmkrasno */
#include "bdscio.h"
#include "float.h"
#define XSCALE "9.0"	/* screen points for 1.0 value */
#define YSCALE "9.0"
#define WH 1	/* colors */
#define BL 15
#define PTS 61
#define wait pause () ; getchar () 

main ()	{	/* read x, y & plot */
	float th[PTS], xy[2*PTS], fpnum[1], rad[1] ;
	float *sine (), *cosine () ; 
	float xscale[1], yscale[1] ;
	float ulim[1], llim[1], delta[1] ;
	unsigned i, j, n, npts, secs1, secs2, siz ;
	byte time1[13], time2[13], time3[13] ;
	byte dif12[13+5], dif23[13+5] ;

		/* initializers */
	fpasg (llim, MHALFPI)  ; fpadd (ulim, llim, TWOPI) ;
	mode2 () ; cls (WH, BL) ;

	for (;;) {	/* forever */
		printf ("Size (1-10), Number (3-%d) ? ", PTS);
		if ( scanf ("%d, %d", &siz, &npts) < 2 ||
		    siz<1 || 10<siz || npts<3 || PTS<npts )
			exit () ;
		itof (xscale, 9 * siz) ; fpasg (yscale,xscale);
		fpdiv (delta, fpsub (delta, ulim, llim),
			itof (fpnum, npts) ) ;

		syst_d (time1) ; 
		printf ("Evaluate %3d points: %s\N", 
			2*npts, time1) ;
		comp_pts (npts, 2, th, xy, 
			llim, delta, cosine, sine) ;

		syst_d (time2) ; cls (WH, BL) ;
		printf ("Plot %2d lines:      %s\N", 
			npts*(npts-1)/2, time2) ;
		for (i = 0 ; i < npts - 1 ; ++i )
		    for (j = i+1 ; j < npts ; ++j)
			fpline (xy+i, xy+i+npts, 
				xy+j, xy+j+npts,
				xscale, yscale, -1) ;

		syst_d (time3) ;
		printf ("Done :               %s\N", time3) ;
		secs1 = tim_dif (dif12, time2, time1) ;
		secs2 = tim_dif (dif23, time2, time3) ;
		/* convert differences to seconds */
		printf ("Evaluate: %d secs; Plot: %d secs\N", 
			secs1, secs2) ;
	} /* do forever */
} /* main */



	for (;;) {	/* forever */
		printf ("Size (1-10), Number (3-%d) ? ", 