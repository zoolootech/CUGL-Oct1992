/*
	program:	rkst2.c
	created:	03-Nov-83
	by:		A. Skjellum

	modified:	14-Nov-83
	by:		M. J. Roberts
	and:		5-Dec-83
	by:		M. J. Roberts

	modified:	25-Jul-84
	by:		A. Skjellum

	Copyright 1983, 1984 (c) California Institute of Technology.
	All rights Reserved.  This program may be freely distributed
	for all non-commercial purposes but may not be sold.

	purpose:	illustrate the use of RKS program
	update:		to test the rk4n() subroutine using a system
			of two differential equations.

	uses:		rk4n() (rks.c)
	summary:

		integrates the differential equation system:

		u1'(t) = 8u2(t)			u1(0) = 10
		u2'(t) = 2u1(t)			u2(0) = 7

		for which the exact solution is known to be:

		u1(t) = 12exp(4t) - 2exp(-4t)
		u2(t) =  6exp(4t) +  exp(-4t)

	
*/

/* constants */

#define SYSIZE	2	/* number of functions in system */
#define	Y1ZERO	10.0	/* initial value for first equation */
#define Y2ZERO	7.0	/* initial value for other equation  */
#define	TSTART	0.0	/* starting time for integration */
#define	TEND	1.0	/* ending time for integration */
#define	STEPS	50	/* 50 steps in integration */

/* variables external to all functions */

double wvalue[STEPS+1][SYSIZE];
double yarray[STEPS+1][SYSIZE],tarray[STEPS];
	/* integrated solution stored here */


/* subroutines: */

/* exact(): returns exact solution value, given t */

double exact(n,t)
int n;		/*  which equation is it?  0 or 1?  */
double t;

{
	extern double exp();	/* exponential function */

	/*  This must find solutions for both U1 and U2.  The
		exact solutions are given in the header comments to
		this program, above.  */

	switch (n)
	{
		case 0:
			return(12*exp(4*t) - 2*exp(-4*t));
		case 1:
			return( 6*exp(4*t) +   exp(-4*t));
}	}


/* fn(j,i,t,y):  return f(t,y) given t,y values */

double fn(j,i,t,y)
int j;
int i;
double t;
double (*y)();
{
	switch (i)
	{
		case 0:
			/*  u1'(t) = 8 * u2(t)  */
			return(8*(*y)(1,j,i));

		case 1:
			/*  u2'(t) = 2 * u1(t) */
			return(2*(*y)(0,j,i));
	}
}



/* store():  the routine to store away the W values for later reference */

double store(row, col, value)
int row, col;		/*  location to store the value  */
double value;		/*  the actual value to store  */
{
	wvalue[row][col]=value;
	return (value);
}

/* source():  return the  W value referenced by input parameters  */

double source(row,col)
int row, col;		/*  location to look up */

{
	return (wvalue[row][col]);
}

/* solutn(): print solution step at console */


solutn(j,i)
int j,i;	/* element numbers  */
{
	printf("\nt=%7.3e, y%1d=%7.3e, y%1d_exact=%7.3e, diff=%7.3e",
		tarray[j],i,source(j,i),i,exact(i,tarray[j]),
		source(j,i) - exact(i,tarray[j]));
}

/* main program: */

main()
{
	/* external declarations */

	double store(), source();

	double fn();	/* ensure that this is typed as double */

	/* local variables: */

	register int i,j;


	double init[SYSIZE];		/* initial condition matrix */

	/* begin code: */

 	printf("\n\nrkst2.c	as of 25-Jul-84\n\n");
	printf("	Integrates the differential equation system:\n\n");
	printf("	u1'(t) = 8u2(t)			u1(0) = 10\n");
	printf("	u2'(t) = 2u1(t)			u2(0) = 7\n");
	printf("	for which the exact solution is known to be:\n\n");
	printf("	u1(t) = 12exp(4t) - 2exp(-4t)\n");
	printf("	u2(t) =  6exp(4t) +  exp(-4t)\n\n");
	printf("        for     t = %7.3e to %7.3e, with %u steps\n\n",
		TSTART,TEND,STEPS);

	/*
		integrate the answer from t = 0 to t = 10 sec 
		STEPS points.
	*/

	init[0] = Y1ZERO;	/* set up initial condition matrix */
	init[1] = Y2ZERO;

	rk4n(fn,source,store,SYSIZE,TSTART,TEND,STEPS,init,tarray,yarray);
			/* compute the answers for 1 function */

	/*  Print out solutions  */

	for(i=0;i<SYSIZE;i++)
		for(j=0;j<STEPS;j++)	/* print solution */
			solutn(j,i);


	printf("\n\nEnd of execution\n\n");
}
