/*
	program:	rktest1.c
	created:	03-Nov-83
	by:		A. Skjellum

	Copyright 1983, 1984 (c) California Institute of Technology.
	All rights Reserved.  This program may be freely distributed
	for all non-commercial purposes but may not be sold.

	updated:	16-Nov-83
	purpose:	illustrate the use of rk4 program

	uses:		rk4.c

	summary:
		integrates the differential equation:

		y'(t) + y(t) = t + 1
		y(0) = 5.0.

		for which the exact solution:

		y(t) = t + 5exp(-t) is known.

*/

/* constants */

#define	YZERO	5.0	/* initial value for y */
#define	TSTART	0.0	/* starting time for integration */
#define	TEND	10.0	/* ending time for integration */
#define	STEPS	80	/* 40 steps in integration */



/* subroutines: */

/* exact(): returns exact solution value, given t */

double exact(t)
double t;
{
	extern double exp();	/* exponential function */

	if(t)
		return((t + YZERO*exp(-t)));

	return(YZERO);
}

/* fn(t,y): return f(t,y) given t,y values */

double fn(t,y)
double t;
double y;
{
	/*
		differential equation is y' + y = t + 1

		therefore, f = t + 1 - y.

	*/

	return(t + 1.0 - y);
}

/* solutn(): print solution step at console */

solutn(t,y)
double *t;	/* pointer to t value */
double *y;	/* pointer to y value */
{
	printf("t = %7.3e, y = %7.3e, y_exact = %7.3e, diff = %7.3e\n",
		*t,*y,exact(*t),*y - exact(*y));
}

/* main program: */

main()
{
	/* external declarations */

	double fn();	/* ensure that this is typed as double */

	/* local variables: */

	register int i;

	double yarray[STEPS],tarray[STEPS];
		/* integrated solution stored here */


	/* begin code: */

	printf("\n\nrktest1.c	as of 03-Nov-83\n\n");
	printf("Integrates: y' + y = 1 + t     for\n\n");
	printf("t = %7.3e to %7.3e, with %u steps\n\n",
		TSTART,TEND,STEPS);

	/*
		integrate the answer from t = 0 to t = 10 sec 
		80 points.
	*/

	rk4(fn,TSTART,TEND,STEPS,YZERO,tarray,yarray);
			/* compute the answers */


	for(i=0;i<STEPS;i++)	/* print solution */
		solutn(tarray+i,yarray+i);


	printf("\n\nEnd of execution\n\n");
}
