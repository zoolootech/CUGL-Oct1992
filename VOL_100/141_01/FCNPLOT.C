/* fcnplot.c -- point-by-point plot of function(s) */
/* 1984 apr 25 pmkrasno */
#include "bdscio.h"
#include "float.h"
#define XSCALE "120.0" /* screen points, near full scale */
#define YSCALE "90.0"
#define WH 1	/* colors */
#define BL 15
#define MAXF 8
#define wait pause () ; getchar () 

main ()	{	/* calc x, y & plot */
	float x[241], y[2*241] ;
	float *sine (), *cosine (), *expe (), *log (),
		*exp10 (), *tangent (), *arctan(); 
	float xscale[1], yscale[1], maxx[1], maxy[1] ;
	float ulim[1], llim[1] ;
	float delta[1], sx[1], sy[1], tx[1] ;
	unsigned fno, lkup(), n, npts, secs1, secs2, nfcns ;
	char inpf[60], inpx[60], inpy[60] ;
	char *tbl[MAXF+1] ;
	byte time1[13], time2[13], time3[13] ;
	byte dif12[13+5], dif23[13+5] ;

	tbl[0] = "sin" ;	tbl[1] = "cos" ;
	tbl[2] = "tan" ;	tbl[3] = "atan";
 	tbl[4] = "expe" ;	tbl[5] = "exp10" ;
	tbl[6] = "log" ;	tbl[MAXF] = 0 ;
	nfcns = 1 ; /* # functions */
    while ( TRUE ) {
	do {	printf ("Function ? ") ; scanf ("%s", inpf) ;
		fno = lkup (inpf, tbl) ;
		} while ( fno >= MAXF ) ;
	printf ("Max x ? ") ; scanf ("%s", inpx) ;
	atof (maxx, inpx) ;
	printf ("Max y ? ") ; scanf ("%s", inpy) ;
	atof (maxy, inpy) ;
	atof (yscale, YSCALE) ;	atof (xscale, XSCALE) ;
	fpdiv (delta, maxx, xscale) ;
	fpdiv (xscale, xscale, maxx) ;
	fpdiv (yscale, yscale, maxy) ;
	fpchs (llim, maxx)  ; fpasg (ulim, maxx) ;
	mode2 () ; cls (WH, BL) ;

	npts = 1 + ftoir (
		fpdiv (tx, fpsub (tx, ulim, llim), delta)) ;
		/* npts = 1 + ((ulim - llim) / delta) ; */
	axes (0, 0) ;

	syst_d (time1) ; 
	printf ("Evaluate %3d points: %s\N", 
		nfcns * npts, time1) ;
	comp_pts (npts, nfcns, x, y, llim, delta, 
		fno == 0 ? sine :	fno == 1 ? cosine :
		fno == 2 ? tangent :	fno == 3 ? arctan :
		fno == 4 ? expe :	fno == 5 ? exp10 :
		log ) ;
		/* gawd, that's hideous - but I can't use an
			array of pointers to functions. */

	syst_d (time2) ;
	printf ("Plot %3d points:     %s\N", 
		nfcns * npts, time2) ;
	plot_pts (npts, nfcns, x, y, xscale, yscale) ;

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
	if ( sign < 0 ) fpchs (res,res) ;
/*
printf ("log10 (%f) = %f\N", xin, res) ;
*/
	return (res) ;
} /* log */

unsigned lkup (str, tbl) char *str, **tbl ; { 
/* lookup str in tbl, return number */
	char *s, *t ;	int n ;
	for (n = 0 ; **tbl ; ++tbl, ++n) /* each entry */
		for (s = str, t = *tbl ; ; ++s, ++t)
			if ( *s == '\0' ) return n ; /* hit */
			else if ( toupper(*s) != toupper(*t) )
				break ;	/* miss, try next */
} /* lkup */

f pointers to functions. */

	